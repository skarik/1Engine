
#include "NamingUtils.h"
#include "core/math/random/Random.h"


const char townNameParts [][8] = {
	"lyar","nu",
	"kok","ri","ki",
	"zor","ra","ko",
	"sar","nith","sinth",
	"ab","ayur",
	"ser","rin","len","burg",
	"san","clem","ment","te",
	"tor","ro",
	"ter","rib",
	"whit","man",
	"flux","nyan","rok",
	"wo","chi","cha",
	"ver","rit",
	"verd","eros","eos","fort","tit"
};
const int townNamePartCount = 40;

/*Race, Geography, Resources
Race would determine which string set to use for name generating.
Geography: Mountains, Valleys, Plains, Seaside, Border,
Resources: Wood, iron, wheat, stone, fruit, herbs, other stuff
*/

const char meowNameParts [][8] =  {
	"Ya", "Ku",
	"Young", "Jao", "Zhi",
	"Cho", "Shi", "Neun",
	"Van", 

};

//Mountain, Valley, Plains, Forest, Seaside, Border
const char meowGeoParts [][8] = {
	"shiao", "gyuu",
	"chodo", "seul",
	"haga", "jiko"
};

//Wood, Stone, Farmland, luxury
const char meowResParts [][8] = {
	"magu", "iki",
	"nokchi", "chisyul"
};

const char humanNameParts [][8] = {
	"Whit", "Clem",
	"Zute", "Bose",
	"Org", "Per",
	"Pull", "Sarp",
	"Art", "Rec",
	"Blat", "Lon",
	"Cas", "Sec"
};

const char humanGeoParts [][10] = {
	"Mountain", "Valley", 
	"Plains", "Forest",
	"Port", "Border"
};

const char humanResParts [][8] = {
	"lig", "cal",
	"dum", "centia"
};

const char gypsyNameParts [][8] = {
	""

};

const char CharacterNames [][15] = {
	"Brian", "James",
	"John",  "Josh", 
	"Amelia", "Sam",
	"Jeonsuk", "Haruka",
	"Jesus", "Jorge",
	"Juan", "Juancho",
	"Flubbergus", "Hime",
	"Hyung", "Ben", 
	"James", "Ashley",
	"Amy", "Hanna",
	"Cody", "Matt",
	"Mike", "Nate",
	"Chad", "Melissa",
	"Chiaki", "Azusa",
	"Jin", "Rin",
	"ERROR", "Jean",
	"Philipe", "Barbara",
	"Cari", "Debbie",
	"Elora", "Fragnorach",
	"Jermaine", "Katherine"
};

arstring<128>	NamingUtils::CreateBaseTownName ( void )
{
	arstring<128> result ("");
	char count = Random.Next()%2+2;
	
	while ( count > 0 ) {
		result += townNameParts[Random.Next()%townNamePartCount];
		count--;
	}

	return result;
}

arstring<128> NamingUtils::CreateDetailedTownName ( sNameInfo base )
{
	switch (base.race)
	{
	case CRACE_HUMAN:
		return CreateHumanTownName (base);
	case CRACE_ELF:
		return CreateGypsyTownName (base);
	case CRACE_DWARF:
		return CreateDwarfTownName (base);
	case CRACE_KITTEN:
		return CreateMeowTownName (base);
	case CRACE_FLUXXOR:
		return CreateFluxTownName (base);
	default:
		return CreateBaseTownName();
	}
}

arstring<128>	NamingUtils::CreateHumanTownName ( sNameInfo base )
{
	arstring<128> result ("");
	
	result += humanNameParts [Random.Next() % 14];
	result += humanResParts [base.resource];
	result += " ";
	if (Random.Next() % 2)
		result += humanGeoParts [base.geo];
	else
		result += "Town";

	return result;
}

arstring<128>	NamingUtils::CreateGypsyTownName ( sNameInfo base )
{
	arstring<128> result ("Blorg");
	return result;
}

arstring<128>	NamingUtils::CreateDwarfTownName ( sNameInfo base)
{
	arstring<128> result ("Dharg");
	return result;
}

arstring<128> NamingUtils::CreateMeowTownName ( sNameInfo base )
{
	arstring<128> result ("");
	
	result += meowNameParts [Random.Next() % 9];
	result += meowResParts [base.resource];
	result += meowGeoParts [base.geo];
	result += "-jo";

	return result;
}

arstring<128>	NamingUtils::CreateFluxTownName ( sNameInfo base )
{
	arstring <128> result ("Shooooop");
	return result;
}

//LOOK JOSH! HERE IT IS
arstring<128>	NamingUtils::CreateCharacterName ( void )
{
	int random = Random.Next() % 40; //Apparently this is random enough
	//random = 3; Guaranteed to be random.
				//Source: xkcd.com/221/
	arstring<128> name ("");

	name += CharacterNames [random];
	return name;
}

