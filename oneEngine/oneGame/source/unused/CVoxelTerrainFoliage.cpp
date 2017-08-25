
#include "CVoxelTerrain.h"

// Include Generator
#include "CTerrainGenerator.h"

// Include all the foliage types
#include "CFoliage.h"
#include "CTreeBase.h"

#include "FoliageBushGreen.h"
#include "FoliageTreePine.h"
#include "FoliageTreeWarped.h"

#include "FoliageCactusDefault.h"

#include <string>
using std::string;

void CVoxelTerrain::TerraGen_NewFoliage ( CBoob * pBoob, Vector3d const& newPos, string const& sTypeName )
{
	int iGenerationMode = generator->GetGenerationMode();
	if (( iGenerationMode == CTerrainGenerator::GENERATE_TO_MEMORY )||( iGenerationMode == CTerrainGenerator::GENERATE_DUAL_SAVE ))
	{
		TerraFoiliage	newFoliageRequest;
		newFoliageRequest.foliage_index	= GetFoliageType( sTypeName );
		newFoliageRequest.position	= newPos;
		//newFoliageRequest.userdata	= char(0);
		memset( newFoliageRequest.userdata, 0xFF, 48 );
		newFoliageRequest.myBoob = pBoob;
		vFoliageQueue.push_back( newFoliageRequest );
	}

	if (( iGenerationMode == CTerrainGenerator::GENERATE_TO_DISK )||( iGenerationMode == CTerrainGenerator::GENERATE_DUAL_SAVE ))
	{
		// TODO: TO-DISK GENERATION
		// we could put it into an array, probably.
		// seeing as the bitmask would be zero, that would be okay
		CMetaboob::sFoliageIO newFoliageIORequest;
		newFoliageIORequest.foliage_index = GetFoliageType( sTypeName );
		newFoliageIORequest.position	= newPos;
		//newFoliageIORequest.userdata	= char(0);
		memset( newFoliageIORequest.userdata, 0xFF, 48 );
		((CMetaboob*)pBoob)->v_foliageIO.push_back( newFoliageIORequest );
	}
}
void CVoxelTerrain::TerraGen_NewGrass ( CBoob * pBoob, Vector3d const& newPos, unsigned short type )
{
	int iGenerationMode = generator->GetGenerationMode();
	if (( iGenerationMode == CTerrainGenerator::GENERATE_TO_MEMORY )||( iGenerationMode == CTerrainGenerator::GENERATE_DUAL_SAVE ))
	{
		sTerraGrass newGrass;
		newGrass.position = newPos;
		newGrass.type = type;
		pBoob->v_grass.push_back( newGrass );
	}

	if (( iGenerationMode == CTerrainGenerator::GENERATE_TO_DISK )||( iGenerationMode == CTerrainGenerator::GENERATE_DUAL_SAVE ))
	{
		// TODO: TO-DISK GENERATION
		sTerraGrass newGrass;
		newGrass.position = newPos;
		newGrass.type = type;
		pBoob->v_grass.push_back( newGrass );
	}
}

unsigned short CVoxelTerrain::GetFoliageType ( CFoliage* pFoliage )
{
	string tname = pFoliage->GetTypeName();

	/*if ( tname == "CFoliage" )
		return 0;
	else if ( tname == "CTreeBase" )
		return 1;
	else if ( tname == "FoliageBushGreen" )
		return 2;*/
	unsigned short val = GetFoliageType( tname );
	if ( val == (unsigned short)(-1) )
	{
		std::cout << "Warning: Given object '" << tname << "' has no valid Foliage type." << std::endl;
	}
	//return (unsigned short)(-1);
	return val;
}
unsigned short CVoxelTerrain::GetFoliageType ( string const& stName )
{
	if ( stName == "CFoliage" )
		return 0;
	else if ( stName == "CTreeBase" )
		return 1;
	else if ( stName == "FoliageBushGreen" )
		return 2;
	else if ( stName == "FoliageTreePine" )
		return 3;
	else if ( stName == "FoliageCactusDefault" )
		return 4;
	else if ( stName == "FoliageTreeWarped" )
		return 5;

	return (unsigned short)(-1);
}

CFoliage* CVoxelTerrain::CreateFoliage ( unsigned short iType, Vector3d const& vInPos, const char* iInData )
{
	CFoliage* result = NULL;
	if ( iType == 0 ) {
		std::cout << "Invalid foliage type" << std::endl;
	}
	else if ( iType == 1 )
	{
		result = new CTreeBase();
		result->transform.position = vInPos;
		((CTreeBase*)(result))->Generate();
		((CTreeBase*)(result))->SetToggle( iInData );
	}
	else if ( iType == 2 )
	{
		result = new FoliageBushGreen();
		result->transform.position = vInPos;
		((CTreeBase*)(result))->Generate();
		((CTreeBase*)(result))->SetToggle( iInData );
	}
	else if ( iType == 3 )
	{
		result = new FoliageTreePine();
		result->transform.position = vInPos;
		((CTreeBase*)(result))->Generate();
		((CTreeBase*)(result))->SetToggle( iInData );
	}
	else if ( iType == 4 )
	{
		result = new FoliageCactusDefault();
		result->transform.position = vInPos;
		((CTreeBase*)(result))->Generate();
		((CTreeBase*)(result))->SetToggle( iInData );
	}
	else if ( iType == 5 )
	{
		result = new FoliageTreeWarped();
		result->transform.position = vInPos;
		((CTreeBase*)(result))->Generate();
		((CTreeBase*)(result))->SetToggle( iInData );
	}

	return result;
}

void CVoxelTerrain::CreateQueuedFoliage ( void )
{
	while ( !vFoliageQueue.empty() )
	{
		TerraFoiliage currentRequest = vFoliageQueue.back();
		vFoliageQueue.pop_back();

		// First need to check if boob exists, I think
		if ( BoobExists( currentRequest.myBoob ) )
		{
			CFoliage* newFoliage = CreateFoliage( currentRequest.foliage_index, currentRequest.position, currentRequest.userdata );
			//newFoliage->pBoob = currentRequest.myBoob;
			currentRequest.myBoob->v_foliage.push_back( newFoliage );
		}
	}
}