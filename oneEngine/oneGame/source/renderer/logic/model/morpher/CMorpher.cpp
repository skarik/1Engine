#include "CMorpher.h"
#include "core/time/time.h"
#include "core/math/Math.h"
#include "core/debug/console.h"
#include "renderer/object/mesh/system/rrSkinnedMesh.h"
#include "gpuw/Device.h"
#include "gpuw/GraphicsContext.h"

CMorphAction CMorpher::m_DeadAction ( "" );

CMorpher::CMorpher ( string const& sInFilename, const  CMorpher* pFoundReference )
{
	pMorphSet = NULL;
	sFilename = sInFilename;
	//pAnimationSet = ModelMaster.GetAnimationReference( sFilename );
	// Grab reference
	//CMorpher* pFoundReference = ModelMaster.GetMorpherReference( sFilename );
	if ( pFoundReference != NULL )
	{
		// Copy the needed data
		(*this) = pFoundReference;
		// Is valid
		bIsValid = true;

		//ModelMaster.AddReference( sFilename, this );
	}
	else
	{
		//cout << "Warning: invalid animation reference...prepare for a clusterfuck." << endl;
		bIsValid = false;
	}
}

CMorpher::CMorpher ( string const& sInFilename, const CMorpherSet* pInSet )
{
	pMorphSet = pInSet;
	sFilename = sInFilename;
	//ModelMaster.AddReference( sFilename, pAnimationSet );
	//ModelMaster.AddReference( sFilename, this );
}

// Destructor
CMorpher::~CMorpher ( void )
{
	//if ( bIsValid )
		//ModelMaster.RemoveMorpherSetReference( sFilename );
}

// Add action
void CMorpher::AddAction ( CMorphAction newAction )
{
	mActions[newAction.GetName()] = newAction;
}

// Copy from existing reference (pointer)
CMorpher& CMorpher::operator= ( const CMorpher* pRight )
{
	return ((*this) = (*pRight));
}
// Copy from existing refernece
CMorpher&	CMorpher::operator= ( CMorpher const& sourceAnimRef )
{
	printf("Copying CMorpher instance.\n");

	// Copy the animation set
	pMorphSet = sourceAnimRef.pMorphSet;
	// Copy the action map
	mActions = sourceAnimRef.mActions;
	// Set all of the animation actions' owners to this, and reset them all
	for ( auto it = mActions.begin(); it != mActions.end(); it++ )
	{
		printf(" - copied action: \"%s\"\n", it->second.GetName().c_str());
	}
	// Set the first action to active
	//mAnimations.begin()->second.Play();
	// Copy the IK list
	//ikList = sourceAnimRef.ikList;

	printf(" + Done.\n");

	// Return this
	return (*this);
}

CMorphAction& CMorpher::operator [] ( const char* animName )
{
	// Find animation with name
	auto it = mActions.find( arstring<128>(animName) );
	if ( it == mActions.end() ) {
#ifdef _ENGINE_DEBUG
		//cout << "WARNING: Could not find animation named \"" << animName << "\" in list!" << endl;
		//throw std::out_of_range( "Bad animation" );
#endif
		//deadAction.isPlaying = false;
		m_DeadAction.weight = 0;
		//deadAction.frame = 0;
		return m_DeadAction;
	}

	// return reference to it
	return it->second;
}
CMorphAction& CMorpher::operator [] ( const int & animIndex )
{
	auto it = mActions.begin();
	/*for ( int i = 0; i < animIndex; ++i )
		++it;*/
	while ( it != mActions.end() && it->second.index != animIndex ) {
		++it;
	}

	if ( it == mActions.end() )
	{
#ifdef _ENGINE_DEBUG
		debug::Console->PrintWarning("WARNING: Could not find index numbered \"%d\" in list!\n", animIndex);
		//throw std::out_of_range( "Bad animation" );
#endif
		//deadAction.isPlaying = false;
		m_DeadAction.weight = 0;
		//deadAction.frame = 0;
		return m_DeadAction;
	}
	return it->second;
}
CMorphAction* CMorpher::FindAction ( const char* animName )
{
	// Find animation with name
	auto it = mActions.find( arstring<128>(animName) );
	if ( it == mActions.end() ) {
		//cout << "Unable to find action name '" << animName << "' in animation " << this << endl;
		return NULL;
	}
	else {
		return &(it->second);
	}
}

/*
// Perform the morphs. Only affect positions and normals of the mesh's stream.
void	CMorpher::PerformMorph ( rrMeshBuffer* sourceMesh, rrMeshBuffer* targetMesh )
{
	// First, grab the stream to work on
	arModelData* pStreamData = targetMesh->modeldata;
	memcpy( pStreamData->vertices, sourceMesh->modeldata->vertices, sizeof(arModelVertex) * sourceMesh->modeldata->vertexNum );

	// Loop through all the actions and perform auto blending
	auto it = mActions.begin();
	while ( it != mActions.end() )
	{
		if ( it->second.auto_blend ) {
			Real delta = it->second.target_weight - it->second.weight;
			Real changeSpeed = Time::deltaTime * 7.0f;
			if ( fabs( delta ) > changeSpeed ) {
				it->second.weight += math::sgn<Real>( delta ) * changeSpeed;
			}
			else {
				it->second.weight = it->second.target_weight;
			}
		}
		++it;
	}

	// Loop through all actions and modify the stream data with the offset*weight
	it = mActions.begin();
	while ( it != mActions.end() )
	{
		if ( fabs(it->second.weight) > 0.01f )
		{
			const arModelVertex* vertOffset = pMorphSet->GetMorphData(it->second.index);
			if ( vertOffset )
			{
				// Perform the morphing
				for ( uint i = 0; i < pStreamData->vertexNum; ++i )
				{
					pStreamData->vertices[i].x += vertOffset[i].x*it->second.weight;
					pStreamData->vertices[i].y += vertOffset[i].y*it->second.weight;
					pStreamData->vertices[i].z += vertOffset[i].z*it->second.weight;
					pStreamData->vertices[i].nx += vertOffset[i].nx*it->second.weight;
					pStreamData->vertices[i].ny += vertOffset[i].ny*it->second.weight;
					pStreamData->vertices[i].nz += vertOffset[i].nz*it->second.weight;
				}	
			}
		}
		++it;
	}
}
*/

//	FrameUpdate()
// Updates the morph cbuffers and parameters needed for rendering.
// This must be called every frame by the renderer.
void CMorpher::FrameUpdate ( void )
{
	// TODO: Select the 4 vertex buffers with the biggest weights.
	// Calculate the final weights, and save them for the following two functions.
}

//	SetVertexBuffers( gfx context, start slot )
// Throws in used vertex buffers into given slot.
void CMorpher::SetVertexBuffers( gpu::GraphicsContext* gfx, int startSlot )
{
	// TODO
}
//	SetBlendCBuffers ( gfx context, cbuffer slot )
// Throws in previously generated blend cbuffer into given slot.
void CMorpher::SetBlendCBuffers ( gpu::GraphicsContext* gfx, int cbufferSlot )
{
	// TODO 
}