
#include "CPlayerStats.h"

namespace Races
{

	string GetRaceGeneralDesc ( const eCharacterRace race )
	{
		switch ( race )
		{
		case CRACE_HUMAN: return "Humans are just an all-around race.";
		case CRACE_ELF: return "The last existing race of elves are all-around in a different way.";
		case CRACE_DWARF: return "Dwarves are short and excellent.";
		case CRACE_KITTEN: return "Catpeople are whimsical bla bla this is taking too damn long";
		case CRACE_FLUXXOR: return "John: write Fluxxor description";
		}

		return "";
	}
	string GetRaceBuffDesc ( const eCharacterRace race )
	{
		switch ( race )
		{
		case CRACE_HUMAN: return "Humans don't face any challenges with any skill build.";
		case CRACE_ELF: return "Elves receive a few nighttime bonuses, and are more suited to roundabout styles of play as opposed to humans' directness.";
		case CRACE_DWARF: return "In addition to a high starting health, dwarves can benefit from:\n -Massive INT bonus when working with machines\n -Being short";
		case CRACE_KITTEN: return "Catpeople are a race that specialize in speed. Their benefits include:\n -Fast movement\n -Nightvision\n -Great hearing\nThey suffer from:\n -Low starting health\n -Panic debuff when wet\n -Will occasionally get a 'distracted' status";
		case CRACE_FLUXXOR: return "Fluxxors have a specialized low level skill tree that include:\n -Shared stamina and mana pool\n -Reduction in physical damage\n -Passive dodge for physical attacks";
		}

		return "";
	}

	string GetCharacterDescriptor ( const CRacialStats* race_stats )
	{
		string result = "";
		if ( race_stats->iGender == CGEND_FEMALE ) {
			result = "Female ";
		}
		else {
			result = "Male ";
		}
		switch ( race_stats->iRace )
		{
		case CRACE_HUMAN:	result += "Human";		break;
		case CRACE_ELF:		result += "Dark Elf";	break;
		case CRACE_DWARF:	result += "Dwarf";		break;
		case CRACE_KITTEN:	result += "Catperson";	break;
		case CRACE_FLUXXOR:	result += "Fluxxor";	break;
		case CRACE_MERCHANT:result += "Merchant";	break;
		}
		return result;
	}
}
