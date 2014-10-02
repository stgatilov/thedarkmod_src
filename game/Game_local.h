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

#ifndef __GAME_LOCAL_H__
#define	__GAME_LOCAL_H__

#include "Game.h"

#ifdef __linux__
#include "../framework/UsercmdGen.h"
#endif

// enables water physics
#define MOD_WATERPHYSICS

/*
===============================================================================

	Local implementation of the public game interface.

===============================================================================
*/

#define LAGO_IMG_WIDTH 64
#define LAGO_IMG_HEIGHT 64
#define LAGO_WIDTH	64
#define LAGO_HEIGHT	44
#define LAGO_MATERIAL	"textures/sfx/lagometer"
#define LAGO_IMAGE		"textures/sfx/lagometer.tga"

// if set to 1 the server sends the client PVS with snapshots and the client compares against what it sees
#ifndef ASYNC_WRITE_PVS
	#define ASYNC_WRITE_PVS 0
#endif

#ifdef ID_DEBUG_UNINITIALIZED_MEMORY
// This is real evil but allows the code to inspect arbitrary class variables.
#define private		public
#define protected	public
#endif

class idLight;			// J.C.Denton: Required for the declaration of FindMainAmbientLight

class idRenderWorld;
extern idRenderWorld *				gameRenderWorld;

class idSoundWorld;
extern idSoundWorld *				gameSoundWorld;
/**
* place to store the sound world pointer when we temporarily set it to NULL
**/
extern idSoundWorld *			gameSoundWorldBuf;

// the "gameversion" client command will print this plus compile date
#define	GAME_VERSION		"The Dark Mod"

// classes used by idGameLocal
class idEntity;
class idActor;
class idPlayer;
class idCamera;
class idWorldspawn;
class idTestModel;
class idAAS;
class idAI;
class idSmokeParticles;
class idEntityFx;
class idTypeInfo;
class idProgram;
class idThread;
class idEditEntities;
class idLocationEntity;

#define	MAX_CLIENTS				32
// Tels: If you change this value, make sure that LUDICROUS_INDEX 
// in renderer/RenderWorld_local.h is higher than MAX_GENTITIES:
#define	GENTITYNUM_BITS			13
#define	MAX_GENTITIES			(1<<GENTITYNUM_BITS)
#define	ENTITYNUM_NONE			(MAX_GENTITIES-1)
#define	ENTITYNUM_WORLD			(MAX_GENTITIES-2)
#define	ENTITYNUM_MAX_NORMAL	(MAX_GENTITIES-2)

//============================================================================

void gameError( const char *fmt, ... );

#include "gamesys/Event.h"
#include "gamesys/Class.h"
#include "gamesys/SysCvar.h"
#include "gamesys/SysCmds.h"
#include "gamesys/SaveGame.h"
#include "gamesys/DebugGraph.h"

#include "script/Script_Program.h"

#include "anim/Anim.h"

#include "ai/AAS.h"

#include "physics/Clip.h"
#include "physics/Push.h"

#include "Pvs.h"
#include "MultiplayerGame.h"

#include "Objectives/EMissionResult.h"
#include "DifficultyManager.h"

#include "ai/AreaManager.h"
#include "GamePlayTimer.h"
#include "ModelGenerator.h"
#include "ImageMapManager.h"
#include "LightController.h"
#include "ModMenu.h"

#include "SearchManager.h" // grayman debug

#include <boost/shared_ptr.hpp>

#ifdef __linux__
#include "../renderer/RenderWorld.h"
#endif

//============================================================================

class CLightMaterial;
class CsndPropLoader;
class CsndProp;
class CRelations;
typedef boost::shared_ptr<CRelations> CRelationsPtr;
class CMissionData;
typedef boost::shared_ptr<CMissionData> CMissionDataPtr;
class CampaignStats;
typedef boost::shared_ptr<CampaignStats> CampaignStatsPtr;
class CStimResponse;
typedef boost::shared_ptr<CStimResponse> CStimResponsePtr;
class CStim;
typedef boost::shared_ptr<CStim> CStimPtr;
class CStimResponseTimer;
class CGrabber;
class CEscapePointManager;
class CMissionManager;
typedef boost::shared_ptr<CMissionManager> CMissionManagerPtr;
class CHttpConnection;
typedef boost::shared_ptr<CHttpConnection> CHttpConnectionPtr;
class CInventory;
typedef boost::shared_ptr<CInventory> CInventoryPtr;

class CModMenu;
typedef boost::shared_ptr<CModMenu> CModMenuPtr;

class CModelGenerator;
typedef boost::shared_ptr<CModelGenerator> CModelGeneratorPtr;
class ImageMapManager;
typedef boost::shared_ptr<ImageMapManager> ImageMapManagerPtr;
class CLightController;
typedef boost::shared_ptr<CLightController> CLightControllerPtr;

// Forward declare the Conversation System
namespace ai { 
	class ConversationSystem;
	typedef boost::shared_ptr<ConversationSystem> ConversationSystemPtr;
} // namespace

class CDownloadMenu;
typedef boost::shared_ptr<CDownloadMenu> CDownloadMenuPtr;
class CDownloadManager;
typedef boost::shared_ptr<CDownloadManager> CDownloadManagerPtr;

class CShop;
typedef boost::shared_ptr<CShop> CShopPtr;

class CSearchManager; // grayman debug

const int MAX_GAME_MESSAGE_SIZE		= 8192;
const int MAX_ENTITY_STATE_SIZE		= 512;
const int ENTITY_PVS_SIZE			= ((MAX_GENTITIES+31)>>5);
const int NUM_RENDER_PORTAL_BITS	= idMath::BitsForInteger( PS_BLOCK_ALL );

const float SMALL_AI_MASS		= 5.0f; // grayman #3756

typedef struct entityState_s {
	int						entityNumber;
	idBitMsg				state;
	byte					stateBuf[MAX_ENTITY_STATE_SIZE];
	struct entityState_s *	next;
} entityState_t;

typedef struct snapshot_s {
	int						sequence;
	entityState_t *			firstEntityState;
	int						pvs[ENTITY_PVS_SIZE];
	struct snapshot_s *		next;
} snapshot_t;

const int MAX_EVENT_PARAM_SIZE		= 128;

typedef struct entityNetEvent_s {
	int						spawnId;
	int						event;
	int						time;
	int						paramsSize;
	byte					paramsBuf[MAX_EVENT_PARAM_SIZE];
	struct entityNetEvent_s	*next;
	struct entityNetEvent_s *prev;
} entityNetEvent_t;

enum {
	GAME_RELIABLE_MESSAGE_INIT_DECL_REMAP,
	GAME_RELIABLE_MESSAGE_REMAP_DECL,
	GAME_RELIABLE_MESSAGE_SPAWN_PLAYER,
	GAME_RELIABLE_MESSAGE_DELETE_ENT,
	GAME_RELIABLE_MESSAGE_CHAT,
	GAME_RELIABLE_MESSAGE_TCHAT,
	GAME_RELIABLE_MESSAGE_SOUND_EVENT,
	GAME_RELIABLE_MESSAGE_SOUND_INDEX,
	GAME_RELIABLE_MESSAGE_DB,
	GAME_RELIABLE_MESSAGE_KILL,
	GAME_RELIABLE_MESSAGE_DROPWEAPON,
	GAME_RELIABLE_MESSAGE_RESTART,
	GAME_RELIABLE_MESSAGE_SERVERINFO,
	GAME_RELIABLE_MESSAGE_TOURNEYLINE,
	GAME_RELIABLE_MESSAGE_CALLVOTE,
	GAME_RELIABLE_MESSAGE_CASTVOTE,
	GAME_RELIABLE_MESSAGE_STARTVOTE,
	GAME_RELIABLE_MESSAGE_UPDATEVOTE,
	GAME_RELIABLE_MESSAGE_PORTALSTATES,
	GAME_RELIABLE_MESSAGE_PORTAL,
	GAME_RELIABLE_MESSAGE_VCHAT,
	GAME_RELIABLE_MESSAGE_STARTSTATE,
	GAME_RELIABLE_MESSAGE_MENU,
	GAME_RELIABLE_MESSAGE_WARMUPTIME,
	GAME_RELIABLE_MESSAGE_EVENT
};

typedef enum {
	GAMESTATE_UNINITIALIZED,		// prior to Init being called
	GAMESTATE_NOMAP,				// no map loaded
	GAMESTATE_STARTUP,				// inside InitFromNewMap().  spawning map entities.
	GAMESTATE_ACTIVE,				// normal gameplay
	GAMESTATE_COMPLETED,			// greebo: Active during "Mission Complete" (TDM)
	GAMESTATE_SHUTDOWN				// inside MapShutdown().  clearing memory.
} gameState_t;

// Message type for user interaction in the main menu
struct GuiMessage
{
	idStr title;
	idStr message;
	
	enum Type
	{
		MSG_OK,
		MSG_YES_NO,
		MSG_OK_CANCEL,
	};

	Type type;

	idStr positiveCmd;	// which "cmd" to execute on OK / Yes
	idStr negativeCmd;	// which "cmd" to execute on Cancel / No
	idStr okCmd;		// which "cmd" to execute on OK (only for MSG_OK)
};

typedef struct {
	idEntity	*ent;
	int			dist;
} spawnSpot_t;

// grayman #3108 - contributed by 7318
enum {
	PORTALSKY_STANDARD = 0,			// classic portalsky
	PORTALSKY_GLOBAL = 1,			// always following portal sky
	PORTALSKY_LOCAL = 2,			// following portal sky from a spot
};
// end 7318


/**
* Sound prop. flags are used by many classes (Actor, soundprop, entity, etc)
* Therefore they are global.
* See sound prop doc file for definitions of these flags.
**/

typedef struct SSprFlagBits_s
{
	// team alert flags
	unsigned int friendly : 1;
	unsigned int neutral : 1;
	unsigned int enemy : 1;
	unsigned int same : 1;

	// propagation flags
	unsigned int omni_dir : 1; // omnidirectional
	unsigned int unique_loc : 1; // sound comes from a unique location
	unsigned int urgent : 1; // urgent (AI tries to respond ASAP)
	unsigned int global_vol : 1; // sound has same volume over whole map
	unsigned int check_touched : 1; // for non-AI, check who last touched the entity
} SSprFlagBits;

typedef union USprFlags_s
{
	unsigned int m_field;
	SSprFlagBits m_bits;
} USprFlags;

/**
* Sound propagation parameters: needed for function arguments
**/

struct SSprParms
{
	USprFlags flags;

	idStr		name; // sound name
	float		propVol; // propagated volume

	// Apparent direction of the sound, determined by the path point on the portal
	idVec3		direction; 
	// actual origin of the sound, used for some localization simulation
	idVec3		origin; 
	float		duration; // duration
	int			frequency; // int representing the octave of the sound
	float		bandwidth; // sound bandwidth

	float		loudness; // this is set by AI hearing response
	float		alertFactor;	// angua: alert increase is scaled by this factor
	float		alertMax;	// angua: alert increase can not become higher than this

	bool		bSameArea; // true if the sound came from same portal area
	bool		bDetailedPath; // true if detailed path minimization was used to obtain the sound path
	int			floods; // number of portals the sound travelled thru before it hit the AI

	idEntity*	maker;		// it turns out the AI needs to know who made the sound to avoid bugs in some cases
	idAI*		makerAI;	// a shorthand of the above. If this is non NULL the <maker> entity is an AI.
	int			messageTag;	// grayman #3355 - a unique number that's shared with a message associated with this sound
};

//============================================================================

class idEventQueue {
public:
	typedef enum {
		OUTOFORDER_IGNORE,
		OUTOFORDER_DROP,
		OUTOFORDER_SORT
	} outOfOrderBehaviour_t;

							idEventQueue() : start( NULL ), end( NULL ) {}

	entityNetEvent_t *		Alloc();
	void					Free( entityNetEvent_t *event );
	void					Shutdown();

	void					Init();
	void					Enqueue( entityNetEvent_t* event, outOfOrderBehaviour_t oooBehaviour );
	entityNetEvent_t *		Dequeue( void );
	entityNetEvent_t *		RemoveLast( void );

	entityNetEvent_t *		Start( void ) { return start; }

private:
	entityNetEvent_t *					start;
	entityNetEvent_t *					end;
	idBlockAlloc<entityNetEvent_t,32>	eventAllocator;
};

//============================================================================

template< class type >
class idEntityPtr {
public:
							idEntityPtr();

	// save games
	void					Save( idSaveGame *savefile ) const;					// archives object for save game file
	void					Restore( idRestoreGame *savefile );					// unarchives object from save game file

	idEntityPtr<type> &		operator=( type *ent );
	bool					operator==(const idEntityPtr<type>& other) const;

	// synchronize entity pointers over the network
	int						GetSpawnId( void ) const { return spawnId; }
	bool					SetSpawnId( int id );
	bool					UpdateSpawnId( void );

	bool					IsValid( void ) const;
	type *					GetEntity( void ) const;
	int						GetEntityNum( void ) const;

private:
	int						spawnId;
};

// Note: Because lightgem.h uses idEntityPr, the file should be included here or 
// idEntityPtr Definition should be moved to lightgem.h. - J.C.Denton

#include "LightGem.h"
//============================================================================

// grayman #3424 - These are the events that are considered suspicious, in that they raise
// the alert levels of AI. Non-specific events that simply add to intruder evidence don't
// need to be kept this way.

enum EventType
{
	E_EventTypeEnemy = 0,	// enemy is seen ("snd_warnSawEnemy")
							// enemy tried to KO me ("snd_warnSawEnemy")
	E_EventTypeDeadPerson,	// found a corpse ("snd_warnFoundCorpse")
	E_EventTypeMissingItem	// noticed something was stolen ("snd_warnMissingItem")
};

// Hold information about a suspicious event (corpse, unconscious person, missing item, etc.)
struct SuspiciousEvent
{
	EventType type;					// type of event
	idVec3 location;				// location
	idEntityPtr<idEntity> entity;	// entity, if relevant
};

class idDeclEntityDef;

class idGameLocal : public idGame {
public:
	idDict					serverInfo;				// all the tunable parameters, like numclients, etc
	int						numClients;				// pulled from serverInfo and verified
	idDict					userInfo[MAX_CLIENTS];	// client specific settings
	usercmd_t				usercmds[MAX_CLIENTS];	// client input commands
	idDict					persistentPlayerInfo[MAX_CLIENTS];
	idEntity *				entities[MAX_GENTITIES];// index to entities
	int						spawnIds[MAX_GENTITIES];// for use in idEntityPtr

	// greebo: For use in Stim/Response system (gets invalidated each frame)
	idEntity*				srEntities[MAX_GENTITIES]; 

	int						firstFreeIndex;			// first free index in the entities array
	int						num_entities;			// current number <= MAX_GENTITIES
	idHashIndex				entityHash;				// hash table to quickly find entities by name
	idWorldspawn *			world;					// world entity
	idLinkList<idEntity>	spawnedEntities;		// all spawned entities
	idLinkList<idEntity>	activeEntities;			// all thinking entities (idEntity::thinkFlags != 0)
	idLinkList<idAI>		spawnedAI;				// greebo: all spawned AI
	int						numEntitiesToDeactivate;// number of entities that became inactive in current frame
	bool					sortPushers;			// true if active lists needs to be reordered to place pushers at the front
	bool					sortTeamMasters;		// true if active lists needs to be reordered to place physics team masters before their slaves
	idDict					persistentLevelInfo;	// contains args that are kept around between levels

	// The inventory class which keeps items safe between maps
	CInventoryPtr			persistentPlayerInventory;

	// The list of campaign info entities in this map
	idList<idEntity*>		campaignInfoEntities;

	// greebo: Is set to TRUE if the post-mission screen (debriefing or success screen) is currently active. 
	// (Usually these state variables should be kept in the GUI, but in this case I need it to be accessible 
	// when the player loads a new map via the console.)
	bool					postMissionScreenActive;

	// Toggle to keep track whether the GUI state variables have been set up
	bool					briefingVideoInfoLoaded;

	// Hold information about a single video piece
	struct BriefingVideoPart
	{
		idStr	material;	// name of the material
		int		lengthMsec; // length in msecs
	};

	// The list of briefing videos for the current mission
	idList<BriefingVideoPart>	briefingVideo;
	
	// Index into the above list
	int							curBriefingVideoPart;

	// The list of DE-briefing videos for the current mission
	idList<BriefingVideoPart>	debriefingVideo;

	// Index into the above list
	int							curDebriefingVideoPart;

	bool					mainMenuExited;			// Solarsplace 19th Nov 2010 - Bug tracker id 0002424

	// can be used to automatically effect every material in the world that references globalParms
	float					globalShaderParms[ MAX_GLOBAL_SHADER_PARMS ];	

	idRandom				random;					// random number generator used throughout the game

	idProgram				program;				// currently loaded script and data space
	idThread *				frameCommandThread;

	idClip					clip;					// collision detection
	idPush					push;					// geometric pushing
	idPVS					pvs;					// potential visible set

	idTestModel *			testmodel;				// for development testing of models
	idEntityFx *			testFx;					// for development testing of fx

	idStr					sessionCommand;			// a target_sessionCommand can set this to return something to the session 

	idMultiplayerGame		mpGame;					// handles rules for standard dm

	idSmokeParticles *		smokeParticles;			// global smoke trails
	idEditEntities *		editEntities;			// in game editing

	// Darkmod's grabber to help with object manipulation
	CGrabber*				m_Grabber;

	// The object handling the difficulty settings
	difficulty::DifficultyManager	m_DifficultyManager;

	// The manager for handling AI => Area mappings (needed for AI to remember locked doors, for instance)
	ai::AreaManager			m_AreaManager;

	// The manager class for all map conversations
	ai::ConversationSystemPtr	m_ConversationSystem;

	/**
	 * greebo: The fan-mission-handling class. Also contains GUI handling code.
	 */
	CModMenuPtr				m_ModMenu;

	// The download menu handler
	CDownloadMenuPtr		m_DownloadMenu;

	// The download manager
	CDownloadManagerPtr		m_DownloadManager;

	/**
	 * greebo: The mission manager instance, for manipulating mission PK4s and saving play info.
	 */
	CMissionManagerPtr		m_MissionManager;

	/**
	 * tels: The model generator instance, for manipulating/generating models on the fly.
	 */
	CModelGeneratorPtr		m_ModelGenerator;

	/**
	 * tels: The image mapmanager instance, for loading/sharing image maps for the SEED system.
	 */
	ImageMapManagerPtr		m_ImageMapManager;

	/**
	 * tels: The light controller instance, used to control local ambient lights.
	 */
	CLightControllerPtr		m_LightController;

	/**
	 * greebo: The class handling the main menu's shop GUI.
	 */
	CShopPtr				m_Shop;

	/**
	 * This list is some sort of queue of error messages collected by 
	 * gameLocal.Error(). The messages here will be sent 
	 * to the main menu GUI once it is displayed again after the error.
	 */
	mutable idStr			m_guiError;

	mutable idList<GuiMessage>	m_GuiMessages;

	/**
	* Pointer to global AI Relations object
	**/
	CRelationsPtr			m_RelationsManager;

	/**
	* Pointer to global Mission Data object (objectives & stats)
	**/
	CMissionDataPtr			m_MissionData;
	EMissionResult			m_MissionResult; // holds the global mission state

	// Campaign statistics
	CampaignStatsPtr		m_CampaignStats;

	/**
	* Pointer to global sound prop loader object
	**/
	CsndPropLoader *		m_sndPropLoader;

	/**
	* Pointer to global sound prop gameplay object
	**/
	CsndProp *				m_sndProp;

	/**
	 * greebo: The escape point manager which is keeping track
	 *         of all the tdmPathFlee entities.
	 */
	CEscapePointManager*	m_EscapePointManager;

	// grayman debug - Search Manager
	CSearchManager*			m_searchManager;

	void					GetPortals(Search* search, idAI* ai); // grayman debug

	/**
	 * greebo: This timer keeps track of the actual gameplay time.
	 */
	GamePlayTimer m_GamePlayTimer;

	// The singleton connection object
	CHttpConnectionPtr		m_HttpConnection;
	
	/**
	* Temporary storage of the walkspeed.  This is a workaround
	*	because the walkspeed keeps getting reset.
	**/
	float					m_walkSpeed;

	// The highest used unique stim/response id
	int						m_HighestSRId;

	idList<CStimResponseTimer *> m_Timer;			// generic timer used for other purposes than stims.
	idList<CStim *>			m_StimTimer;			// All stims that have a timer associated. 
	idList< idEntityPtr<idEntity> >		m_StimEntity;			// all entities that currently have a stim regardless of it's state
	idList< idEntityPtr<idEntity> >		m_RespEntity;			// all entities that currently have a response regardless of it's state

	int						cinematicSkipTime;		// don't allow skipping cinemetics until this time has passed so player doesn't skip out accidently from a firefight
	int						cinematicStopTime;		// cinematics have several camera changes, so keep track of when we stop them so that we don't reset cinematicSkipTime unnecessarily
	int						cinematicMaxSkipTime;	// time to end cinematic when skipping.  there's a possibility of an infinite loop if the map isn't set up right.
	bool					inCinematic;			// game is playing cinematic (player controls frozen)
	bool					skipCinematic;

													// are kept up to date with changes to serverInfo
	int						framenum;
	int						previousTime;			// time in msec of last frame
	int						time;					// in msec
	int						m_Interleave;			// How often should the lightgem calculation be skipped?
	static const int		msec = USERCMD_MSEC;	// time since last update in milliseconds

	int						vacuumAreaNum;			// -1 if level doesn't have any outside areas

	gameType_t				gameType;
	bool					isMultiplayer;			// set if the game is run in multiplayer mode
	bool					isServer;				// set if the game is run for a dedicated or listen server
	bool					isClient;				// set if the game is run for a client
													// discriminates between the RunFrame path and the ClientPrediction path
													// NOTE: on a listen server, isClient is false
	int						localClientNum;			// number of the local client. MP: -1 on a dedicated
	idLinkList<idEntity>	snapshotEntities;		// entities from the last snapshot
	int						realClientTime;			// real client time
	bool					isNewFrame;				// true if this is a new game frame, not a rerun due to prediction
	float					clientSmoothing;		// smoothing of other clients in the view
	int						entityDefBits;			// bits required to store an entity def number

	static const char *		surfaceTypeNames[ MAX_SURFACE_TYPES ];	// text names for surface types
	/**
	* DarkMod: text names for new surface types
	**/
	static const char *		m_NewSurfaceTypes[ MAX_SURFACE_TYPES*2 + 1];

	idEntityPtr<idEntity>	lastGUIEnt;				// last entity with a GUI, used by Cmd_NextGUI_f
	int						lastGUI;				// last GUI on the lastGUIEnt

	idList<int>				currentLights;			// sikk - Soft Shadows PostProcess
	bool					ambientOn;

	idEntityPtr<idEntity>	portalSkyEnt;
	bool					portalSkyActive;

	void					SetPortalSkyEnt( idEntity *ent );
	bool					IsPortalSkyActive();

	// grayman #3108 - contributed by 7318
	bool					globalPortalSky;	
	int						portalSkyScale;		
	int						currentPortalSkyType; // 0 = classic, 1 = global, 2 = local 
	idVec3					portalSkyOrigin;	
	idVec3					portalSkyGlobalOrigin;	
	idVec3					playerOldEyePos;	
	bool					CheckGlobalPortalSky();	
	void					SetGlobalPortalSky(const char *name);
	void					SetCurrentPortalSkyType(int type); // 0 = classic, 1 = global, 2 = local
	int						GetCurrentPortalSkyType(); // 0 = classic, 1 = global, 2 = local
	// end 7318

	// grayman #3424 - The list of suspicious events
	idList<SuspiciousEvent> m_suspiciousEvents;

	// grayman #3584 - The list of ambient lights
    idList< idEntityPtr<idLight> > m_ambientLights;

	// tels: a list of all speaker entities with s_music set, these are affected by s_vol_music:
	idList<int>				musicSpeakers;

	// A flag set by the player to fire a "final save" which must occur at the end of this frame
	bool					m_TriggerFinalSave;

	// grayman #2933 - store the start position selected during the mission briefing, if any
	const char *			m_StartPosition;

	// grayman #3763 - true when the Mission Start gui can be displayed, or the mission started,
	// based on when the Loading Bar reaches 100%
	bool					m_time2Start;


	// ---------------------- Public idGame Interface -------------------

							idGameLocal();
							~idGameLocal();

	virtual void			Init( void );
	virtual void			Shutdown( void );
	virtual void			SetLocalClient( int clientNum );
	virtual void			ThrottleUserInfo( void );
	virtual const idDict *	SetUserInfo( int clientNum, const idDict &userInfo, bool isClient, bool canModify );
	virtual const idDict *	GetUserInfo( int clientNum );
	virtual void			SetServerInfo( const idDict &serverInfo );

	virtual const idDict &	GetPersistentPlayerInfo( int clientNum );
	virtual void			SetPersistentPlayerInfo( int clientNum, const idDict &playerInfo );
	virtual void			SetTime2Start(); // grayman #3763
	// Obsttorte
	virtual idStr			triggeredSave();
	virtual void			incrementSaveCount();
	virtual bool			savegamesDisallowed();
	virtual bool			quicksavesDisallowed();
	// <--- end
	virtual void			InitFromNewMap( const char *mapName, idRenderWorld *renderWorld, idSoundWorld *soundWorld, bool isServer, bool isClient, int randSeed );
	virtual bool			InitFromSaveGame( const char *mapName, idRenderWorld *renderWorld, idSoundWorld *soundWorld, idFile *saveGameFile );
	virtual void			SaveGame( idFile *saveGameFile );
	virtual void			MapShutdown( void );
	virtual void			CacheDictionaryMedia( const idDict *dict );
	virtual void			SpawnPlayer( int clientNum );
	virtual gameReturn_t	RunFrame( const usercmd_t *clientCmds );
	virtual int				GetSpyglassOverlay(); // grayman #3807
	virtual int				DetermineSpyglassOverlay(); // grayman #3807

	/**
	* TDM: Pause/Unpause game
	**/
	virtual void			PauseGame( bool bPauseState );
	virtual bool			Draw( int clientNum );
	virtual escReply_t		HandleESC( idUserInterface **gui );
	virtual idUserInterface	*StartMenu( void );
	virtual const char *	HandleGuiCommands( const char *menuCommand );
	virtual void			HandleMainMenuCommands( const char *menuCommand, idUserInterface *gui );
	/**
	* Adjusts the size of GUI variables to support stretching/scaling of the GUI.
    */
	virtual void			UpdateGUIScaling( idUserInterface *gui );
	virtual allowReply_t	ServerAllowClient( int numClients, const char *IP, const char *guid, const char *password, char reason[MAX_STRING_CHARS] );
	virtual void			ServerClientConnect( int clientNum, const char *guid );
	virtual void			ServerClientBegin( int clientNum );
	virtual void			ServerClientDisconnect( int clientNum );
	virtual void			ServerWriteInitialReliableMessages( int clientNum );
	virtual void			ServerWriteSnapshot( int clientNum, int sequence, idBitMsg &msg, byte *clientInPVS, int numPVSClients );
	virtual bool			ServerApplySnapshot( int clientNum, int sequence );
	virtual void			ServerProcessReliableMessage( int clientNum, const idBitMsg &msg );
	virtual void			ClientReadSnapshot( int clientNum, int sequence, const int gameFrame, const int gameTime, const int dupeUsercmds, const int aheadOfServer, const idBitMsg &msg );
	virtual bool			ClientApplySnapshot( int clientNum, int sequence );
	virtual void			ClientProcessReliableMessage( int clientNum, const idBitMsg &msg );
	virtual gameReturn_t	ClientPrediction( int clientNum, const usercmd_t *clientCmds, bool lastPredictFrame );

	virtual void			GetClientStats( int clientNum, char *data, const int len );
	virtual void			SwitchTeam( int clientNum, int team );

	virtual bool			DownloadRequest( const char *IP, const char *guid, const char *paks, char urls[ MAX_STRING_CHARS ] );


	// ---------------------- Public idGameLocal Interface -------------------

	void					Printf( const char *fmt, ... ) const id_attribute((format(printf,2,3)));
	void					DPrintf( const char *fmt, ... ) const id_attribute((format(printf,2,3)));
	void					Warning( const char *fmt, ... ) const id_attribute((format(printf,2,3)));
	void					DWarning( const char *fmt, ... ) const id_attribute((format(printf,2,3)));
	void					Error( const char *fmt, ... ) const id_attribute((format(printf,2,3)));

							// Initializes all map variables common to both save games and spawned games
	void					LoadMap( const char *mapName, int randseed );

	void					LocalMapRestart( void );
	void					MapRestart( void );
	static void				MapRestart_f( const idCmdArgs &args );
	bool					NextMap( void );	// returns whether serverinfo settings have been modified
	static void				NextMap_f( const idCmdArgs &args );

	idMapFile *				GetLevelMap( void );
	const char *			GetMapName( void ) const;

	int						NumAAS( void ) const;
	int						GetAASId( idAAS* aas ) const; // greebo: Returns the ID for the given pointer (-1 for invalid pointers)
	idAAS *					GetAAS( int num ) const;
	idAAS *					GetAAS( const char *name ) const;
	void					SetAASAreaState( const idBounds &bounds, const int areaContents, bool closed );
	aasHandle_t				AddAASObstacle( const idBounds &bounds );
	void					RemoveAASObstacle( const aasHandle_t handle );
	void					RemoveAllAASObstacles( void );

	// greebo: Initialises the EAS (routing system for elevators)
	void					SetupEAS();

	bool					CheatsOk( bool requirePlayer = true );
	gameState_t				GameState( void ) const;

	/**
	 * greebo: Prepares the running map for mission end. Does nothing if the current gamestate
	 *         is lower than GAMESTATE_ACTIVE. Removes all entities of certain types from the
	 *         world and sets all moveables to rest.
	 */
	void					PrepareForMissionEnd();

	void					 SetMissionResult(EMissionResult result);
	ID_INLINE EMissionResult GetMissionResult() const;

	idEntity *				SpawnEntityType( const idTypeInfo &classdef, const idDict *args = NULL, bool bIsClientReadSnapshot = false );
	bool					SpawnEntityDef( const idDict &args, idEntity **ent = NULL, bool setDefaults = true );
	int						GetSpawnId( const idEntity *ent ) const;

	const idDeclEntityDef *	FindEntityDef( const char *name, bool makeDefault = true ) const;
	const idDict *			FindEntityDefDict( const char *name, bool makeDefault = true ) const;

	void					RegisterEntity( idEntity *ent );
	void					UnregisterEntity( idEntity *ent );

	bool					RequirementMet( idEntity *activator, const idStr &requires, int removeItem );

	bool					InPlayerPVS( idEntity *ent ) const;
	bool					InPlayerConnectedArea( idEntity *ent ) const;

	pvsHandle_t				GetPlayerPVS()			{ return playerPVS; };

	void					SetCamera( idCamera *cam );
	idCamera *				GetCamera( void ) const;
	bool					SkipCinematic( void );
	void					CalcFov( float base_fov, float &fov_x, float &fov_y ) const;

	void					AddEntityToHash( const char *name, idEntity *ent );
	bool					RemoveEntityFromHash( const char *name, idEntity *ent );
	int						GetTargets( const idDict &args, idList< idEntityPtr<idEntity> >& list, const char *ref ) const;
	int						GetRelights(const idDict &args, idList<idEntityPtr<idEntity> >& list, const char *ref) const; // grayman #2603
	float					GetAmbientIllumination( const idVec3 point ) const; // grayman #3132

							// returns the master entity of a trace.  for example, if the trace entity is the player's head, it will return the player.
	idEntity *				GetTraceEntity( const trace_t &trace ) const;

	static void				ArgCompletion_EntityName( const idCmdArgs &args, void(*callback)( const char *s ) );
	idEntity *				FindTraceEntity( idVec3 start, idVec3 end, const idTypeInfo &c, const idEntity *skip ) const;
	idEntity *				FindEntity( const char *name ) const;
	idEntity *				FindEntityUsingDef( idEntity *from, const char *match ) const;
	int						EntitiesWithinRadius( const idVec3 org, float radius, idEntity **entityList, int maxCount ) const;

	/**
	* Get the entity that the player is looking at
	* Currently called by console commands, does a long trace so should not be
	* called every frame.
	**/
	idEntity *				PlayerTraceEntity( void );

	void					KillBox( idEntity *ent, bool catch_teleport = false );
	void					RadiusDamage( const idVec3 &origin, idEntity *inflictor, idEntity *attacker, idEntity *ignoreDamage, idEntity *ignorePush, const char *damageDefName, float dmgPower = 1.0f );
	void					RadiusPush( const idVec3 &origin, const float radius, const float push, const idEntity *inflictor, const idEntity *ignore, float inflictorScale, const bool quake );
	void					RadiusPushClipModel( const idVec3 &origin, const float push, const idClipModel *clipModel );

	void					ProjectDecal( const idVec3 &origin, const idVec3 &dir, float depth, bool parallel, float size, const char *material,
										  float angle = 0, idEntity* target = NULL, bool save = false, int starttime = -1 ); // target, save, startime added #3817 -- SteveL
	void					BloodSplat( const idVec3 &origin, const idVec3 &dir, float size, const char *material );

	void					CallFrameCommand( idEntity *ent, const function_t *frameCommand );
	void					CallObjectFrameCommand( idEntity *ent, const char *frameCommand );

	const idVec3 &			GetGravity( void ) const;

	// added the following to assist licensees with merge issues
	int						GetFrameNum() const { return framenum; };
	int						GetTime() const { return time; };
	int						GetMSec() const { return msec; };

	int						GetNextClientNum( int current ) const;
	idPlayer *				GetClientByNum( int current ) const;
	idPlayer *				GetClientByName( const char *name ) const;
	idPlayer *				GetClientByCmdArgs( const idCmdArgs &args ) const;

	idPlayer *				GetLocalPlayer() const;

	void					SpreadLocations();
	idLocationEntity *		LocationForPoint( const idVec3 &point );	// May return NULL
	/**
	* LocationForArea returns a pointer to the location entity for the given area number
	* Returns NULL if the area number is out of bounds, or if locations haven't sprad yet
	**/
	idLocationEntity *		LocationForArea( const int areaNum ); 

	idEntity *				SelectInitialSpawnPoint( idPlayer *player );

	void					SetPortalState( qhandle_t portal, int blockingBits );
	void					SaveEntityNetworkEvent( const idEntity *ent, int event, const idBitMsg *msg );
	void					ServerSendChatMessage( int to, const char *name, const char *text );
	int						ServerRemapDecl( int clientNum, declType_t type, int index );
	int						ClientRemapDecl( declType_t type, int index );

	void					SetGlobalMaterial( const idMaterial *mat );
	const idMaterial *		GetGlobalMaterial();

	void					SetGibTime( int _time ) { nextGibTime = _time; };
	int						GetGibTime() { return nextGibTime; };

	bool					NeedRestart();

	/**
	 * LoadLightMaterial loads the falloff textures from the light materials. The appropriate
	 * textures are only loaded when the light is spawned and requests the texture.
	 */
	void					LoadLightMaterial(const char *Filename, idList<CLightMaterial *> *);

	/**
	 * CalcLightgem will do the rendersnapshot and analyze the snaphost image in order
	 * to determine the lightvalue for the lightgem.
	 */
	float					CalcLightgem(idPlayer*);

	bool					AddStim(idEntity *);
	void					RemoveStim(idEntity *);
	bool					AddResponse(idEntity *);
	void					RemoveResponse(idEntity *);
	bool					DoesOpeningExist( const idVec3 origin, const idVec3 target, const float radius, const idVec3 normal, idEntity* ent ); // grayman #1104

	
	/************************************************************************/
	/* J.C.Denton:	Finds the main ambient light and creates a new one if	*/
	/*				the main ambient is not found							*/
	/************************************************************************/
	idLight *				FindMainAmbientLight( bool a_bCreateNewIfNotFound = false );  

	/** 
	 * greebo: Links the given entity into the list of Stim entities, such that
	 * it gets considered each frame as potential stim emitter.
	 */
	void					LinkStimEntity(idEntity* ent);

	/**
	 * greebo: Removes the given entity from the global list of stimming entities.
	 * Although it might have active stims, it is no longer considered each frame.
	 */
	void					UnlinkStimEntity(idEntity* ent);

	/**
	 * Checks whether the entity <e> is in the given list named <list>. 
	 * @returns: the list index or -1 if the entity is not in the list.
	 */
	int						CheckStimResponse(idList< idEntityPtr<idEntity> > &list, idEntity *);

	/**
	* Fires off all the enabled responses to this stim of the entities in the given entites list.
	* If the trigger is coming from a timer, <Timer> is set to true.
	* 
	* @return The number of responses triggered
	*
	*/
	int						DoResponseAction(const CStimPtr& stim, int numEntities, idEntity* originator, const idVec3& stimOrigin);

	/**
	 * Trace a LOS path from gas origin to a point.
	 */
	int						TraceGasPath( idVec3 from, idVec3 to, idEntity* ignore, idVec3& normal ); // grayman #1104


	/**
	 * Process the timer ticks for all timers that are used for other purposes than stim/responses.
	 */
	void					ProcessTimer(unsigned long ticks);

	/**
	 * ProcessStimResponse will check whether stims are in reach of a response and if so activate them.
	 */
	void					ProcessStimResponse(unsigned long ticks);

	/**
	 * greebo: Traverses the entities and tries to find the Stim/Response with the given ID.
	 * This is expensive, so don't call this during map runtime, only in between maps.
	 *
	 * @returns: the pointer to the class, or NULL if the uniqueId couldn't be found.
	 */
	CStimResponsePtr		FindStimResponse(int uniqueId);

	// Checks the TDM version
	void					CheckTDMVersion();

	void					AddMainMenuMessage(const GuiMessage& message);
	void					HandleGuiMessages(idUserInterface* ui);

	// Tels: Return mapFileName as it is private
	const idStr&			GetMapFileName() const;

	/**
	 * greebo: Register a trigger that is to be fired when the mission <missionNum> is loaded.
	 * The activatorName is stored along with the name of the target to be triggered. When the target map
	 * has been loaded and all entities have been spawned the game will try to resolve these names
	 * and issue the activation event.
	 * If the activator's name is empty, the local player will be used as activator.
	 */
	void					AddInterMissionTrigger(int missionNum, const idStr& activatorName, const idStr& targetName);

	// For internal use, is public to be callable by the event system
	void					ProcessInterMissionTriggers();

	// Remove any persistent inventory items, clear inter-mission triggers, etc.
	void					ClearPersistentInfo();

	// Events invoked by the engine on reloadImages or vid_restart
	void					OnReloadImages();
	void					OnVidRestart();

	void					AllowImmediateStim( idEntity* e, int stimType ); // grayman #3317

	int						GetNextMessageTag(); // grayman #3355

	int						FindSuspiciousEvent( EventType type, idVec3 location, idEntity* entity ); // grayman #3424
	int						LogSuspiciousEvent( SuspiciousEvent se ); // grayman #3424  
	
private:
	const static int		INITIAL_SPAWN_COUNT = 1;

	idStr					m_strMainAmbientLightName;	// The name of main ambient light, default is: "ambient_world". - J.C.Denton
	idStr					mapFileName;			// name of the map, empty string if no map loaded
	idMapFile *				mapFile;				// will be NULL during the game unless in-game editing is used
	bool					mapCycleLoaded;

	int						spawnCount;
	int						mapSpawnCount;			// it's handy to know which entities are part of the map

	idLocationEntity **		locationEntities;		// for location names, etc

	idCamera *				camera;
	const idMaterial *		globalMaterial;			// for overriding everything

	idList<idAAS *>			aasList;				// area system
	idStrList				aasNames;

	idDict					spawnArgs;				// spawn args used during entity spawning  FIXME: shouldn't be necessary anymore

	pvsHandle_t				playerPVS;				// merged pvs of all players
	pvsHandle_t				playerConnectedAreas;	// all areas connected to any player area

	idVec3					gravity;				// global gravity vector
	gameState_t				gamestate;				// keeps track of whether we're spawning, shutting down, or normal gameplay
	bool					influenceActive;		// true when a phantasm is happening
	int						nextGibTime;

	idList<int>				clientDeclRemap[MAX_CLIENTS][DECL_MAX_TYPES];

	entityState_t *			clientEntityStates[MAX_CLIENTS][MAX_GENTITIES];
	int						clientPVS[MAX_CLIENTS][ENTITY_PVS_SIZE];
	snapshot_t *			clientSnapshots[MAX_CLIENTS];
	idBlockAlloc<entityState_t,256>entityStateAllocator;
	idBlockAlloc<snapshot_t,64>snapshotAllocator;

	idEventQueue			eventQueue;
	idEventQueue			savedEventQueue;

	idStaticList<spawnSpot_t, MAX_GENTITIES> spawnSpots;
	idStaticList<idEntity *, MAX_GENTITIES> initialSpots;
	int						currentInitialSpot;

	idDict					newInfo;

	idStrList				shakeSounds;

	byte					lagometer[ LAGO_IMG_HEIGHT ][ LAGO_IMG_WIDTH ][ 4 ];

	bool					m_DoLightgem;		// Signal when the lightgem may be processed.
	LightGem				m_lightGem;

	int						m_uniqueMessageTag;	// grayman #3355 - unique number for tying AI barks and messages together 

	int						m_spyglassOverlay; // grayman #3807 - no need to save/restore

	// A container for keeping the inter-mission trigger information
	struct InterMissionTrigger
	{
		// The number of the mission this trigger applies to
		int missionNum;

		// The name of the entity that should be used as activator. Is resolved immediately after spawn time.
		// If empty, the player will be used.
		idStr	activatorName;

		// The name of the target entity to be triggered. Is resolved immediately after spawn time.
		idStr	targetName;
	};

	idList<InterMissionTrigger>	m_InterMissionTriggers;

	// Tels: For each part in a GUI command in 'set "cmd" "command arg1 arg2;" the game will
	//		 call HandleMainMenuCommand(), sometimes with a final call with the ";".
	//		 This list here keeps all these parts so we can execute the command
	//		 and have all the arguments, too.
	idList<idStr>				m_GUICommandStack;
	// how many arguments do we expect for the current command (m_GUICommandStack[0]):
	int							m_GUICommandArgs;

	void					Clear( void );
							// returns true if the entity shouldn't be spawned at all in this game type or difficulty level
	bool					InhibitEntitySpawn( idDict &spawnArgs );

							// spawn entities from the map file
	void					SpawnMapEntities( void );
							// commons used by init, shutdown, and restart
	void					MapPopulate( void );
	void					MapClear( bool clearClients );

	pvsHandle_t				GetClientPVS( idPlayer *player, pvsType_t type );
	void					SetupPlayerPVS( void );
	void					FreePlayerPVS( void );
	void					UpdateGravity( void );
	void					SortActiveEntityList( void );
	void					ShowTargets( void );
	void					RunDebugInfo( void );

	void					InitScriptForMap( void );

	void					InitConsoleCommands( void );
	void					ShutdownConsoleCommands( void );

	void					InitAsyncNetwork( void );
	void					ShutdownAsyncNetwork( void );
	void					InitLocalClient( int clientNum );
	void					InitClientDeclRemap( int clientNum );
	void					ServerSendDeclRemapToClient( int clientNum, declType_t type, int index );
	void					FreeSnapshotsOlderThanSequence( int clientNum, int sequence );
	bool					ApplySnapshot( int clientNum, int sequence );
	void					WriteGameStateToSnapshot( idBitMsgDelta &msg ) const;
	void					ReadGameStateFromSnapshot( const idBitMsgDelta &msg );
	void					NetworkEventWarning( const entityNetEvent_t *event, const char *fmt, ... ) id_attribute((format(printf,3,4)));
	void					ServerProcessEntityNetworkEventQueue( void );
	void					ClientProcessEntityNetworkEventQueue( void );
	void					ClientShowSnapshot( int clientNum ) const;
							// call after any change to serverInfo. Will update various quick-access flags
	void					UpdateServerInfoFlags( void );
	void					RandomizeInitialSpawns( void );
	static int				sortSpawnPoints( const void *ptr1, const void *ptr2 );

	void					DumpOggSounds( void );
	void					GetShakeSounds( const idDict *dict );
	void					SelectTimeGroup( int timeGroup );
	int						GetTimeGroupTime( int timeGroup );
	void					GetBestGameType( const char* map, const char* gametype, char buf[ MAX_STRING_CHARS ] );

	void					Tokenize( idStrList &out, const char *in );

	void					UpdateLagometer( int aheadOfServer, int dupeUsercmds );

	void					GetMapLoadingGUI( char gui[ MAX_STRING_CHARS ] );

	// Sets the video CVARs according to the settings in the given GUI
	void					UpdateScreenResolutionFromGUI(idUserInterface* gui);

	// Splits the given string and stores the found video materials in the target list.
	// Calculates the length of the ROQ videos as defined in the string (each material
	// is separated by a semicolon) Returns the total length in milliseconds, or -1 on failure.
	// The lengthStr corresponds to the videosStr, but contains the lengths of the clips
	static int				LoadVideosFromString(const char* videosStr, const char* lengthStr, 
												 idList<BriefingVideoPart>& targetList);
};

//============================================================================

extern idGameLocal			gameLocal;
extern idAnimManager		animationLib;

//============================================================================

template< class type >
ID_INLINE idEntityPtr<type>::idEntityPtr() {
	spawnId = 0;
}

template< class type >
ID_INLINE void idEntityPtr<type>::Save( idSaveGame *savefile ) const {
	savefile->WriteInt( spawnId );
}

template< class type >
ID_INLINE void idEntityPtr<type>::Restore( idRestoreGame *savefile ) {
	savefile->ReadInt( spawnId );
}

template< class type >
ID_INLINE idEntityPtr<type> &idEntityPtr<type>::operator=( type *ent ) {
	if ( ent == NULL ) {
		spawnId = 0;
	} else {
		spawnId = ( gameLocal.spawnIds[ent->entityNumber] << GENTITYNUM_BITS ) | ent->entityNumber;
	}
	return *this;
}

template< class type >
ID_INLINE bool idEntityPtr<type>::operator==(const idEntityPtr<type>& other) const
{
	return spawnId == other.spawnId;
}

template< class type >
ID_INLINE bool idEntityPtr<type>::SetSpawnId( int id ) {
	// the reason for this first check is unclear:
	// the function returning false may mean the spawnId is already set right, or the entity is missing
	if ( id == spawnId ) {
		return false;
	}
	if ( ( id >> GENTITYNUM_BITS ) == gameLocal.spawnIds[ id & ( ( 1 << GENTITYNUM_BITS ) - 1 ) ] ) {
		spawnId = id;
		return true;
	}
	return false;
}

template< class type >
ID_INLINE bool idEntityPtr<type>::IsValid( void ) const {
	return ( gameLocal.spawnIds[ spawnId & ( ( 1 << GENTITYNUM_BITS ) - 1 ) ] == ( spawnId >> GENTITYNUM_BITS ) );
}

template< class type >
ID_INLINE type *idEntityPtr<type>::GetEntity( void ) const {
	int entityNum = spawnId & ( ( 1 << GENTITYNUM_BITS ) - 1 );
	if ( ( gameLocal.spawnIds[ entityNum ] == ( spawnId >> GENTITYNUM_BITS ) ) ) {
		return static_cast<type *>( gameLocal.entities[ entityNum ] );
	}
	return NULL;
}

template< class type >
ID_INLINE int idEntityPtr<type>::GetEntityNum( void ) const {
	return ( spawnId & ( ( 1 << GENTITYNUM_BITS ) - 1 ) );
}

//============================================================================

class idGameError : public idException {
public:
	idGameError( const char *text ) : idException( text ) {}
};

//============================================================================


//
// these defines work for all startsounds from all entity types
// make sure to change script/tdm_defs.script if you add any channels, or change their order
//
typedef enum {
	SND_CHANNEL_ANY = SCHANNEL_ANY,
	SND_CHANNEL_VOICE = SCHANNEL_ONE,
	SND_CHANNEL_VOICE2,
	SND_CHANNEL_BODY,
	SND_CHANNEL_BODY2,
	SND_CHANNEL_BODY3,
	SND_CHANNEL_WEAPON,
	SND_CHANNEL_ITEM,
	SND_CHANNEL_HEART,
	SND_CHANNEL_UNUSED,
	SND_CHANNEL_DEMONIC,
	SND_CHANNEL_UNUSED_2,

	// internal use only.  not exposed to script or framecommands.
	// tels: Have now been exposed to script and framecommands. Why were they internal only?
	SND_CHANNEL_AMBIENT,
	SND_CHANNEL_DAMAGE
} gameSoundChannel_t;

// content masks
#define	MASK_ALL					(-1)
#define	MASK_SOLID					(CONTENTS_SOLID)
#define	MASK_MONSTERSOLID			(CONTENTS_SOLID|CONTENTS_MONSTERCLIP|CONTENTS_BODY)
#define	MASK_PLAYERSOLID			(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_BODY)
#define	MASK_DEADSOLID				(CONTENTS_SOLID|CONTENTS_PLAYERCLIP)
#define	MASK_WATER					(CONTENTS_WATER)
#define	MASK_OPAQUE					(CONTENTS_OPAQUE)
#define	MASK_SHOT_RENDERMODEL		(CONTENTS_SOLID|CONTENTS_RENDERMODEL)
#define	MASK_SHOT_BOUNDINGBOX		(CONTENTS_SOLID|CONTENTS_BODY)

const float DEFAULT_GRAVITY			= 1066.0f;
#define DEFAULT_GRAVITY_STRING		"1066"
const idVec3 DEFAULT_GRAVITY_VEC3( 0, 0, -DEFAULT_GRAVITY );

const int	CINEMATIC_SKIP_DELAY	= SEC2MS( 2.0f );

//============================================================================

#include "physics/Force.h"
#include "physics/Force_Constant.h"
#include "physics/Force_Drag.h"
#include "physics/Force_Field.h"
#include "physics/Force_Spring.h"
#include "physics/Physics.h"
#include "physics/Physics_Static.h"
#include "physics/Physics_StaticMulti.h"
#include "physics/Physics_Base.h"
#include "physics/Physics_Actor.h"
#include "physics/Physics_Monster.h"
#include "physics/Physics_Player.h"
#include "physics/Physics_Parametric.h"
#include "physics/Physics_RigidBody.h"
#include "physics/Physics_AF.h"
#include "physics/Physics_Liquid.h"

#include "SmokeParticles.h"

#include "Entity.h"
#include "GameEdit.h"
#include "AF.h"
#include "IK.h"
#include "AFEntity.h"
#include "Misc.h"
#include "Actor.h"
#include "Projectile.h"
#include "Weapon.h"
#include "Light.h"
#include "WorldSpawn.h"
#include "Item.h"
#include "PlayerView.h"
#include "PlayerIcon.h"
#include "Player.h"
#include "Mover.h"
#include "Camera.h"
#include "Moveable.h"
#include "Target.h"
#include "Trigger.h"
#include "Sound.h"
#include "FX.h"
#include "SecurityCamera.h"
#include "BrittleFracture.h"
#include "Liquid.h"

#include "ai/AI.h"
#include "anim/Anim_Testmodel.h"

#include "script/Script_Compiler.h"
#include "script/Script_Interpreter.h"
#include "script/Script_Thread.h"

const float	RB_VELOCITY_MAX				= 16000;
const int	RB_VELOCITY_TOTAL_BITS		= 16;
const int	RB_VELOCITY_EXPONENT_BITS	= idMath::BitsForInteger( idMath::BitsForFloat( RB_VELOCITY_MAX ) ) + 1;
const int	RB_VELOCITY_MANTISSA_BITS	= RB_VELOCITY_TOTAL_BITS - 1 - RB_VELOCITY_EXPONENT_BITS;
const float	RB_MOMENTUM_MAX				= 1e20f;
const int	RB_MOMENTUM_TOTAL_BITS		= 16;
const int	RB_MOMENTUM_EXPONENT_BITS	= idMath::BitsForInteger( idMath::BitsForFloat( RB_MOMENTUM_MAX ) ) + 1;
const int	RB_MOMENTUM_MANTISSA_BITS	= RB_MOMENTUM_TOTAL_BITS - 1 - RB_MOMENTUM_EXPONENT_BITS;
const float	RB_FORCE_MAX				= 1e20f;
const int	RB_FORCE_TOTAL_BITS			= 16;
const int	RB_FORCE_EXPONENT_BITS		= idMath::BitsForInteger( idMath::BitsForFloat( RB_FORCE_MAX ) ) + 1;
const int	RB_FORCE_MANTISSA_BITS		= RB_FORCE_TOTAL_BITS - 1 - RB_FORCE_EXPONENT_BITS;

#endif	/* !__GAME_LOCAL_H__ */
