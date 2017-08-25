
#ifndef _C_SPEECH_TRIE_H_
#define _C_SPEECH_TRIE_H_

#include <array>
#include <string>

#include "core/common.h"
#include "core/types/types.h"

#include "after/types/character/Dialogue.h"
#include "after/states/CharacterStats.h"

class CSpeechTrie
{
public:
	explicit CSpeechTrie();
	~CSpeechTrie();

	void SetKey ( const NPC::eGeneralSpeechType, const eCharacterGender, const eCharacterRace, const char* );
	char* GetKey ( const NPC::eGeneralSpeechType, const eCharacterGender, const eCharacterRace ) const;

protected:
	struct raceKey_t
	{
		eCharacterRace value;
		char* child;
	};
	struct genderKey_t
	{
		eCharacterGender value;
		std::array<raceKey_t*,7> children;
	};
	struct speechKey_t
	{
		NPC::eGeneralSpeechType value;
		std::array<genderKey_t*,2> children;
	};
	std::array<speechKey_t*,20> rootKeys;
	

};


#endif//_C_SPEECH_TRIE_H_