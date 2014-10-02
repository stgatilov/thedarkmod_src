/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod (http://www.thedarkmod.com/)
 
 $Revision: 5121 $ (Revision of last commit) 
 $Date: 2011-12-11 14:12:26 -0500 (Sun, 11 Dec 2011) $ (Date of last commit)
 $Author: greebo $ (Author of last commit)
 
******************************************************************************/

#ifndef __AI_GUARD_SPOT_TASK_H__
#define __AI_GUARD_SPOT_TASK_H__

#include "Task.h"

namespace ai
{

/**
* greebo: This task requires memory.currentSearchSpot to be valid.
* 
* This task is intended to be pushed into the action Subsystem and
* performs single-handedly how the given hiding spot should be handled.
*
* Note: This Task employs the Movement Subsystem when the algorithm
* judges to walk/run over to the given search spot.
**/

// Define the name of this task
#define TASK_GUARD_SPOT "GuardSpot"

class GuardSpotTask;
typedef boost::shared_ptr<GuardSpotTask> GuardSpotTaskPtr;

class GuardSpotTask :
	public Task
{
	// The spot to guard
	idVec3 _guardSpot;

	// The time this task may exit
	int _exitTime;

	// Whether this task has told the AI to actually move to the spot
	bool _moveInitiated;

	// Whether the guard has arrived at the guard spot
	bool _moveCompleted;

	// The next time the guard should turn
	int _nextTurnTime;

	// Direction you face when you arrive at the spot
	float _baseYaw;

	// Private default constructor
	GuardSpotTask();
public:
	// Get the name of this task
	virtual const idStr& GetName() const;

	// Override the base Init method
	virtual void Init(idAI* owner, Subsystem& subsystem);

	virtual bool Perform(Subsystem& subsystem);

	/** 
	 * greebo: Sets a new goal position for this task.
	 *
	 * @newPos: The new position
	 */
	virtual void SetNewGoal(const idVec3& newPos);

	virtual void OnFinish(idAI* owner); // grayman #2560

	// Save/Restore methods
	virtual void Save(idSaveGame* savefile) const;
	virtual void Restore(idRestoreGame* savefile);

	// Creates a new Instance of this task
	static GuardSpotTaskPtr CreateInstance();
};

} // namespace ai

#endif /* __AI_GUARD_SPOT_TASK_H__ */
