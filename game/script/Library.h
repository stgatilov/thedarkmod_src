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

#ifndef _LIBRARY_H
#define _LIBRARY_H

#include "../gamesys/Class.h"

typedef bool (idClass::*boolFunc) ();
typedef const char* (idClass::*strFunc) ();
class Library;

class LibraryModule {
	public:
		LibraryModule(Library* library, idStr _path, idStr _dllName) : parentLibrary(library), path(_path), dllName(_dllName), fh(0) {}
		~LibraryModule();
		void Load(idList<const function_t*> requestedFunctions, idList<idEventFunc<idClass>>* functionCallbacks);

	private:
		Library *parentLibrary;
		idStr path;
		idStr dllName;
		uintptr_t fh;
		boolFunc Initialize;
		strFunc GetModuleName;
		boolFunc Deinitialize;
		template <typename T>
		void LoadFunction(T* funcPtr, const char* name);
		void Error( const char *fmt, ... ) const id_attribute((format(printf,2,3)));
};

class Library : public idClass
{
public:
	int GetLibraryCount();
	idTypeInfo							*Type;
	static	idClass							*CreateInstance( void );
	virtual	idTypeInfo						*GetType( void ) const override;
	static idEventFunc<Library>					eventCallbacks[];
	void								LoadAll( void );
	void								Load( void );
	int								libraryNumber;
	idStr								path;
	idStr								name;
	Library*							AddLibrary( const char *name );
	void								DebugInfo();
	bool								mgr;
	const function_t						*GetLibraryFunction(int libraryNumber, int functionNumber);
	int					NumFunctions( void ) const;
	int					GetFunctionNumber( const function_t *func ) const;
	const function_t	*GetFunction( int funcNumber ) const;
	void				AddFunction( const function_t *func );
	void				Error( const char *fmt, ... ) const id_attribute((format(printf,2,3)));
	idList<const function_t *>					functions;
	static	idHashIndex						libraryHash;				// hash table to quickly find entities by name
	static	Library*						FindLibrary( const char *name );
	static	bool							AddLibraryToHash( const char *name, Library *library );
	static	bool							RemoveLibraryFromHash( const char *name, Library *library );
	static	idList<Library *>					libraries;	// index to entities

	Library(bool mgr = false);
	~Library();
	void Construct(idStr name, bool mgr = false);

	ID_INLINE void Spawn() {}
	ID_INLINE void Save(idSaveGame *savefile) {}
	ID_INLINE void Restore( idRestoreGame *savefile ) {}

private:
	uintptr_t shdHandle;
	LibraryModule* libraryModule;

protected:
	void Event_GetLibraryCount();
	idList<idEventFunc<idClass>>					functionCallbacks;

};

#endif /* _LIBRARY_H */
