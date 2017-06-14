
// class CMCCRealm
//  Class pertaining to realm-wide settings (such as the terrain seed)
// This is specifically meant to make loading and saving realm options easy.
// All instances of this class point to the currently active realm on default.
// Realm options are automatically saved when an instance is destroyed.

#ifndef _C_MCC_REALM_H_
#define _C_MCC_REALM_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"

class CMCCRealm
{

public:
	explicit CMCCRealm ( const char* targetRealm = "" );
	~CMCCRealm ( void );

	// Sets the seed for the realm.
	// If this is the active realm, will set the default seed
	void SetSeed ( const int32_t );
	void SetSeed ( const char* );	// Set seed via string hash w/ special cases
	// Returns the seed for the realm.
	// If this is the active realm, will set the default seed
	int32_t GetSeed ( void );

	// Increment the selection count
	void IncSelectCount ( void );
	// Get the selection count
	uint32_t GetSelectCount ( void );

	// Set the name of the last character played on the world
	void SetSavedSoulname ( const char* );
	// Get the name of the last character played on the world
	const char* GetSavedSoulname ( void );

	// Increment the person count
	void IncPersonCount ( void );
	// Get the person count
	uint32_t GetPersonCount ( void );

	// Saves the realm settings
	void Save ( void );
	// Loads realm settings from file without saving
	void Load ( void );

private:
	void CheckActiveRealm ( void );
	void UploadSettings ( void );

	bool					b_activeRealm;
	arstring<256>			m_targetRealm;
	static arstring<256>	m_currentRealm;

	struct realmSettings_t
	{
		int32_t		seed;
		char		seed_name [256];

		uint32_t	selection_count;
		char		soulname_previous_person [256];
		uint32_t	person_count;
	};
	realmSettings_t			settings;

};


#endif//_C_MCC_REALM_H_