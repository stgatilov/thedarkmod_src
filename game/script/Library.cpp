/*****************************************************************************
The Dark Mod GPL Source Code

This file is part of the The Dark Mod Source Code, originally based
on the Doom 3 GPL Source Code as published in 2011.

The Dark Mod Source Code is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version. For details, see LICENSE.TXT.

Project: The Dark Mod (http://www.thedarkmod.com/)

******************************************************************************/

#include "../Game_local.h"
#include "../DarkModGlobals.h"
#include "Library.h"
#include <dlfcn.h>

//===============================================================================
// Library
//===============================================================================

idList<Library*> Library::libraries;
idHashIndex Library::libraryHash;

typedef void (*idClassCallback)( void );

idClass *Library::CreateInstance( void ) {
	try {
		Library *ptr = new Library();
		/*ptr->FindUninitializedMemory();*/
		return ptr;
	}
	catch( idAllocError & ) {
		return NULL;
	}
}

idTypeInfo *Library::GetType( void ) const {
	return Type;
}

const idEventDef EV_TDM_Library_GetLibraryCount( "GetLibraryCount", EventArgs(), 'f', "Returns the number of loaded pre-compiled mod libraries." );

/*
================
Library::Library
================
*/
Library::Library(bool mgr) : name(""), path(""), mgr(mgr), shdHandle(0), Type(NULL), libraryModule(NULL) {
	if (mgr) {
		Type = new idTypeInfo(
			"Library", "idClass",
			( idEventFunc<idClass> * )Library::eventCallbacks,
			Library::CreateInstance,
			( void ( idClass::* )( void ) )&Library::Spawn,
			( void ( idClass::* )( idSaveGame * ) const )&Library::Save,
			( void ( idClass::* )( idRestoreGame * ) )&Library::Restore
		);
	}
	functions.SetGranularity( 1 );
}

Library::~Library() {
	if (shdHandle) {
		Sys_DLL_Unload( shdHandle );
	}
}

void Library::Construct(idStr _name, bool _mgr)
{
	DM_LOG(LC_FUNCTION, LT_DEBUG)LOGSTRING("library: %08lX %s\r", this, __FUNCTION__);

	name = _name;
	assert(mgr == _mgr);
	if (_mgr) {
		assert(name == "(manager)");
		libraryNumber	= -1;
	} else {
		libraryNumber	= GetLibraryCount();
		libraries.Append(this);
		AddLibraryToHash(name, this);
		gameLocal.program.SetLibrary(name, this);
	}
}

const function_t* Library::GetLibraryFunction(int libraryNumber, int functionNumber) {
	// TODO: errors
	return libraries[libraryNumber]->GetFunction(functionNumber);
}

int Library::GetLibraryCount() {
	return libraries.Num();
}

void Library::Event_GetLibraryCount() {
	idThread::ReturnInt(GetLibraryCount());
}


idEventFunc<Library> Library::eventCallbacks[] = {
	EVENT( EV_TDM_Library_GetLibraryCount,					Library::Event_GetLibraryCount )
};

/*
=============
Library::FindLibrary

Returns the entity whose name matches the specified string.
=============
*/
Library *Library::FindLibrary( const char *name ) {
	int hash, i;

	hash = libraryHash.GenerateKey( name, true );
	for ( i = libraryHash.First( hash ); i != -1; i = libraryHash.Next( i ) ) {
		if ( libraries[i] && libraries[i]->name.Icmp( name ) == 0 ) {
			return libraries[i];
		}
	}

	return NULL;
}

Library* Library::AddLibrary(const char *name) {
	Library *library = FindLibrary(name);
	if (!library) {
		library = (Library*)Library::CreateInstance();
		library->Construct(name, false);
	}
	return library;
}

void Library::DebugInfo() {
	if (mgr) {
		int count = GetLibraryCount();
		gameLocal.Printf("Libraries: [MGR] there are %d libraries loaded\n", count);
		if (count) {
			for ( Library** li = libraries.begin(); li != libraries.end(); ++li ) {
				(*li)->DebugInfo();
			}
		}
	} else {
		int count = functions.Num();
		gameLocal.Printf("Libraries: [%s] has %d functions\n", name.c_str(), count);
	}
}

/*
================
Library::AddLibraryToHash
================
*/
bool Library::AddLibraryToHash( const char *name, Library *library ) {
	if ( FindLibrary( name ) ) {
		printf( "Multiple libraries named '%s'", name );
		return false;
	}
	libraryHash.Add( libraryHash.GenerateKey( name, true ), library->libraryNumber );
	return true;
}

/*
================
Library::RemoveLibraryFromHash
================
*/
bool Library::RemoveLibraryFromHash( const char *name, Library *library ) {
	int hash, i;

	hash = libraryHash.GenerateKey( name, true );
	for ( i = libraryHash.First( hash ); i != -1; i = libraryHash.Next( i ) ) {
		if ( libraries[i] && libraries[i] == library && libraries[i]->name.Icmp( name ) == 0 ) {
			libraryHash.Remove( hash, i );
			return true;
		}
	}
	return false;
}

/*
================
Library::NumFunctions
================
*/
int Library::NumFunctions( void ) const {
	return functions.Num();
}

/*
================
Library::GetFunctionNumber
================
*/
int Library::GetFunctionNumber( const function_t *func ) const {
	int i;

	for( i = 0; i < functions.Num(); i++ ) {
		if ( functions[ i ] == func ) {
			return i;
		}
	}
	return -1;
}

/*
================
Library::GetFunction
================
*/
const function_t *Library::GetFunction( int funcNumber ) const {
	assert( funcNumber >= 0 );
	assert( funcNumber < functions.Num() );
	return functions[ funcNumber ];
}

/*
============
Library::Error

Aborts the currently executing function
============
*/
void Library::Error( const char *fmt, ... ) const {
	va_list argptr;
	char	text[ 1024 ];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	common->Error( "Library '%s': %s\n", name.c_str(), text );
}

/*
================
Library::AddFunction
================
*/
void Library::AddFunction( const function_t *func ) {
	int i;

	for( i = 0; i < functions.Num(); i++ ) {
		if ( !strcmp( functions[ i ]->def->Name(), func->def->Name() ) ) {
			if ( func->def->TypeDef()->MatchesVirtualFunction( *functions[ i ]->def->TypeDef() ) ) {
				// Keep the original, so we do not overwrite a loaded function
				// We let this get to here to make sure that we do not have an incompatible alternative
				// definition.
				return;
			} else {
				Error("Tried to add two different functions called '%s' in library '%s'", func->def->Name(), name.c_str());
			}
		}
	}
	functions.Append( func );
}

void Library::LoadAll() {
	for ( Library* library : libraries ) {
		library->Load();
	}
}

typedef union {
	eventCallback_t cb;
	struct {
		uintptr_t ptr;
		ptrdiff_t adj;
	};
} ptmf_t;

typedef void (*myfunc)();

void Library::Load() {
	idFile *fp;

	if (path == "") {
		Error("No library definition script was ever loaded for this library");
	}

	for ( const function_t* func : functions ) {
		if ( func->def->initialized == idVarDef::uninitialized ) {
			Error("Function '%s' was found in a calling module but no definition was ever loaded", func->Name());
		} else if ( func->def->initialized != idVarDef::stackVariable ) {
			Error("Function '%s' was loaded but is in the wrong state - perhaps it was loaded already?", func->Name());
		}
	}

	libraryModule = new LibraryModule(this, path, "test");
	libraryModule->Load(functions, &functionCallbacks);

	Type = new idTypeInfo(
		name.c_str(), "idClass", // Not inheriting from Library as we do not inherit the events.
		functionCallbacks.Ptr(),
		Library::CreateInstance,
		( void ( idClass::* )( void ) )&Library::Spawn,
		( void ( idClass::* )( idSaveGame * ) const )&Library::Save,
		( void ( idClass::* )( idRestoreGame * ) )&Library::Restore
	);
	Type->Init();
}

#include <iostream>
#include <dlfcn.h>

template <typename T>
void LibraryModule::LoadFunction(T* funcPtr, const char* name) {
	idStr fnName = name;
	uintptr_t fnHandle = reinterpret_cast<uintptr_t>(Sys_DLL_GetProcAddress( fh, fnName ));

	if ( !fnHandle ) {
		Sys_DLL_Unload( fh );
		fh = 0;
		Error( "couldn't find library DLL function: '%s' in DLL '%s'", fnName.c_str(), dllName.c_str() );
	}
	*funcPtr = *(reinterpret_cast<T*>(&fnHandle));
}

/*
============
LibraryModule::Error

Aborts the currently executing function
============
*/
void LibraryModule::Error( const char *fmt, ... ) const {
	va_list argptr;
	char	text[ 1024 ];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	common->Error( "Library Module '%s': %s\n", dllName.c_str(), text );
}

void LibraryModule::Load(idList<const function_t*> requestedFunctions, idList<idEventFunc<idClass>>* functionCallbacks) {
	char dllPath[ MAX_OSPATH ];

	strcpy(dllPath, path.StripFilename().c_str());
	idLib::fileSystem->FindDLL( dllName, dllPath, true );

	if ( !dllPath[0] ) {
		Error( "couldn't find game dynamic library from '%s/%s'", dllPath, path.StripPath().c_str() );
		return;
	}

	fh = idLib::sys->DLL_Load( dllPath );
	if ( !fh ) {
		Error( "couldn't load game dynamic library from '%s'", dllPath );
		return;
	}

	int i = 0;
	functionCallbacks->Resize(requestedFunctions.Num() + 1);

	LoadFunction(&Initialize, "trm__initialize");
	LoadFunction(&Deinitialize, "trm__deinitialize");
	LoadFunction(&GetModuleName, "trm__module_name");
	const char* moduleName = (parentLibrary->*GetModuleName)();

	char functionName[2000] = "";
	for ( const function_t** func = requestedFunctions.begin() ; func != requestedFunctions.end() ; func++ ) {
		// This is a hacky workaround for the fact we do not want to pass `this` to
		// a library function. Nicer would require changing Class.cpp to use generic functions.
		idStr fnName = (*func)->Name();
		fnName = fnName.Split({ ":" }, true).Last();
		sprintf(functionName, "trm__usermod__%s__%s", moduleName, fnName.c_str());
		eventCallback_t functionHandle;
		LoadFunction(&functionHandle, functionName);
//
		(*functionCallbacks)[i] = idEventFunc<idClass> {
			(*func)->eventdef,
			functionHandle
		};
		i++;
	}

	(*functionCallbacks)[requestedFunctions.Num()] = {NULL, NULL};

	(parentLibrary->*Initialize)();
}

LibraryModule::~LibraryModule() {
	if (fh != 0) {
		if (Deinitialize) {
			(parentLibrary->*Deinitialize)();
		}
		Sys_DLL_Unload( fh );
		fh = 0;
	}
}
