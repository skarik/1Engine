
#include "CParticleSystem.h"

#include "core/system/io/CBinaryFile.h"
#include "core/system/io/CSegmentedFile.h"
#include "core-ext/system/io/serializer/ISerialBinary.h"
#include "core-ext/system/io/Resources.h"

#include "renderer/material/glMaterial.h"

#include "renderer/logic/particle/CParticleEmitter.h"
#include "renderer/logic/particle/CParticleUpdater.h"
#include "renderer/object/particle/CParticleRenderer.h"

#include "renderer/logic/particle/CParticleEmitterSkeleton.h"

#include "renderer/logic/particle/modifiers/CParticleMod_Windmotion.h"
#include "renderer/object/particle/CParticleRenderer_Animated.h"

#include <sstream>

using namespace std;

CParticleSystem::CParticleSystem ( const string& s_ps, const bool b_hasMeshOverride )
	: CExtendableGameObject(), bAutoDestroy( true )
{
	this->name = "Particle System";
	enabled = true;
	Init( s_ps, b_hasMeshOverride );
}
CParticleSystem::CParticleSystem ( const string& s_ps, const string& s_mat )
	: CExtendableGameObject(), bAutoDestroy( true )
{
	this->name = "Particle System";
	enabled = true;
	Init( s_ps, false );

	if ( s_mat.size() > 0 )
	{
		glMaterial* mat = new glMaterial();
		mat->loadFromFile( s_mat.c_str() );
		mat->removeReference();
		CRenderableObject* targetRenderable;
		targetRenderable = GetRenderable();
		if ( targetRenderable ) {
			targetRenderable->SetMaterial( mat );
		}
	}
}

CParticleSystem::~CParticleSystem ( void )
{
	/*for ( vector<CParticleEmitter*>::iterator it = vpEmitters.begin(); it != vpEmitters.end(); it++ )
	{
		//delete (*it); // Why was this commented out?
		// commented out b/c of memory error
		(*it)->RemoveReference();
		if ( !((*it)->HasReference()) )
		{
			delete (*it);
		}
	}*/
	vpEmitters.clear();
}

void CParticleSystem::Init ( const string& sSystemFile, const bool bHasMeshOverride )
{
	string sActualSystemFile = Core::Resources::PathTo( sSystemFile );
	uint32_t iVersion = 0;
	{
		// Input
		CSegmentedFile inFile ( sActualSystemFile );
		inFile.ReadData();

		// Check header for version number
		{
			// Get data
			string str;
			inFile.GetSectionData( string("header"), str );
			stringstream sin ( str, ios_base::binary|ios_base::in|ios_base::out );

			// Read version
			sin.read( (char*)(&iVersion), sizeof(uint32_t) );
		}
	}

	switch ( iVersion )
	{
	case 1:
		// Create a version 1 particle system attached to this object
		{
			CParticleEmitter* emitter;
			if ( !bHasMeshOverride ) { 
				emitter = new CParticleEmitter();
			}
			else {
				emitter = new CParticleEmitterSkeleton();
			}
			emitter->LoadFromFile( sActualSystemFile );
			CParticleUpdater* updater = new CParticleUpdater( emitter );
			CParticleRenderer* renderer = new CParticleRenderer( emitter );

			AddComponent( emitter );
			AddComponent( updater );
			AddComponent( renderer );

			vpEmitters.push_back( emitter );
		}
		break;
	case 2:
	case 3:
		// Create a version 2 particle system attached to this object
		{
			CBinaryFile inFile;
			inFile.Open( sActualSystemFile.c_str(), CBinaryFile::IO_READ );
			// First read in past the ascii header
			{
				string gaurdSearch = "";
				bool endGaurd = false;
				while ( !endGaurd ) {
					gaurdSearch += inFile.ReadChar();
					if ( string("-end>>").find( gaurdSearch ) == string::npos ) {
						gaurdSearch = "";
					}
					else if ( gaurdSearch == "-end>>" ) {
						endGaurd = true;
					}
				}
			}
			// Now that the header is read in, read in component type, and run serializer for each class
			{
				ISerialBinary deserializer ( &inFile, iVersion );

				void* newComponent = NULL;
				CParticleEmitter* lastEmitter = NULL;
				CParticleUpdater* lastUpdater = NULL;

				uint32_t currentObjType = inFile.ReadUInt32();
				while ( !feof( inFile.GetFILE() ) )
				{
					switch ( currentObjType )
					{
					case 0:	// Default emitter
						if ( !bHasMeshOverride ) { 
							newComponent = new CParticleEmitter();
						}
						else {
							newComponent = new CParticleEmitterSkeleton();
						}
						deserializer >> ((CParticleEmitter*)(newComponent));
						AddComponent( (CParticleEmitter*)(newComponent) );
						lastEmitter = (CParticleEmitter*)(newComponent);
						vpEmitters.push_back( lastEmitter );
						break;
					case 1: // Default updater
						newComponent = new CParticleUpdater(lastEmitter);
						deserializer >> ((CParticleUpdater*)(newComponent));
						AddComponent( (CParticleUpdater*)(newComponent) );
						break;
					case 2: // Default renderer
						newComponent = new CParticleRenderer(lastEmitter);
						((CParticleRenderer*)(newComponent))->SetMaterial( new glMaterial );
						deserializer >> ((CParticleRenderer*)(newComponent));
						AddComponent( (CParticleRenderer*)(newComponent) );
						break;
					case 3: // Emitter - Clouds; Skip this
						break;
					case 5: // Renderer - Animation; not yet implemented
						newComponent = new CParticleRenderer_Animated(lastEmitter);
						((CParticleRenderer_Animated*)(newComponent))->SetMaterial( new glMaterial );
						deserializer >> ((CParticleRenderer_Animated*)(newComponent));
						AddComponent( (CParticleRenderer_Animated*)(newComponent) );
						break;
					case 4: // Modifier - Wind
						newComponent = new CParticleMod_Windmotion();
						deserializer >> ((CParticleMod_Windmotion*)(newComponent));
						lastUpdater->AddModifier( ((CParticleMod_Windmotion*)(newComponent)) );
						break;
					default:
						Debug::Console->PrintError( "particle system: unrecognized component type!" );
						break;
					}
					currentObjType = inFile.ReadUInt32();
				}
			}

		}
		break;
	default:
		Debug::Console->PrintError( "could not read " );
		Debug::Console->PrintError( sSystemFile.c_str() );
		Debug::Console->PrintError( ": unsupported file version or invalid\n" );
		break;
	}
}

void CParticleSystem::PostUpdate ( void )
{
	// Loop through all the components. If they've no owner, remove them.
	/*for ( vector<CGameBehavior*>::iterator it = vpComponents.begin(); it != vpComponents.end(); it++ )
	{
		(*it)->transform.get( this->transform );
	}*/
	for ( vector<CParticleEmitter*>::iterator it = vpEmitters.begin(); it != vpEmitters.end(); it++ )
	{
		(*it)->transform.Get( this->transform );
		//(*it)->active = enabled;
		(*it)->SetActive( enabled );
	}
	for ( vector<CRenderableObject*>::iterator it = vpRComponents.begin(); it != vpRComponents.end(); it++ )
	{
		(*it)->transform.Get( this->transform );
	}

	if ( bAutoDestroy )
	{
		// Loop through emitters
		bool bCanDestroy = true;
		for ( vector<CParticleEmitter*>::iterator it = vpEmitters.begin(); it != vpEmitters.end(); it++ )
		{
			// If the emitter is not one-shot or has particles out, then destroy self automatically
			CParticleEmitter* emitter = *it;
			
			if ( ( !emitter->bOneShot )||( emitter->GetParticleCount() > 0 )||( !emitter->HasEmitted() ) ) {
				bCanDestroy = false;
				break;
			}
		}
		// Send delete message if should be deleting
		if ( bCanDestroy ) {
			//cout << "DESTROYED :D" << endl;
			DeleteObject( this );
		}
	}
}