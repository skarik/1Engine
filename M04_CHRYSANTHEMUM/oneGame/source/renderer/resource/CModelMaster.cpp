
#include "CModelMaster.h"
#include "core/system/io/FileUtils.h"

#include "core-ext/animation/CAnimation.h"
#include "renderer/logic/model/morpher/CMorpher.h"

#include "renderer/object/mesh/system/glMesh.h"
#include "physical/physics/shapes/physMesh.h"

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
void CModelMaster::AddReference ( const string& filename, vector<glMesh*>& meshes, vector<sHitbox>& hitboxes )
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
void CModelMaster::AddReference ( const string& filename, CAnimation* animSet )
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
			/*for ( unsigned int i = 0; i < it->second.vMeshes.size(); i++ )
			{
				delete it->second.vMeshes[i];
			}*/
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
			/*for ( unsigned int i = 0; i < it->second.vMeshes.size(); i++ )
			{
				delete it->second.vMeshes[i];
			}*/
			delete (it->second.pAnimSet->GetMorpherSet());
			delete (it->second.pAnimSet);
			// Remove from map
			morphSetMap.erase( it );

			// currently memory issues with deleting mesh 
		}
	}
}

// Returns a pointer to the wanted model data, null if not found
const vector<glMesh*>* CModelMaster::GetReference ( const string& filename )
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



CAnimation*			CModelMaster::GetAnimationReference ( const string& filename )
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
}