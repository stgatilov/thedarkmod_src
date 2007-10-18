/***************************************************************************
 *
 * PROJECT: The Dark Mod
 * $Revision: 1435 $
 * $Date: 2007-10-16 18:53:28 +0200 (Di, 16 Okt 2007) $
 * $Author: greebo $
 *
 ***************************************************************************/

#ifndef __AI_MEMORY_H__
#define __AI_MEMORY_H__

#include "../idlib/precompiled.h"

namespace ai
{

/**
 * greebo: This defines the ABC of an AI mind. It basically
 *         handles the incoming stimuli and emits signals to the 
 *         AI subsystems like movement, interaction and sensory stuff.
 */
class Memory
{
public:
	// The alert state we're currently in
	EAlertState	alertState;

	// The path entity we're supposed to be heading to
	idEntityPtr<idPathCorner> currentPath;

	// The last time the AI has been barking when patrolling
	int lastPatrolChatTime;

	Memory() :
		alertState(ERelaxed),
		lastPatrolChatTime(-1)
	{}

	// Save/Restore routines
	void Save(idSaveGame* savefile) const
	{
		savefile->WriteInt(static_cast<int>(alertState));
		currentPath.Save(savefile);
		savefile->WriteInt(lastPatrolChatTime);
	}

	void Restore(idRestoreGame* savefile)
	{
		int temp;
		savefile->ReadInt(temp);
		alertState = static_cast<EAlertState>(temp);

		currentPath.Restore(savefile);
		savefile->ReadInt(lastPatrolChatTime);
	}
};

} // namespace ai

#endif /*__AI_MEMORY_H__*/
