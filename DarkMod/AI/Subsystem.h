/***************************************************************************
 *
 * PROJECT: The Dark Mod
 * $Revision: 1435 $
 * $Date: 2007-10-16 18:53:28 +0200 (Di, 16 Okt 2007) $
 * $Author: greebo $
 *
 ***************************************************************************/

#ifndef __AI_SUBSYSTEM_H__
#define __AI_SUBSYSTEM_H__

#include <boost/shared_ptr.hpp>

#include "Tasks/Task.h"

namespace ai
{

enum SubsystemId {
	SubsysMovement = 0,
	SubsysSenses,
	SubsysCommunication,
	SubsysAction,
	SubsystemCount,
};

class Subsystem
{
protected:
	idEntityPtr<idAI> _owner;

	// The currently active task
	TaskPtr _task;

	// One task may be queued. It is installed the next
	// time PerformTask() is called.
	TaskPtr _nextTask;

	// TRUE if this subsystem is performing, default is ON
	bool _enabled;

public:
	Subsystem(idAI* owner);

	// Called regularly by the Mind to run the currently assigned routine.
	virtual void PerformTask();

	// Plugs a new task into this subsystem
	virtual void InstallTask(const TaskPtr& newTask);

	// Puts another task into the queue of this subsystem
	// This can be used by the Tasks to swap themselves with another task
	// without having to fear their immediate destruction.
	virtual void QueueTask(const TaskPtr& nextTask);

	// Sets the current task to the EmptyTask
	virtual void ClearTask();

	// Enables/disables this subsystem
	virtual void Enable();
	virtual void Disable();

	// Returns TRUE if this subsystem is performing.
	virtual bool IsEnabled() const;

	// Save/Restore methods
	virtual void Save(idSaveGame* savefile) const;
	virtual void Restore(idRestoreGame* savefile);
};
typedef boost::shared_ptr<Subsystem> SubsystemPtr;

} // namespace ai

#endif /* __AI_SUBSYSTEM_H__ */
