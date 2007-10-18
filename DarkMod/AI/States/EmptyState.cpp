/***************************************************************************
 *
 * PROJECT: The Dark Mod
 * $Revision: 1435 $
 * $Date: 2007-10-16 18:53:28 +0200 (Di, 16 Okt 2007) $
 * $Author: greebo $
 *
 ***************************************************************************/

#include "../idlib/precompiled.h"
#pragma hdrstop

static bool init_version = FileVersionList("$Id: EmptyState.cpp 1435 2007-10-16 16:53:28Z greebo $", init_version);

#include "EmptyState.h"
#include "../Tasks/EmptyTask.h"
#include "../Library.h"

namespace ai
{

// Get the name of this state
const idStr& EmptyState::GetName() const
{
	static idStr _name(STATE_EMPTY);
	return _name;
}

void EmptyState::Init(idAI* owner)
{
	DM_LOG(LC_AI, LT_INFO).LogString("EmptyState initialised.\r");
	assert(owner);

	// Fill the subsystems with Empty Tasks
	owner->GetSubsystem(SubsysMovement)->InstallTask(EmptyTask::CreateInstance());
	owner->GetSubsystem(SubsysCommunication)->InstallTask(EmptyTask::CreateInstance());
	owner->GetSubsystem(SubsysAction)->InstallTask(EmptyTask::CreateInstance());
	owner->GetSubsystem(SubsysSenses)->InstallTask(EmptyTask::CreateInstance());
}

StatePtr EmptyState::CreateInstance()
{
	return StatePtr(new EmptyState);
}

// Register this task with the TaskLibrary
StateLibrary::Registrar emptyStateRegistrar(
	STATE_EMPTY, // Task Name
	StateLibrary::CreateInstanceFunc(&EmptyState::CreateInstance) // Instance creation callback
);

} // namespace ai
