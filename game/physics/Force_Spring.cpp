/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod (http://www.thedarkmod.com/)
 
 $Revision$ (Revision of last commit) 
 $Date$ (Date of last commit)
 $Author$ (Author of last commit)
 
******************************************************************************/

#include "precompiled_engine.h"
#pragma hdrstop

static bool versioned = RegisterVersionedFile("$Id$");

#include "../Game_local.h"

CLASS_DECLARATION( idForce, idForce_Spring )
END_CLASS

/*
================
idForce_Spring::idForce_Spring
================
*/
idForce_Spring::idForce_Spring( void ) {
	Kstretch		= 100.0f;
	Kcompress		= 100.0f;
	damping			= 0.0f;
	restLength		= 0.0f;
	physics1		= NULL;
	id1				= 0;
	p1				= vec3_zero;
	physics2		= NULL;
	id2				= 0;
	p2				= vec3_zero;
}

/*
================
idForce_Spring::~idForce_Spring
================
*/
idForce_Spring::~idForce_Spring( void ) {
}

/*
================
idForce_Spring::InitSpring
================
*/
void idForce_Spring::InitSpring( float Kstretch, float Kcompress, float damping, float restLength ) {
	this->Kstretch = Kstretch;
	this->Kcompress = Kcompress;
	this->damping = damping;
	this->restLength = restLength;
}

/*
================
idForce_Spring::SetPosition
================
*/
void idForce_Spring::SetPosition( idPhysics *physics1, int id1, const idVec3 &p1, idPhysics *physics2, int id2, const idVec3 &p2 ) {
	this->physics1 = physics1;
	this->id1 = id1;
	this->p1 = p1;
	this->physics2 = physics2;
	this->id2 = id2;
	this->p2 = p2;
}

/*
================
idForce_Spring::Evaluate
================
*/
void idForce_Spring::Evaluate( int time ) {
	float length;
	idMat3 axis;
	idVec3 pos1, pos2, velocity1, velocity2, force, dampingForce;
	impactInfo_t info;

	pos1 = p1;
	pos2 = p2;
	velocity1 = velocity2 = vec3_origin;

	if ( physics1 ) {
		axis = physics1->GetAxis( id1 );
		pos1 = physics1->GetOrigin( id1 );
		pos1 += p1 * axis;
		if ( damping > 0.0f ) {
			physics1->GetImpactInfo( id1, pos1, &info );
			velocity1 = info.velocity;
		}
	}

	if ( physics2 ) {
		axis = physics2->GetAxis( id2 );
		pos2 = physics2->GetOrigin( id2 );
		pos2 += p2 * axis;
		if ( damping > 0.0f ) {
			physics2->GetImpactInfo( id2, pos2, &info );
			velocity2 = info.velocity;
		}
	}

	force = pos2 - pos1;
	dampingForce = ( damping * ( ((velocity2 - velocity1) * force) / (force * force) ) ) * force;
	length = force.Normalize();

	// if the spring is stretched
	if ( length > restLength ) {
		if ( Kstretch > 0.0f ) {
			force = ( Square( length - restLength ) * Kstretch ) * force - dampingForce;
			if ( physics1 ) {
				physics1->AddForce( id1, pos1, force );
			}
			if ( physics2 ) {
				physics2->AddForce( id2, pos2, -force );
			}
		}
	}
	else {
		if ( Kcompress > 0.0f ) {
			force = ( Square( length - restLength ) * Kcompress ) * force - dampingForce;
			if ( physics1 ) {
				physics1->AddForce( id1, pos1, -force );
			}
			if ( physics2 ) {
				physics2->AddForce( id2, pos2, force );
			}
		}
	}
}

/*
================
idForce_Spring::RemovePhysics
================
*/
void idForce_Spring::RemovePhysics( const idPhysics *phys ) {
	if ( physics1 == phys ) {
		physics1 = NULL;
	}
	if ( physics2 == phys ) {
		physics2 = NULL;
	}
}
