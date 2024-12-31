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

//===============================================================================
// Library
//===============================================================================

idTypeInfo Library::Type( "Library", "idClass",
	( idEventFunc<idClass> * )Library::eventCallbacks,	Library::CreateInstance, ( void ( idClass::* )( void ) )&Library::Spawn,
	( void ( idClass::* )( idSaveGame * ) const )&Library::Save, ( void ( idClass::* )( idRestoreGame * ) )&Library::Restore );
idList<Library*> Library::libraries;
idHashIndex Library::libraryHash;

idClass *Library::CreateInstance( void ) {
	try {
		Library *ptr = new Library;
		/*ptr->FindUninitializedMemory();*/
		return ptr;
	}
	catch( idAllocError & ) {
		return NULL;
	}
}

idTypeInfo *Library::GetType( void ) const {
	return &( Library::Type );
}

const idEventDef EV_TDM_Library_GetLibraryCount( "GetLibraryCount", EventArgs(), 'f', "Returns the number of loaded pre-compiled mod libraries." );

/*
================
Library::Library
================
*/
Library::Library()
{
	DM_LOG(LC_FUNCTION, LT_DEBUG)LOGSTRING("library: %08lX %s\r", this, __FUNCTION__);

	libraryNumber	= -1;
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
;
