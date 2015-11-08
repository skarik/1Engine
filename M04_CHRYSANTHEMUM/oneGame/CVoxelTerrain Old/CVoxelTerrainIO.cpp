
#include "CVoxelTerrain.h"
#include "CGameSettings.h"
#include "CTreeBase.h"
#include <sstream>
using std::stringstream;
/*
filenames:
x_y_z
binary:
VERSION 0
unsigned short version
data*{
	unsigned char [512] terrain_data
	unsigned char [512] water_data
}
unsigned char terrain
unsigned char biome
bufferbufferbuffer

VERSION 1 (THIS IS NOT EVEN FUCKING CORRECT)
unsigned short version
data*{
	unsigned char terrain_data
	unsigned char water_data
}
unsigned char terrain
unsigned char biome
data*{
	unsigned short big_objects
}

VERSION 2
unsigned short version
data*{
	unsigned char terrain_data
}
data*{
	unsigned char water_data
}
unsigned char terrain
unsigned char biome
data*{
	unsigned short big_objects
	unsigned long little_objects
	unsigned int medium_objects
}

VERSION 3
unsigned short version
data*{
	unsigned char terrain_data
}
data*{
	unsigned char water_data
}
unsigned char terrain
unsigned char biome
uint32_t foliage_number
data*{
	unsigned short foliage_index
	float x, y, z
	uint64_t userbitmask
}


VERSION 4
unsigned short version
data*{
	unsigned char terrain_data
}
data*{
	unsigned char water_data
}
unsigned char terrain
unsigned char biome
uint32_t foliage_number
data*{
	unsigned short foliage_index
	float x, y, z
	uint64_t userbitmask
}
uint32_t component_number
data*{
	unsigned short component_index
	float x,y,z
	float xrot,yrot,zrot
	float xscal,yscal,zscal
	uint64_t userdata
	char b16index
	char b8index
	short bindex
}

VERSION 5
unsigned short version
data*{
	unsigned char terrain_data
}
data*{
	unsigned char water_data
}
unsigned char terrain
unsigned char biome
uint32_t foliage_number
data*{
	unsigned short foliage_index
	float x, y, z
	uint64_t userbitmask
}
uint32_t component_number
data*{
	unsigned short component_index
	float x,y,z
	float xrot,yrot,zrot
	float xscal,yscal,zscal
	uint64_t userdata
	char b16index
	char b8index
	short bindex
}
uint32_t grass_number
data*{
	unsigned short type
	float x,y,z
}

VERSION 6
unsigned short version
data*{
	unsigned char terrain_data
}
data*{
	unsigned char water_data
}
unsigned char terrain
unsigned char biome
uint32_t foliage_number
data*{
	unsigned short foliage_index
	float x, y, z
	string userdata
}
uint32_t component_number
data*{
	unsigned short component_index
	float x,y,z
	float xrot,yrot,zrot
	float xscal,yscal,zscal
	uint64_t userdata
	char b16index
	char b8index
	short bindex
}
uint32_t grass_number
data*{
	unsigned short type
	float x,y,z
}

*/
void CVoxelTerrain::WriteBoobToFile ( CBoob * pBoob, CBinaryFile * pFile )
{
	//==Boob and Terrain Section==
	pFile->WriteUShort( pBoob->version );
	for ( unsigned char i = 0; i < 8; i += 1 )
	{
		for ( unsigned char j = 0; j < 8; j += 1 )
		{
			for ( unsigned short k = 0; k < 512; k += 1 )
			{
				pFile->WriteUChar( pBoob->data[i].data[j].data[k] );
			}
			for ( unsigned short k = 0; k < 512; k += 1 )
			{
				pFile->WriteUChar( pBoob->data[i].data[j].water[k] );
			}
		}
	}
	pFile->WriteUChar( pBoob->terrain );
	pFile->WriteUChar( pBoob->biome );

	//==Foliage Section==
	pFile->WriteUInt32( (uint32_t)pBoob->v_foliage.size() );
	for ( unsigned int i = 0; i < pBoob->v_foliage.size(); i++ )
	{
		pFile->WriteUShort( GetFoliageType( pBoob->v_foliage[i] ) );
		pFile->WriteFloat( pBoob->v_foliage[i]->transform.position.x );
		pFile->WriteFloat( pBoob->v_foliage[i]->transform.position.y );
		pFile->WriteFloat( pBoob->v_foliage[i]->transform.position.z );
		//pFile->WriteUInt64( uint64_t(0) );
		pFile->WriteString( ((CTreeBase*)(pBoob->v_foliage[i]))->GetToggle() );
	}

	//==Component Section==
	pFile->WriteUInt32( (uint32_t)pBoob->v_component.size() );
	for ( unsigned int i = 0; i < pBoob->v_component.size(); i++ )
	{
		pFile->WriteUShort( GetComponentType( pBoob->v_component[i] ) );
		pFile->WriteFloat( pBoob->v_component[i]->transform.position.x );
		pFile->WriteFloat( pBoob->v_component[i]->transform.position.y );
		pFile->WriteFloat( pBoob->v_component[i]->transform.position.z );
		pFile->WriteFloat( pBoob->v_component[i]->transform.rotation.getEulerAngles().x );
		pFile->WriteFloat( pBoob->v_component[i]->transform.rotation.getEulerAngles().y );
		pFile->WriteFloat( pBoob->v_component[i]->transform.rotation.getEulerAngles().z );
		pFile->WriteFloat( pBoob->v_component[i]->transform.scale.x );
		pFile->WriteFloat( pBoob->v_component[i]->transform.scale.y );
		pFile->WriteFloat( pBoob->v_component[i]->transform.scale.z );
		pFile->WriteUInt64( uint64_t(0) );
		BlockInfo tempBlockInfo = pBoob->v_component[i]->GetBlockInfo();
		pFile->WriteChar( tempBlockInfo.b16index );
		pFile->WriteChar( tempBlockInfo.b8index  );
		pFile->WriteShort( tempBlockInfo.b1index  );
	}

	//==Grass Section==
	pFile->WriteUInt32( (uint32_t)pBoob->v_grass.size() );
	for ( unsigned int i = 0; i < pBoob->v_grass.size(); i++ )
	{
		pFile->WriteUShort( pBoob->v_grass[i].type );
		pFile->WriteFloat( pBoob->v_grass[i].position.x );
		pFile->WriteFloat( pBoob->v_grass[i].position.y );
		pFile->WriteFloat( pBoob->v_grass[i].position.z );
	}
}
void CVoxelTerrain::WriteMetaboobToFile ( CMetaboob * pBoob, CBinaryFile * pFile )
{
	//==Boob and Terrain Section==
	pFile->WriteUShort( pBoob->version );
	for ( unsigned char i = 0; i < 8; i += 1 )
	{
		for ( unsigned char j = 0; j < 8; j += 1 )
		{
			for ( unsigned short k = 0; k < 512; k += 1 )
			{
				pFile->WriteUChar( pBoob->data[i].data[j].data[k] );
			}
			for ( unsigned short k = 0; k < 512; k += 1 )
			{
				pFile->WriteUChar( pBoob->data[i].data[j].water[k] );
			}
		}
	}
	pFile->WriteUChar( pBoob->terrain );
	pFile->WriteUChar( pBoob->biome );

	//==Foliage Section==
	pFile->WriteUInt32( (uint32_t)pBoob->v_foliageIO.size() );
	for ( unsigned int i = 0; i < pBoob->v_foliageIO.size(); i++ )
	{
		pFile->WriteUShort( pBoob->v_foliageIO[i].foliage_index );
		pFile->WriteFloat( pBoob->v_foliageIO[i].position.x );
		pFile->WriteFloat( pBoob->v_foliageIO[i].position.y );
		pFile->WriteFloat( pBoob->v_foliageIO[i].position.z );
		//pFile->WriteUInt64( pBoob->v_foliageIO[i].bitmask );
		pFile->WriteString( pBoob->v_foliageIO[i].userdata );
	}

	//==Component Section==
	pFile->WriteUInt32( (uint32_t)pBoob->v_componentIO.size() );
	for ( unsigned int i = 0; i < pBoob->v_componentIO.size(); i++ )
	{
		pFile->WriteUShort( pBoob->v_componentIO[i].component_index );
		pFile->WriteFloat( pBoob->v_componentIO[i].position.x );
		pFile->WriteFloat( pBoob->v_componentIO[i].position.y );
		pFile->WriteFloat( pBoob->v_componentIO[i].position.z );
		pFile->WriteFloat( pBoob->v_componentIO[i].rotation.x );
		pFile->WriteFloat( pBoob->v_componentIO[i].rotation.y );
		pFile->WriteFloat( pBoob->v_componentIO[i].rotation.z );
		pFile->WriteFloat( pBoob->v_componentIO[i].scaling.x );
		pFile->WriteFloat( pBoob->v_componentIO[i].scaling.y );
		pFile->WriteFloat( pBoob->v_componentIO[i].scaling.z );
		pFile->WriteUInt64( pBoob->v_componentIO[i].data );
		pFile->WriteChar( pBoob->v_componentIO[i].b16index );
		pFile->WriteChar( pBoob->v_componentIO[i].b8index  );
		pFile->WriteShort( pBoob->v_componentIO[i].bindex  );
	}

	//==Grass Section==
	pFile->WriteUInt32( (uint32_t)pBoob->v_grass.size() );
	for ( unsigned int i = 0; i < pBoob->v_grass.size(); i++ )
	{
		pFile->WriteUShort( pBoob->v_grass[i].type );
		pFile->WriteFloat( pBoob->v_grass[i].position.x );
		pFile->WriteFloat( pBoob->v_grass[i].position.y );
		pFile->WriteFloat( pBoob->v_grass[i].position.z );
	}
}

void CVoxelTerrain::ReadBoobFromFile ( CBoob * pBoob, CBinaryFile * pFile )
{
	//==Boob and Terrain Section==
	pBoob->version = (unsigned char) pFile->ReadUShort();
	for ( unsigned char i = 0; i < 8; i += 1 )
	{
		for ( unsigned char j = 0; j < 8; j += 1 )
		{
			pFile->ReadUCharArray( pBoob->data[i].data[j].data, 512 );
			pFile->ReadUCharArray( pBoob->data[i].data[j].water, 512 );
		}
	}
	pBoob->terrain = pFile->ReadUChar();
	pBoob->biome = pFile->ReadUChar();

	pBoob->bitinfo[3] = true; // It's been generated, bitches!

	pBoob->hasBlockData = true; // And it now has block data!

	//==Foliage Section==
	if ( pBoob->current_resolution == 1 )
	{
		uint32_t foliageNum = pFile->ReadUInt32();
		for ( unsigned int i = 0; i < foliageNum; i++ )
		{
			unsigned short foliageType = pFile->ReadUShort();
			float x = pFile->ReadFloat();
			float y = pFile->ReadFloat();
			float z = pFile->ReadFloat();
			//uint64_t inData = pFile->ReadUInt64();
			string inData = pFile->ReadString(0);

			Vector3d foliagePos = Vector3d( x,y,z );
			//pBoob->v_foliage.push_back( CreateFoliage( foliageType, foliagePos, inData ) );

			TerraFoiliage	newFoliageRequest;
			newFoliageRequest.foliage_index	= foliageType;
			newFoliageRequest.position	= foliagePos;
			newFoliageRequest.userdata	= inData;
			newFoliageRequest.myBoob	= pBoob;
			vFoliageQueue.push_back( newFoliageRequest );
		}

		//==Component Section==
		uint32_t componentNum = pFile->ReadUInt32();
		for ( unsigned int i = 0; i < componentNum; i++ )
		{
			unsigned short componentType = pFile->ReadUShort();
			float x = pFile->ReadFloat();
			float y = pFile->ReadFloat();
			float z = pFile->ReadFloat();
			float xR = pFile->ReadFloat();
			float yR = pFile->ReadFloat();
			float zR = pFile->ReadFloat();
			float xS = pFile->ReadFloat();
			float yS = pFile->ReadFloat();
			float zS = pFile->ReadFloat();
			uint64_t inData = pFile->ReadUInt64();
			char  i16 = pFile->ReadChar();
			char  i8 = pFile->ReadChar();
			short i1 = pFile->ReadShort();

			Vector3d componentPos = Vector3d( x,y,z );			// TODO: this creation NEEDS to go into a queue.
			Vector3d componentRot = Vector3d( xR,yR,zR );
			Vector3d componentScl = Vector3d( xS,yS,zS );
			CreateComponent( componentType, componentPos, componentRot, componentScl, inData, pBoob, i16, i8, i1 );
		}

		//==Grass Section==
		uint32_t grassNum = pFile->ReadUInt32();
		for ( unsigned int i = 0; i < grassNum; i++ )
		{
			unsigned short grassType = pFile->ReadUShort();
			float x = pFile->ReadFloat();
			float y = pFile->ReadFloat();
			float z = pFile->ReadFloat();

			sTerraGrass newGrass;
			newGrass.type = grassType;
			newGrass.position.x = x;
			newGrass.position.y = y;
			newGrass.position.z = z;

			pBoob->v_grass.push_back( newGrass );
		}

		// Boob now has data
		pBoob->hasData = true;
	}
}

void CVoxelTerrain::GetBoobData ( CBoob * pBoob, LongIntPosition const& position, bool generateTerrain )
{
	if ( pBoob->hasData )
	{
		cout << "Warning: Attempting to get data on a boob that already has data." << endl;
		//return;
	}
	CBinaryFile file;
	stringstream tempStream;
	string sFilename;
	tempStream << CGameSettings::Active()->GetTerrainSaveDir() << "\\";
	tempStream << position.x << "_" << position.y << "_" << position.z;
	sFilename = tempStream.str();

	if ( file.Exists( sFilename.c_str() ) )	
	{
		if ( file.GetFilesize() < 64000 )	// Check for proper size
		{
			cout << " Area " << sFilename << " is corrupted! Regenerating." << endl;
			if ( generateTerrain )
			{
				AddGenerationRequest( position );
			}
		}
		else
		{
			file.Open( sFilename.c_str(), CBinaryFile::IO_READ );
		}

		if ( file.IsOpen() )
		{
			// Lock the terrain while updating
			mutex::scoped_lock local_lock( mtGaurd );

			ReadBoobFromFile( pBoob, &file );
			file.Close();
		}
	}
	else if ( generateTerrain ) //for some reason doesn't run correctly if hasData is true.
	{
		// Lock the terrain while updating
		//mutex::scoped_lock local_lock( mtGaurd );

		//GenerateSingleTerrain( pBoob, position );
		AddGenerationRequest( position );
	}
	// Set position
	pBoob->position = Vector3d( ftype(position.x*64+32),ftype(position.y*64+32),ftype(position.z*64+32) );
	pBoob->li_position = position;
}
void CVoxelTerrain::SaveBoobData ( CBoob * pBoob, LongIntPosition const& position )
{
	stringstream tempStream;
	string sFilename;
	tempStream << CGameSettings::Active()->GetTerrainSaveDir() << "\\";
	tempStream << position.x << "_" << position.y << "_" << position.z;
	sFilename = tempStream.str();

	CBinaryFile file;
	if ( file.Open( sFilename.c_str(), CBinaryFile::IO_WRITE ) )
	{
		WriteBoobToFile( pBoob, &file );
		file.Close();
	}
	else
	{
		cout << "error saving...likely lack of permissions or bad lock?" << endl;
	}
}

void CVoxelTerrain::SaveMetaboobData ( CMetaboob * pBoob, LongIntPosition const& position )
{
	stringstream tempStream;
	string sFilename;
	tempStream << CGameSettings::Active()->GetTerrainSaveDir() << "\\";
	tempStream << position.x << "_" << position.y << "_" << position.z;
	sFilename = tempStream.str();

	CBinaryFile file;
	if ( file.Exists( sFilename.c_str() ) )
	{
		cout << "Attempting to regen existing area ("
			<< position.x << ","
			<< position.y << ","
			<< position.z << ")....ignoring." << endl;
	}
	else if ( file.Open( sFilename.c_str(), CBinaryFile::IO_WRITE ) )
	{
		WriteMetaboobToFile( pBoob, &file );
		file.Close();
	}
	else
	{
		cout << "error saving...likely lack of permissions or bad lock?" << endl;
	}
}

void CVoxelTerrain::SaveActiveTerrain ( CBoob * pBoob, LongIntPosition const& position )
{
	// Recursively check the outliers first, then work way in, because it likely will be unneeded on the inlyers.
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[7] == false )
		{
			pBoob->bitinfo[7] = true;

			// Move to the edges FIRST.
			if ( position.x >= root_position.x )
				SaveActiveTerrain( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
			if ( position.y >= root_position.y )
				SaveActiveTerrain( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
			if ( position.z >= root_position.z )
				SaveActiveTerrain( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
			if ( position.x <= root_position.x )
				SaveActiveTerrain( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
			if ( position.y <= root_position.y )
				SaveActiveTerrain( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );
			if ( position.z <= root_position.z )
				SaveActiveTerrain( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );

			//cout << "loading " << position.x << "_" << position.y << "_" << position.z << endl;
			//SaveBoobData( pBoob, position );
			PushSavingList( pBoob, position );
		}
	}
}
void CVoxelTerrain::LoadTerrain ( CBoob * pBoob, LongIntPosition const& position )
{
	// Recursively check the outliers first, then work way in, because it likely will be unneeded on the inlyers.
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[7] == false )
		{
			pBoob->bitinfo[7] = true;

			// Move to the edges FIRST.
			if ( position.x >= root_position.x )
				LoadTerrain( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
			if ( position.y >= root_position.y )
				LoadTerrain( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
			if ( position.z >= root_position.z )
				LoadTerrain( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
			if ( position.x <= root_position.x )
				LoadTerrain( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
			if ( position.y <= root_position.y )
				LoadTerrain( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );
			if ( position.z <= root_position.z )
				LoadTerrain( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );

			//cout << "saving " << position.x << "_" << position.y << "_" << position.z << endl;
			GetBoobData( pBoob, position, true );
		}
	}
}

bool CVoxelTerrain::TerraFileExists ( void )
{
	CBinaryFile file;
	stringstream tempStream;
	string sFilename;
	tempStream << CGameSettings::Active()->GetTerrainSaveDir() << ".info";
	sFilename = tempStream.str();
	if ( file.Exists( sFilename.c_str() ) )
	{
		return true;
	}
	else
	{
		file.Open( sFilename.c_str(), CBinaryFile::IO_WRITE );
		file.WriteUChar( 0 );
		file.Close();
		return false;
	}
}
