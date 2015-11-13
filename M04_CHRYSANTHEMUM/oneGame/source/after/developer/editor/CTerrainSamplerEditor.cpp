#include "CTerrainSamplerEditor.h"

////#include "unused/CVoxelTerrain.h"

#include "core/input/CInput.h"
#include "core/settings/CGameSettings.h"
#include "core-ext/system/io/Resources.h"

#include "engine-common/entities/CPlayer.h"

#include "after/types/terrain/TerrainTypes.h"
#include "after/types/terrain/BiomeTypes.h"

#include "renderer/object/CRenderableObject.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

// C++ STD Includes
#include <iostream>
#include <fstream>
#include <string>

// Namespaces
using namespace std;

CTerrainSamplerEditor::CTerrainSamplerEditor ( void )
	: CGameBehavior(), CRenderableObject()
{
	// Set target file
	sTargetFile = Core::Resources::PathTo( "terra/biomeSampler.pgm" );
	eCurrentEditor = EM_TerrainType;

	// Create sampler
	pSampler = new unsigned char [64*64];
	for ( unsigned short i = 0; i < 64*64; i++ )
		pSampler[i] = Terrain::TER_DEFAULT;
	if ( eCurrentEditor == EM_TerrainType )
		GenerateDefault_Terrain();
	else
		GenerateDefault_Terrain();
	LoadFromPPM();


	// Create render options
	myMaterial = new glMaterial;
	myMaterial->m_diffuse = Color( 1.0f,1,1 );
	myMaterial->passinfo.push_back( glPass() );
	myMaterial->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	myMaterial->setTexture( 0, new CTexture( "textures/white.jpg" ) );
	myMaterial->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
	SetMaterial( myMaterial );

	// Set to top layer drawing
	renderType = Renderer::V2D;
}
CTerrainSamplerEditor::~CTerrainSamplerEditor ( void )
{
	SaveToPPM();
	delete [] pSampler;
	delete myMaterial;
}

void CTerrainSamplerEditor::GenerateDefault_Terrain ( void )
{
	unsigned char newValue;
	float fx, fy;
	for ( unsigned short y = 0; y < 64; y++ )
	{
		for ( unsigned short x = 0; x < 64; x++ )
		{
			newValue = Terrain::TER_DEFAULT;
			fx = x/64.0f;
			fy = y/64.0f;

			// General terrain types
			if ( fy < 0.3f )
				newValue = Terrain::TER_OCEAN;
			else if ( fy < 0.6f )
				newValue = Terrain::TER_FLATLANDS;
			else if ( fy < 0.85f )
				newValue = Terrain::TER_HILLLANDS;
			else
				newValue = Terrain::TER_MOUNTAINS;
			// Now for islands in the flatlands
			if (( fy > 0.3f )&&( fy < 0.6f )&&( fx < 0.75f ))
			{
				if ( fy < 0.3f+sqrt((fx/0.75f))*0.15f )
					newValue = Terrain::TER_ISLANDS;
			}
			// Now for mountains in the hillands
			if (( fy > 0.6f )&&( fx <= 0.75f ))
			{
				if ( fy > 0.85f-sqr((fx/0.75f))*0.1f )
					newValue = Terrain::TER_MOUNTAINS;
			}
			// Now for the spires in the ocean
			if (( fy < 0.3f )&&( fx < 0.7f )&&( fy > 0.2f )&&( fx > 0.35f ))
				newValue = Terrain::TER_SPIRES;
			// Now for the spike of ocean in the islands
			if (( fy < 0.45f )&&( fx > 0.6f ))
			{
				if ( fy < 0.3f+(fx-0.6f) )
					newValue = Terrain::TER_OCEAN;
			}
			// Now for the desert
			if (( fy < 0.45f )&&( fx >= 0.749f ))
				newValue = Terrain::TER_DESERT;
			// Now for the edgelands
			if (( fy < 0.45f )&&( fx > 0.93f ))
				newValue = Terrain::TER_THE_EDGE;
			// Now for the badlands in the flatlands
			if (( fy >= 0.45f )&&( fy <= 0.6f )&&( fx >= 0.65f ))
			{
				if ( fy >= 0.6f-((fx-0.65f)/0.25f)*0.15f )
					newValue = Terrain::TER_BADLANDS;
			}
			// Now for the badlands in the hilllands
			if (( fy >= 0.6f )&&( fy <= 0.75f )&&( fx >= 0.65f ))
			{
				if ( fy <= 0.6f+((fx-0.65f)/0.15f)*0.15f )
					newValue = Terrain::TER_BADLANDS;
			}
			// And last the outlands
			if (( fy > 0.75f )&&( fx > 0.75f ))
				newValue = Terrain::TER_OUTLANDS;


			pSampler[x+y*64] = newValue;
		}
	}
}



void CTerrainSamplerEditor::GenerateDefault_Biome ( void )
{
	unsigned char newValue;
	float fx, fy;
	for ( unsigned short y = 0; y < 64; y++ )
	{
		for ( unsigned short x = 0; x < 64; x++ )
		{
			newValue = Terrain::TER_DEFAULT;
			fx = x/64.0f;
			fy = y/64.0f;

			// General biome types
			if ( fy < 0.3f )
				newValue = Terrain::BIO_TAIGA;
			else if ( fy < 0.5f )
				newValue = Terrain::BIO_EVERGREEN;
			else
				newValue = Terrain::BIO_TEMPERATE;

			// Wet biome types
			if ( fy > 0.6f )
			{
				if ( fx > 0.7f )
					newValue = Terrain::BIO_RAINFOREST;
				else if ( fx > 0.5f )
					if ( fy > 0.7 )
						newValue = Terrain::BIO_SWAMP;
			}
			// Super tundra type
			if ( fy <= 0.1f-fx*0.15f )
			{
				newValue = Terrain::BIO_S_TUNDRA;
			}
			// Evergreen in the taiga
			if ( fy < 0.5f )
			{
				if ( fy > 0.3f-fx*0.2f )
					newValue = Terrain::BIO_EVERGREEN;
			}


			pSampler[x+y*64] = newValue;
		}
	}
}

void CTerrainSamplerEditor::SaveToPPM ( void )
{
	string sOutputFile ( sTargetFile );
	CGameSettings::Active()->MakeDirectory( Core::Resources::GetPrimaryResourcePath() + "system" );

	ofstream fileOutput ( sOutputFile.c_str(), iostream::binary );
	fileOutput << "P5" << endl;
	fileOutput << 64 << " " << 64 << endl;
	fileOutput << "255" << endl;

	unsigned char temp;
	for ( unsigned int i = 0; i < 64*64; i += 1 )
	{
		temp = unsigned char( pSampler[i] );
		fileOutput << temp;
	}

	fileOutput.close();
}
void CTerrainSamplerEditor::LoadFromPPM ( void )
{
	string sInputFile ( sTargetFile );

	ifstream fileInput ( sInputFile.c_str(), iostream::binary );
	if ( fileInput.is_open() )
	{
		string sTemp;
		int iTemp;
		fileInput >> sTemp;
		fileInput >> iTemp;
		fileInput >> iTemp;
		fileInput >> iTemp;
		fileInput.get();

		fileInput.read( (char*)pSampler, 64*64 );

		fileInput.close();
	}
}

void CTerrainSamplerEditor::Update ( void )
{
	//CPlayer::pActive

	if ( CInput::MouseDown( CInput::MBLeft ) )
	{
		//if ( CInput::mouseX >= 100 )&&( CInput::mouseY
		int indexX = (int) (CInput::MouseX()-100)/8;
		//int indexY = (CInput::mouseY-100)/8;
		int indexY = (int) (100+64*8+8-CInput::MouseY())/8;
		if (( indexX >= 0 )&&( indexX < 64 )&&( indexY >= 0 )&&( indexY < 64 ))
		{
			pSampler[indexX+indexY*64] += 1;
			if ( pSampler[indexX+indexY*64] > Terrain::TER_THE_EDGE )
				pSampler[indexX+indexY*64] = Terrain::TER_DEFAULT;
		}
	}
}
bool CTerrainSamplerEditor::Render ( const char pass )
{
	GL_ACCESS;
	GLd_ACCESS;
	GL.beginOrtho();
	GLd.DrawSet2DMode( GLd.D2D_FLAT );

	for ( unsigned short y = 0; y < 64; y++ )
	{
		for ( unsigned short x = 0; x < 64; x++ )
		{
			// boom
			//glColor4f( 1,0,0,1 );
			if ( eCurrentEditor == EM_TerrainType )
			{
				switch ( pSampler[x+y*64] )
				{
				case Terrain::TER_BADLANDS:
					myMaterial->m_diffuse = Color( 1,0.8f,0.4f,1 );
					break;
				case Terrain::TER_DESERT:
					myMaterial->m_diffuse = Color( 1,0.9f,0.5f,1 );
					break;
				case Terrain::TER_OUTLANDS:
					myMaterial->m_diffuse = Color( 1,0.4f,0.6f,1 );
					break;
				case Terrain::TER_THE_EDGE:
					myMaterial->m_diffuse = Color( 0.5f,0.5f,0.5f,1 );
					break;
				case Terrain::TER_OCEAN:
					myMaterial->m_diffuse = Color( 0.35f,0.6f,0.9f,1 );
					break;
				case Terrain::TER_SPIRES:
					myMaterial->m_diffuse = Color( 0.6f,0.7f,1.0f,1 );
					break;
				case Terrain::TER_ISLANDS:
					myMaterial->m_diffuse = Color( 0.25f,0.95f,0.7f,1 );
					break;
				case Terrain::TER_FLATLANDS:
					myMaterial->m_diffuse = Color( 0.4f,0.9f,0.4f,1 );
					break;
				case Terrain::TER_HILLLANDS:
					myMaterial->m_diffuse = Color( 0.55f,0.85f,0.24f,1 );
					break;
				case Terrain::TER_MOUNTAINS:
					myMaterial->m_diffuse = Color( 0.5f,0.7f,0.4f,1 );
					break;
				case Terrain::TER_DEFAULT:
				default:
					myMaterial->m_diffuse = Color( 1,0,0,1 );
					break;
				}
			}
			else if ( eCurrentEditor == EM_TerrainType )
			{
				switch ( pSampler[x+y*64] )
				{
				case Terrain::BIO_S_TUNDRA:
					myMaterial->m_diffuse = Color( 0.6f,0.7f,1.0f,1 );
					break;
				case Terrain::BIO_TAIGA:
					myMaterial->m_diffuse = Color( 0.35f,0.6f,0.9f,1 );
					break;
				case Terrain::BIO_SWAMP:
					myMaterial->m_diffuse = Color( 0.4f,0.5f,0.4f,1 );
					break;
				case Terrain::BIO_RAINFOREST:
					myMaterial->m_diffuse = Color( 0.4f,0.9f,0.4f,1 );
					break;
				case Terrain::BIO_TEMPERATE:
					myMaterial->m_diffuse = Color( 0.55f,0.85f,0.24f,1 );
					break;
				case Terrain::BIO_EVERGREEN:
					myMaterial->m_diffuse = Color( 0.5f,0.7f,0.4f,1 );
					break;
				case Terrain::BIO_DEFAULT:
				default:
					myMaterial->m_diffuse = Color( 1,0,0,1 );
					break;
				}
			}
			
			myMaterial->bindPass(0);
			GLd.DrawRectangle( 100+x*8,100+(8*64)-y*8,8,8 );
		}
	}

	//myMaterial->unbind();

	GL.endOrtho();
	// Return success
	return true;
}