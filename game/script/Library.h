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

class Library : public idClass
{
public:
	int GetLibraryCount();
	static	idTypeInfo						Type;
	static	idClass							*CreateInstance( void );
	virtual	idTypeInfo						*GetType( void ) const override;
	static	idEventFunc<Library>		eventCallbacks[];
	int								libraryNumber;
	idStr								name;
	static	idHashIndex						libraryHash;				// hash table to quickly find entities by name
	static	Library*						FindLibrary( const char *name );
	static	bool							AddLibraryToHash( const char *name, Library *library );
	static	bool							RemoveLibraryFromHash( const char *name, Library *library );
	static	idList<Library *>					libraries;	// index to entities

	Library();

	ID_INLINE void Spawn() {}
	ID_INLINE void Save(idSaveGame *savefile) {}
	ID_INLINE void Restore( idRestoreGame *savefile ) {}

protected:
	void Event_GetLibraryCount();

};

#endif /* _LIBRARY_H */
