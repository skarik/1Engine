
#include <sstream>
using std::stringstream;

#include "CWorldState.h"

#include "core/time.h"
#include "core/settings/CGameSettings.h"
#include "core/system/io/CBinaryFile.h"


CWorldState* ActiveGameWorld = NULL;

CWorldState::CWorldState ( void )
{
	ActiveGameWorld = this;
}
CWorldState::~CWorldState ( void )
{
	if ( ActiveGameWorld == this )
		ActiveGameWorld = NULL;
}


void CWorldState::Save ( void )
{
	CBinaryFile file;
	stringstream tempStream;
	string sFilename;
	tempStream << CGameSettings::Active()->MakeWorldSaveDirectory() << ".state";
	sFilename = tempStream.str();
	
	// Open file for writing
	file.Open( sFilename.c_str(), CBinaryFile::IO_WRITE );

	if ( file.IsOpen() )
	{
		// Time variables
		file.WriteData( (char*)this, sizeof(CWorldState) );

		// Close file
		file.Close();
	}
}
void CWorldState::Load ( void )
{
	CBinaryFile file;
	stringstream tempStream;
	string sFilename;
	tempStream << CGameSettings::Active()->MakeWorldSaveDirectory() << ".state";
	sFilename = tempStream.str();
	if ( file.Exists( sFilename.c_str() ) )
	{
		// == Open file for reading ==
		file.Open( sFilename.c_str(), CBinaryFile::IO_READ );

		// Time variables
		file.ReadData( (char*)this, sizeof(CWorldState) );

		// == Close file ==
		file.Close();
	}
	else
	{
		SetDefaults();
		Save();
	}
}
void CWorldState::SetDefaults ( void )
{
	fCurrentDay = 0;
	fCurrentTime = 60*60*2;
	fTimeSpeed = 60;
}


void CWorldState::Tick ( void )
{
	//fTimeSpeed = 60;

	if (( fCurrentDay < 1 )&&( bIsDaytime ))	// Make the first day twice as long
		fCurrentTime += Time::smoothDeltaTime * fTimeSpeed * 0.5f;
	else
		fCurrentTime += Time::smoothDeltaTime * fTimeSpeed;

	if( fCurrentTime > 86400 ) // Checks if a full day cycle has passed.
	{
		fCurrentTime = 0; // Resets time of day to 0(zero) if complete cycle occures.
		// Increment a day
		fCurrentDay += 1;
	}

	if( fCurrentTime <= (86400 / 2) ) // Checks if half the day cycle is over.
	{
		bIsDaytime = true; // If day cycle is less then or equal to half the cycle, day time is true.
	}
	else
	{
		bIsDaytime = false; // if day cycle is greater than half the cylce, day time is false.
	}

	cBaseAmbient = Color ( 0,0,0 );
	cBaseColor = Color( 0,0,0 );
}