/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////// i just wanted to make a line of slashes this long /////////////
#ifndef _C_ZONED_CHARACTER_H_
#define _C_ZONED_CHARACTER_H_

#include "CZonedCharacterController.h"
#include "after/types/character/NPC_WorldState.h"

namespace NPC
{
	//#define DEFINE_ZCC(a) explicit a ( const Vector3d & inPosition, const Rotator & inRotation=Rotator() );
	#define DEFINE_ZCC(a) explicit a ( const Vector3d & inPosition, const uint64_t & inID, const Rotator & inRotation=Rotator() ); \
		protected: \
			void ZCC_SetCharacterID ( void ) { ZCC_CharacterId = ( #a ); }; \
			const char* const ZCC_GetCharacterId ( void ) { return ZCC_CharacterId.c_str(); }; \
			arstring128 ZCC_CharacterId; \
		private: \
			static CZonedCharacterController::Registrar<a> Reg; \
		public:
	#define CONSTRUCTOR_ZCC(a) a :: a ( const Vector3d & inPosition, const Rotator & inRotation ) : CZonedCharacter ( inPosition, inRotation ) { ZCC_AddInstance();
	#define END_GROUP_ZCC }
	#define DESTRUCTOR_ZCC(a) a :: ~ ## a ( void ) { ZCC_RemoveInstance();
	#define CONSTRUCTOR_ZCC_V2(a) a ( const Vector3d & inPosition, const uint64_t & inID, const Rotator & inRotation ) : CZonedCharacter ( inPosition, inID, inRotation )
	#define REGISTER_ZCC(a) CZonedCharacterController::Registrar<a> a :: Reg ( ( #a ) );
}

// Includes
#include "renderer/logic/model/CModel.h"

#include "after/entities/character/CCharacter.h"
#include "after/terrain/Zones.h"

// Prototypes
class CPlayer;

namespace NPC
{
	// ZonedCharacter-san
	class CZonedCharacter : public CCharacter
	{
		ClassName( "CZonedCharacter" );
	public:
		explicit CZonedCharacter ( const Vector3d & inPosition, const uint64_t & inID, const Rotator & inRotation=Rotator() );
		~CZonedCharacter ( void );

		virtual void PostLoad ( void ) {} // Called specifically for character NPCs. Used for initializing stats.

		void Update ( void );		// You should not override update if you're making a character from this class.
		void LateUpdate ( void );
		void PostUpdate ( void );

		void SaveInfo ( void );
		void LoadInfo ( void );

		// Save stuff with these guys
		//virtual void SaveTo ( ostringstream & ) {};
		//virtual void LoadFrom ( istringstream & ) {};

		// This is called when the NPC is active. You can put AI and such in here.
		virtual void UpdateActive ( void )=0;
		// This is called when the NPC is inactive. This is where you put extremely simple behaviors.
		// For example, you can put code to bring the character back into range, or to just destroy the character.
		// Note that even with this function, after a certain distance and time delay, the character WILL be saved and destroyed.
		virtual void UpdateInactive ( void )=0;

		// The following are targets for current biome and terra
		static short biomeType;	// They are updated by the NPC controller, in this case, CNpcSpawner
		static short terraType;

		// These are the CZonedCharacter properties. They can be overridden by child classes.
		virtual short MaxSpawnCount ( void ) { return 1; }
		virtual short IsUniqueNPC ( void ) { return 0; }

		// Returns the class's current value of currentActivePlayer, which is a pointer to the current active
		// player object.
		static CPlayer* GetActivePlayer ( void ) { return currentActivePlayer; }
		// Returns the current char model. Is used by the AI for common attack. However, common attack should be moved into the
		// NPC code.
		CCharacterModel* GetCharModel ( void ) { return pCharModel; }

		// Returns the instance's ZCC-NPC id
		uint64_t	GetNPCID ( void ) { return characterId; }

	public:
		// Called by the AI when a character triggers a talk to this character
		virtual void OnTalkTo ( CCharacter* talkingCharacter ) {}

		// AI Calls this to attack
		virtual bool PerformAttack ( void ) { return false; }
		// AI Calls this to defend
		virtual bool PerformDefend ( void ) { return false; }
		// AI Calls this to swap equipment (returns false on unsuccessful swap)
		virtual bool PerformSwapEquipped ( const uint n_targetequipment_type ) { return false; }
		// AI Calls this to see if this character is an ally
		virtual bool IsAlliedToCharacter ( CCharacter* character ) { return GetTypeName()==character->GetTypeName(); }
		// AI Calls this to see if this character is an enemy
		virtual bool IsEnemyToCharacter ( CCharacter* character ) { return false; }
	protected:

		// This is a model pointer that can be used by inheriting classes.
		// What's nice about this guy is that the visibility and unloading for it is handled by this base class.
		// Thus, you should use this if you're thinking of giving your character class a general model.
		CModel*	pModel;
		// If you're using a model, and you need an animation, you'll need to make it yourself.
		// Set this guy up if you really want it.
		CAnimation* animator;
		// However, say that managing a simple model like that isn't your cup of tea, there's also this bad boy.
		// That's right, this base class also manages a CCharacterModel instance.
		// You should use this if you need your character class to have a character model.
		// A CCharacterModel instance handles its own animation state as well, so no need to make one yourself.
		CCharacterModel* pCharModel;
		// This is a pointer to the current active player.
		static CPlayer*	currentActivePlayer;

		// *The following are the options for Zoned character behavior
		// If set to true, this character's info is saved.
		//bool	bRememberMe;	//always true

		// These are used for keeping track of saved and loaded characters, as to prevent duplicate information.
		// It is advised to not edit these after the constructor has done its thing.
		//int		iCharacterID;
		//string		sCharacterID;
		uint64_t	characterId;

		// Character file for storing values
		characterFile_t	characterFile;

	protected:
		// The following is for managing the list of active zoned characters
		virtual void ZCC_SetCharacterID ( void )=0;
		void ZCC_AddInstance ( void )
		{
			//cout << "Begin class register" << endl;
			zcc_characterinfo_t newinfo;
			ZCC_SetCharacterID();
			//cout << "ID set" << endl;
			newinfo.name = ZCC_GetCharacterId();
			//cout << "Name grabbed" << endl;
			newinfo.pointer = this;
			//vCharacterList.push_back( newinfo );
			newinfo.id = characterId;

			Manager->AddCharacter( newinfo );

			this->RemoveReference();
		}
		void ZCC_RemoveInstance ( void )
		{
			/*for ( vector<characterinfo_t>::iterator it = vCharacterList.begin(); it != vCharacterList.end(); ++it ) {
				if ( it->pointer == this ) {
					vCharacterList.erase( it );
					return;
				}
			}
			cout << "Warning: unable to remove character from active list." << endl;*/
			if ( Manager )
			{
				Manager->RemoveCharacter( this );
			}
		}
		//static vector<characterinfo_t> vCharacterList;
		virtual const char* const ZCC_GetCharacterId ( void )=0;

		// Turns off saving for on death
		void ZCC_DisableSave ( void ) {
			bSaveOnUnload = false;
		}

	private:
		friend	CZonedCharacterController;

		Real	fOutOfRangeTime;
		bool	bOutOfRange;

		Real	fActiveAreaCheckTime;
		bool	bOutOfActiveArea;

		bool	bSaveOnUnload;
	};

}

#endif