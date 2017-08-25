//===============================================================================================//
//
//		class CGameState
//
// Container and manager for all game-based objects.
//
//===============================================================================================//

#ifndef ENGINE_C_GAME_STATE_
#define ENGINE_C_GAME_STATE_

// Includes
#include "engine/types.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine/state/CGameScene.h"
#include "engine/system/CGameMessenger.h"
#include "physical/physics/IPrGameUpdate.h"
#include <memory>
#include <vector>
#include <mutex>

// Prototypes
class CResourceManager;

// Deletion Struct Definition
struct sObjectICounter
{
	CGameBehavior*	pBehavior;
	unsigned int	iCounter;
};

struct sObjectFCounter
{
	CGameBehavior*	pBehavior;
	unsigned int	id;
	float			fCounter;
};

// Class Definition
class CGameState : public IPrGameUpdate
{
	typedef std::mutex		mutex;
public:
	// Constructor and destructor
	ENGINE_API				CGameState ();
	ENGINE_API				~CGameState();

	// Step update for game logic
	ENGINE_API void			Update ( void );
	ENGINE_API void			LateUpdate ( void );

	// Fixed timestep for physics
	ENGINE_API void			FixedUpdate ( void ) override;
	ENGINE_API void			RigidbodyUpdate ( Real interpolation ) override;

	// Accessor
	ENGINE_API static CGameState*	Active ( void );

	// -Getter functions for good stuff-

	// Returns the object with the given ID, NULL if out of range.
	ENGINE_API CGameBehavior* GetBehavior ( gameid_t id )
	{
		if ( id >= iCurrentIndex ) {
			return NULL;
		}
		return pBehaviors[id];
	}

	// Remover functions for other stuff
	ENGINE_API void DeleteObject ( CGameBehavior* );
	ENGINE_API void DeleteObjectDelayed ( CGameBehavior*, float );

	// == Finder functions ==
	// Returns if given behavior pointer is in the list
	ENGINE_API bool ObjectExists ( CGameBehavior* );

	// Returns a list of objects in the given layer.
	// The list must be freed by the user.
	ENGINE_API std::vector<CGameBehavior*>* FindObjectsWithLayer ( physical::prLayer );

	// Returns the first found object with the target name string.
	// NULL is returned otherwise.
	ENGINE_API CGameBehavior*	FindFirstObjectWithName ( const string & );

	// Returns the first found object with the target typename string.
	// NULL is returned otherwise.
	ENGINE_API CGameBehavior*	FindFirstObjectWithTypename ( const string & );
	// Gives a list of objects with the target typename string.
	ENGINE_API void FindObjectsWithTypename ( const string &, std::vector<CGameBehavior*> & );

	// Returns a pointer to the spot in the array where the object is held.
	// This is very unsafe and can cause memory leaks if not used properly!
	// Avoid use of this function if possible!
	ENGINE_API CGameBehavior**	GetBehaviorArrayPointer ( CGameBehavior* );

	// == Removing all objects ==
	ENGINE_API void CleanWorld ( void );

	// == Ending Game ==
	ENGINE_API void EndGame ( void );
	ENGINE_API FORCE_INLINE bool EndingGame ( void ) { return bEndingGame; }

	// == Scene Management ==
	ENGINE_API void SetNextScene ( CGameScene* scene ) { pNextScene = scene; bSceneActive = false; };
	ENGINE_API CGameScene* GetNextScene ( void ) { return pNextScene; };
public:
	// Public active instance pointer
	//static CGameState* pActive;
	// Public messenger
	CGameMessenger	messenger;
	// Public resource manager
	CResourceManager*	mResourceManager;

private:
	// Give base GameBehavior access.
	friend CGameBehavior;
	// Give Messenger full access to send messages through game
	friend CGameMessenger;
	// Adding and removing renderer objects
	gameid_t AddBehavior ( CGameBehavior* );
	void RemoveBehavior ( const gameid_t );
	// Generate a unique GUID for the caller
	guid32_t GetGUID ( void );
	// TODO
	void CleanBehaviorList ( void );

private:
	static CGameState*	mActive;
	static guid32_t		nextGUID;

	// -- Game Behavior Management --
	// Private list of game objects
	CGameBehavior** pBehaviors;
	//auto_ptr<CGameBehavior>* pBehaviors;
	unsigned int	iCurrentIndex;
	unsigned int	iListSize;
	// Mutex for threaded access of game objects
	mutex	mtListLock;

	// Deletion List for objects thrown into DeleteObject().
	std::vector<sObjectFCounter>	vDeletionList;
	std::vector<CGameBehavior*>		vCreationList;

	// -- Game Scene Management --
	CGameScene*	pScene;
	CGameScene*	pNextScene;
	bool		bSceneActive;

	bool		bEndingGame;
};


#endif//ENGINE_C_GAME_STATE_