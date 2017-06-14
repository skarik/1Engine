//  class CZonedCharacterController
// extern CZonedCharacterController* NPC::Manager
// 
// Character controller for zoned characters, or characters with existance linked with the terrain.
// 

#ifndef _C_ZONED_CHARACTER_CONTROLLER_H_
#define _C_ZONED_CHARACTER_CONTROLLER_H_

#include <vector>
#include <map>

#include "engine/behavior/CGameObject.h"
#include "after/types/WorldVector.h"

namespace NPC
{
	class CZonedCharacter;

	// Instantiation template
	template<typename T> CZonedCharacter * _instZCC( const Vector3d & inPosition, const Rotator & inRotation, const uint64_t & inID ) { return new T(inPosition,inID,inRotation); }

	// NPC id type
	typedef uint64_t	npcid_t;

	// NPC Enum ID
	enum eNPC_ID_TYPE
	{
		npcid_FAUNA,
		npcid_UNIQUE,
		npcid_SETPIECE
	};
	// NPC info
	struct zcc_characterinfo_t
	{
		string		name;
		uint64_t	id;
		CZonedCharacter* pointer;
		RangeVector	start_li_position;
	};
	// External NPC info
	struct zcc_characterinfo_extern_t
	{
		uint64_t	id;
		Vector3d	position;
		RangeVector	li_position;
	};

	// Zoned Character Controller/Manager class
	class CZonedCharacterController : public CGameBehavior
	{
	public:
		CZonedCharacterController ( void );
		~CZonedCharacterController ( void );

		void ReadyUp ( void );

		void Update ( void );
		void LateUpdate ( void );
		void PostUpdate ( void );

		int GetCharacterCount ( const string & );
		#define GetCount(a) GetCharacterCount(string( #a ))

		// == Character System Management ==
		//  AddCharacter()
		// To be called in ZCC constructor
		void		AddCharacter ( zcc_characterinfo_t & );
		//  RemoveCharacter()
		// To be called in ZCC destructor
		void		RemoveCharacter ( CZonedCharacter* );
		//	SaveCharacterOnUnload()
		// Called when character goes out of range of the game state.
		void		SaveCharacterOnUnload ( CZonedCharacter* );
		//	SaveCharacterLoad()
		// Called when character is created.
		void		SaveCharacterLoad ( CZonedCharacter* );

		// == Character Game Management ==
		//	GetCharactersInParty()
		// Populates the list with all the NPCs in the given party
		void		GetCharactersInParty ( std::vector<uint64_t>&, uint64_t );

		// == Character Grabbing ==
		//	CheckNPC()
		// Returns true if an NPC of the type already exists
		bool				CheckNPC ( uint64_t ntarget_id );
		//  RequestNPC()
		// Request an NPC id to use. Marks NPC id as used. (Only unused IDs are saved)
		uint64_t			RequestNPC ( eNPC_ID_TYPE nid_type, uint64_t ntarget_id = uint64_t(-1) );
		//  SpawnNPC()
		// Using the given ID, spawns an NPC. If the NPC file does not exist, will generate the id.
		CZonedCharacter*	SpawnNPC ( uint64_t );
		//  FreeNPCID()
		// Frees the given ID. This deletes the NPC off the disk and removes shit and yeah!
		void				FreeNPCID ( uint64_t );
		//  GetNPCList()
		// Populates a list of NPC ids that are currently spawned.
		void				GetNPCList ( std::vector<uint64_t>& );
		//  GetNPC()
		// Returns a character pointer to the character that matches with the ID. ID's 1024 and above are unique.
		CZonedCharacter*	GetNPC ( uint64_t );

		//	Spawn


		// == Generation ==
		//  GenerateNPC
		// Generates an NPC given the NPC ID and the region to associate the NPC's start area with.
		void				GenerateNPC ( uint64_t nid, const rangeint& x, const rangeint& y );
		void				GenerateNPC ( uint64_t nid, uint32_t nregion );

		// == Character Factory ==
		CZonedCharacter* SpawnCharacter ( const string & s , const Vector3d & inPosition, const uint64_t & inID, const Rotator & inRotation=Rotator() );
		CZonedCharacter* SpawnFauna ( const string & s , const Vector3d & inPosition, const Rotator & inRotation=Rotator() );
		typedef std::pair<string,CZonedCharacter*(*)( const Vector3d & inPosition, const Rotator & inRotation, const uint64_t & inID )> pairtype;
		// Factory for creating zoned characters
		struct BaseFactory {
			typedef std::map<std::string, CZonedCharacter*(*)( const Vector3d & inPosition, const Rotator & inRotation, const uint64_t & inID )> map_type;

			static CZonedCharacter * createInstance(std::string const& s, const Vector3d & inPosition, const uint64_t & inID, const Rotator & inRotation=Rotator() ) {
				map_type::iterator it = getMap()->find(s);
				if(it == getMap()->end()) {
					std::cout << "Type " << s << " is not registered!" << std::endl;
					return 0;
				}
				return it->second( inPosition, inRotation, inID );
			}
		protected:
			static map_type * getMap() {
				if (!map) { map = new map_type; } 
				return map; 
			}
		private:
			static map_type * map;
		};
		// Registration class for zoned characters
		template<typename T>
		struct Registrar : BaseFactory {
			Registrar( const string & s ) {
				getMap()->insert(pairtype (s, &_instZCC<T>));
			};
		};

	private:
		void		LoadSector ( const RangeVector& );

	private:
		// == Creation State ==
		uint16_t	nextFaunaNPC;
		uint64_t	nextFreeNPC; // needs to load from realmstate
		uint64_t	freeIDcount; // needs to load from realmstate

		std::vector<zcc_characterinfo_t>	characterList;
		std::vector<zcc_characterinfo_extern_t>	externalCharacterList;
		std::vector<RangeVector>			externalCharacterSectors;

		bool		systemReady;
		ftype		updateTimer;
		ftype		npcsimTimer;

	};

	// Global instance of the ZonedCharacter Controller
	extern CZonedCharacterController* Manager;
}


#endif