
#include "CModelMaster.h"
#include "core/system/io/FileUtils.h"

#include "core-ext/animation/AnimationControl.h"
#include "renderer/logic/model/morpher/CMorpher.h"
#include "renderer/resource/CModelMaster.h"

#include "renderer/object/mesh/system/rrMesh.h"
//#include "physical/physics/shapes/physMesh.h"
#include "physical/physics/shapes/PrShape.h"

ARSINGLETON_CPP_DEF(RenderResources);

#define HANDLE_REFERENCE_FIND(LISTING) \
	for ( auto itr = LISTING.begin(); itr != LISTING.end(); ++itr ) \
		if ( itr->first.compare( filename ) )

#define HANDLE_REFERENCE_DECREMENT(LISTING) \
	HANDLE_REFERENCE_FIND(LISTING) { \
		if ( itr->second.refCount == 0 ) throw core::NullReferenceException(); \
		itr->second.refCount--; return; \
	} throw core::NullReferenceException();

void RenderResources::AddMeshSet ( const char* filename, std::vector<rrMesh*>& meshes )
{
	HANDLE_REFERENCE_FIND(meshSets) throw core::InvalidCallException();
	meshset_reference_t ref;
	ref.refCount = 0;
	ref.set = meshes;
	meshSets[arstring256(filename)] = ref;
}
void RenderResources::AddMorphSet ( const char* filename, CMorpher* morphSet )
{
	HANDLE_REFERENCE_FIND(morphSets) throw core::InvalidCallException();
	morphset_reference_t ref;
	ref.refCount = 0;
	ref.set = morphSet;
	morphSets[arstring256(filename)] = ref;
}

//	GetMesh ( filename )
// Returns the mesh set saved previously, and increments the reference count.
// Returns NULL if no reference is found.
const std::vector<rrMesh*>* RenderResources::GetMesh ( const char* filename )
{
	HANDLE_REFERENCE_FIND(meshSets)
	{
		itr->second.refCount++;
		return &itr->second.set;
	}
	return NULL;
}
//	GetMorpher ( filename )
// Returns the morpher saved previously, and increments the reference count.
// Returns NULL if no reference is found.
const CMorpher* RenderResources::GetMorpher ( const char* filename )
{
	HANDLE_REFERENCE_FIND(morphSets)
	{
		itr->second.refCount++;
		return itr->second.set;
	}
	return NULL;
}

void RenderResources::ReleaseMeshSet ( const char* filename )
{
	HANDLE_REFERENCE_DECREMENT(meshSets)
}

void RenderResources::ReleaseMorphSet ( const char* filename )
{
	HANDLE_REFERENCE_DECREMENT(morphSets)
}

//	Cleanup ()
// Deletes all mesh sets and morphs with no existing references
void RenderResources::Cleanup ( void )
{
	// Clean up the meshes
	for ( auto itr = meshSets.begin(); itr != meshSets.end(); )
	{
		if ( itr->second.refCount == 0 )
		{
			for ( size_t i = 0; i < itr->second.set.size(); ++i )
			{
				// Delete all the meshes in the list
				delete itr->second.set[i];
			}
			// Remove this entry from the list
			itr = meshSets.erase(itr);
		}
		else
		{
			++itr;
		}
	}

	// Clean up the morphs
	for ( auto itr = morphSets.begin(); itr != morphSets.end(); )
	{
		if ( itr->second.refCount == 0 )
		{
			// Delete the morph
			delete itr->second.set;
			// Remove this entry from the list
			itr = morphSets.erase(itr);
		}
		else
		{
			++itr;
		}
	}

	// Done cleaning
}

#undef HANDLE_REFERENCE_FIND
#undef HANDLE_REFERENCE_DECREMENT

/*
#include <iostream>

using namespace std;

// Instance Define
CModelMaster ModelMaster;

// Constructor
CModelMaster::CModelMaster ( void )
{

}

// Destructor
CModelMaster::~CModelMaster ( void )
{

}

// Add a reference to a mesh set
void CModelMaster::AddReference ( const string& filename, vector<rrMesh*>& meshes, vector<sHitbox>& hitboxes )
{
	// First look for it
	std::map<string,MeshSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = meshMap.find( tFilename );
	if ( it == meshMap.end() )
	{
		MeshSetReference newRef;
		newRef.referenceCount = 2;
		newRef.vMeshes = meshes;
		newRef.vHitboxes = hitboxes;

		meshMap[ tFilename ] = newRef;
	}
	else
	{
		it->second.referenceCount += 1;
	}
}
void CModelMaster::AddReference ( const string& filename, vector<physMesh*>& meshes )
{
	// First look for it
	std::map<string,PhysSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = physMeshMap.find( tFilename );
	if ( it == physMeshMap.end() )
	{
		PhysSetReference newRef;
		newRef.referenceCount = 2;
		newRef.vMeshes = meshes;

		physMeshMap[ tFilename ] = newRef;
	}
	else
	{
		it->second.referenceCount += 1;
	}
}
void CModelMaster::AddReference ( const string& filename, AnimationControl* animSet )
{
	// First look for it
	map<string,AnimSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = animSetMap.find( tFilename );
	if ( it == animSetMap.end() )
	{
		AnimSetReference newRef;
		newRef.referenceCount = 2;
		//newRef.vMeshes = meshes;
		newRef.pAnimSet = animSet;

		animSetMap[ tFilename ] = newRef;
	}
	else
	{
		it->second.referenceCount += 1;
	}
}
void CModelMaster::AddReference ( const string& filename, CMorpher* animSet )
{
	// First look for it
	map<string,MorphSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = morphSetMap.find( tFilename );
	if ( it == morphSetMap.end() )
	{
		MorphSetReference newRef;
		newRef.referenceCount = 2;
		//newRef.vMeshes = meshes;
		newRef.pAnimSet = animSet;

		morphSetMap[ tFilename ] = newRef;
	}
	else
	{
		it->second.referenceCount += 1;
	}
}

// Decrement the target reference
void CModelMaster::RemoveReference ( const string& filename )
{
	// First look for it
	map<string,MeshSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = meshMap.find( tFilename );
	if ( it == meshMap.end() )
	{
		std::cout << "Attempting to free a non-existant model reference: " << tFilename << std::endl;
	}
	else
	{
		it->second.referenceCount -= 1; //not the problem
		
		// If there's no more references, the delete the model
		if ( it->second.referenceCount == 0 )
		{
			// Delete all the meshes
			for ( unsigned int i = 0; i < it->second.vMeshes.size(); i++ )
			{
				delete it->second.vMeshes[i];
			}
			// Remove from map
			meshMap.erase( it );

			// currently memory issues with deleting mesh
		}
	}
}
void CModelMaster::RemovePhysReference ( const string& filename )
{
	// First look for it
	map<string,PhysSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = physMeshMap.find( tFilename );
	if ( it == physMeshMap.end() )
	{
		cout << "Attempting to free a non-existant model reference: " << tFilename << endl;
	}
	else
	{
		it->second.referenceCount -= 1; //not the problem
		
		// If there's no more references, the delete the model
		if ( it->second.referenceCount == 0 )
		{
			// Delete all the meshes
			for ( unsigned int i = 0; i < it->second.vMeshes.size(); i++ )
			{
				delete it->second.vMeshes[i];
			}
			// Remove from map
			physMeshMap.erase( it );

			// currently memory issues with deleting mesh
		}
	}
}
void CModelMaster::RemoveAnimSetReference ( const string& filename )
{
	// First look for it
	map<string,AnimSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = animSetMap.find( tFilename );
	if ( it == animSetMap.end() )
	{
		cout << "Attempting to free a non-existant animation reference: " << tFilename << endl;
		cout << "  Note: This can happen normally while animations are freed." << endl;
	}
	else
	{
		it->second.referenceCount -= 1; //not the problem
		
		// If there's no more references, the delete the model
		if ( it->second.referenceCount == 0 )
		{
			// Delete all the meshes
			delete (it->second.pAnimSet->GetAnimationSet());
			delete (it->second.pAnimSet);
			// Remove from map
			animSetMap.erase( it );

			// currently memory issues with deleting mesh 
		}
	}
}
void CModelMaster::RemoveMorpherSetReference ( const string& filename )
{
	// First look for it
	map<string,MorphSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = morphSetMap.find( tFilename );
	if ( it == morphSetMap.end() )
	{
		cout << "Attempting to free a non-existant morph reference: " << tFilename << endl;
		cout << "  Note: This can happen normally while animations are freed." << endl;
	}
	else
	{
		it->second.referenceCount -= 1; //not the problem
		
		// If there's no more references, the delete the morph set
		if ( it->second.referenceCount == 0 )
		{
			// Delete all the meshes
			delete (it->second.pAnimSet->GetMorpherSet());
			delete (it->second.pAnimSet);
			// Remove from map
			morphSetMap.erase( it );

			// currently memory issues with deleting mesh 
		}
	}
}

// Returns a pointer to the wanted model data, null if not found
const vector<rrMesh*>* CModelMaster::GetReference ( const string& filename )
{
	// First look for it
	map<string,MeshSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = meshMap.find( tFilename );
	if ( it == meshMap.end() )
	{
		return NULL;
	}
	else
	{
		return &(it->second.vMeshes);
	}
}

const vector<sHitbox>* CModelMaster::GetHitboxReference ( const string& filename )
{
	// First look for it
	map<string,MeshSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = meshMap.find( tFilename );
	if ( it == meshMap.end() ) {
		return NULL;
	}
	else {
		return &(it->second.vHitboxes);
	}
}



const vector<physMesh*>*	CModelMaster::GetCollisionReference ( const string& filename )
{
	// First look for it
	map<string,PhysSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = physMeshMap.find( tFilename );
	if ( it == physMeshMap.end() )
	{
		return NULL;
	}
	else
	{
		return &(it->second.vMeshes);
	}
}



AnimationControl*			CModelMaster::GetAnimationReference ( const string& filename )
{
	// First look for it
	map<string,AnimSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = animSetMap.find( tFilename );
	if ( it == animSetMap.end() )
	{
		return NULL;
	}
	else
	{
		return (it->second.pAnimSet);
	}
}


CMorpher*			CModelMaster::GetMorpherReference ( const string& filename )
{
	// First look for it
	map<string,MorphSetReference>::iterator it;

	string tFilename = IO::FilenameStandardize( filename );
	it = morphSetMap.find( tFilename );
	if ( it == morphSetMap.end() )
	{
		return NULL;
	}
	else
	{
		return (it->second.pAnimSet);
	}
}*/