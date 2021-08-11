#include "RrCModel.h"

#include "core/time/time.h"
#include "core/utils/string.h"
#include "core-ext/animation/AnimationControl.h"

#include "core-ext/system/io/FileUtils.h"
#include "core-ext/system/io/Resources.h"
#include "core-ext/resources/ResourceManager.h"

#include "renderer/object/mesh/Mesh.h"
#include "renderer/meshmodel/RrAnimatedMeshGroup.h"
#include "renderer/meshmodel/RrModelMasterSubsystem.h"

#include "core-ext/system/io/assets/ModelIO.h"

template <typename Type>
void AssignMPDDataAsType ( Type*& target, const void* data )
{
	ARCORE_ASSERT(target == NULL);
	target = (Type*)data;
}

static bool LoadModelToMeshGroup ( const rrModelLoadParams& load_params, RrAnimatedMeshGroup* mesh_group )
{
	ARCORE_ASSERT(mesh_group != NULL);

	// For now, we just mark the mesh group's load state as loaded for unsupported items
	if (load_params.morphs)
	{
		mesh_group->m_loadState.morphs = true;
	}
	if (load_params.skeleton)
	{
		mesh_group->m_loadState.skeleton = true;
	}

	// Load in geometry
	if (load_params.geometry)
	{
		core::MpdInterface mpd;
		mpd.Open(load_params.resource_name, true);
		
		for (uint meshIndex = 0; meshIndex < mpd.GetSegmentCount(core::ModelFmtSegmentType::kGeometryInfo); ++meshIndex)
		{
			rrMeshBuffer*		meshBuffer = nullptr;
			arModelData*		meshData = nullptr;
			core::math::Cubic	meshBounds;
			arstring64			meshName;

			// First pull in the geometry general information
			core::modelFmtSegmentGeometry_Info* geoInfo = (core::modelFmtSegmentGeometry_Info*)mpd.GetSegmentData(core::ModelFmtSegmentType::kGeometryInfo, meshIndex);
			ARCORE_ASSERT(geoInfo->IsValid());
			{
				meshBuffer = new rrMeshBuffer();
				meshData = new arModelData();

				meshData->indexNum = geoInfo->index_count;
				meshData->vertexNum = geoInfo->vertex_count;
				meshBounds = core::math::Cubic::FromPosition(geoInfo->bbox_min, geoInfo->bbox_max);
				meshName = geoInfo->name;
			}
			mpd.ReleaseSegmentData(core::ModelFmtSegmentType::kGeometryInfo, meshIndex);

			// Pull in the index data of matching mesh
			for (uint segmentIndex = 0; segmentIndex < mpd.GetSegmentCount(core::ModelFmtSegmentType::kGeometryIndices); ++segmentIndex)
			{
				if (mpd.GetSegment(core::ModelFmtSegmentType::kGeometryIndices, segmentIndex).subindex == meshIndex)
				{
					meshData->indices = (uint16_t*)mpd.GetSegmentData(core::ModelFmtSegmentType::kGeometryIndices, segmentIndex);
					break;
				}
			}

			// Pull in the attribute data of matching mesh
			for (uint segmentIndex = 0; segmentIndex < mpd.GetSegmentCount(core::ModelFmtSegmentType::kGeometryVertexData); ++segmentIndex)
			{
				if (mpd.GetSegment(core::ModelFmtSegmentType::kGeometryVertexData, segmentIndex).subindex == meshIndex)
				{
					switch (mpd.GetSegment(core::ModelFmtSegmentType::kGeometryVertexData, segmentIndex).subtype)
					{
					case (uint)core::ModelFmtVertexAttribute::kPosition:
						AssignMPDDataAsType(meshData->position, mpd.GetSegmentData(core::ModelFmtSegmentType::kGeometryVertexData, segmentIndex));
						break;
					case (uint)core::ModelFmtVertexAttribute::kUV0:
						AssignMPDDataAsType(meshData->texcoord0, mpd.GetSegmentData(core::ModelFmtSegmentType::kGeometryVertexData, segmentIndex));
						break;
					case (uint)core::ModelFmtVertexAttribute::kColor:
						AssignMPDDataAsType(meshData->color, mpd.GetSegmentData(core::ModelFmtSegmentType::kGeometryVertexData, segmentIndex));
						break;
					case (uint)core::ModelFmtVertexAttribute::kNormal:
						AssignMPDDataAsType(meshData->normal, mpd.GetSegmentData(core::ModelFmtSegmentType::kGeometryVertexData, segmentIndex));
						break;
					case (uint)core::ModelFmtVertexAttribute::kTangent:
						AssignMPDDataAsType(meshData->tangent, mpd.GetSegmentData(core::ModelFmtSegmentType::kGeometryVertexData, segmentIndex));
						break;
					case (uint)core::ModelFmtVertexAttribute::kBinormal:
						AssignMPDDataAsType(meshData->binormal, mpd.GetSegmentData(core::ModelFmtSegmentType::kGeometryVertexData, segmentIndex));
						break;
					case (uint)core::ModelFmtVertexAttribute::kUV1:
						AssignMPDDataAsType(meshData->texcoord1, mpd.GetSegmentData(core::ModelFmtSegmentType::kGeometryVertexData, segmentIndex));
						break;
					case (uint)core::ModelFmtVertexAttribute::kBoneWeight:
						AssignMPDDataAsType(meshData->weight, mpd.GetSegmentData(core::ModelFmtSegmentType::kGeometryVertexData, segmentIndex));
						break;
					case (uint)core::ModelFmtVertexAttribute::kBoneIndices:
						AssignMPDDataAsType(meshData->bone, mpd.GetSegmentData(core::ModelFmtSegmentType::kGeometryVertexData, segmentIndex));
						break;
					}
				}
			}

			// Upload mesh to GPU
			meshBuffer->InitMeshBuffers(meshData);

			// Clean up mesh data refs
			{
				arModelData storedRefsMeshData = *meshData;
				*meshData = arModelData();
				meshData->vertexNum = storedRefsMeshData.vertexNum;
				meshData->indexNum = storedRefsMeshData.indexNum;
			}

			// Add the new data to the Meshgroup
			mesh_group->m_meshes.push_back(meshBuffer);
			mesh_group->m_meshBounds.push_back(meshBounds);
			mesh_group->m_meshNames.push_back(meshName);
		}

		mesh_group->m_loadState.meshes = true;
	}

	return true;
}

RrAnimatedMeshGroup* renderer::LoadMeshGroup ( const rrModelLoadParams& load_params )
{
	auto resm = core::ArResourceManager::Active();

	// Generate the resource name from the filename:
	arstring256 resource_str_id (load_params.resource_name);
	core::utils::string::ToResourceName(resource_str_id);

	// First, find the model in the resource system:
	IArResource* existingResource = resm->Find(core::kResourceTypeRrMeshGroup, resource_str_id);
	if (existingResource != NULL)
	{
		// Found it! Add a reference and return it.
		RrAnimatedMeshGroup* existingMeshGroup = (RrAnimatedMeshGroup*)existingResource;
		existingMeshGroup->AddReference();

		// Check what is loaded in the mesh group, and load up the rest:
		rrModelLoadParams effective_load_params = load_params;

		if (load_params.geometry && existingMeshGroup->m_loadState.meshes)
		{
			effective_load_params.geometry = false;
		}
		if (load_params.morphs && existingMeshGroup->m_loadState.morphs)
		{
			effective_load_params.morphs = false;
		}
		if (load_params.skeleton && existingMeshGroup->m_loadState.skeleton)
		{
			effective_load_params.skeleton = false;
		}

		if (effective_load_params.geometry
			|| effective_load_params.morphs
			|| effective_load_params.skeleton)
		{
			bool loadResult = LoadModelToMeshGroup(effective_load_params, existingMeshGroup);
			ARCORE_ASSERT(loadResult);
		}

		return existingMeshGroup;
	}
	else
	{
		// Create a mesh group that holds the information:
		RrAnimatedMeshGroup* meshGroup = new RrAnimatedMeshGroup();
		{
			bool loadResult = LoadModelToMeshGroup(load_params, meshGroup);
			ARCORE_ASSERT(loadResult);
		}

		// Add self to the resource system:
		if (resm->Contains(meshGroup) == false)
			resm->Add(meshGroup);

		return meshGroup;
	}
}

RrCModel*
RrCModel::Load ( const char* resource_name, RrWorld* world_to_add_to )
{
	rrModelLoadParams mlparams;
	mlparams.resource_name = resource_name;
	mlparams.geometry = true;
	mlparams.morphs = true;
	mlparams.animation = true;
	mlparams.skeleton = true;
	mlparams.collision = true;

	return Load(mlparams, world_to_add_to);
}

RrCModel*
RrCModel::Load ( const rrModelLoadParams& load_params, RrWorld* world_to_add_to )
{
	RrAnimatedMeshGroup* meshGroup = renderer::LoadMeshGroup(load_params);

	// Create a new RrCModel with the given RrAnimatedMeshGroup.
	RrCModel* model = new RrCModel(meshGroup, load_params, world_to_add_to);

	return model;
}

RrCModel*
RrCModel::Upload ( arModelData& model_data, RrWorld* world_to_add_to )
{
	return NULL;
}

static core::math::Cubic CalculateTotalMeshBounds ( RrAnimatedMeshGroup* mesh_group )
{
	if (mesh_group->m_meshBounds.empty())
	{
		return core::math::Cubic(Vector3f(), Vector3f());
	}
	else
	{
		core::math::Cubic result = mesh_group->m_meshBounds[0];
		for (int i = 1; i < mesh_group->m_meshBounds.size(); ++i)
		{
			result.Expand(mesh_group->m_meshBounds[i]);
		}
		return result;
	}
}

RrCModel::RrCModel ( RrAnimatedMeshGroup* mesh_group, const rrModelLoadParams& params, RrWorld* world_to_add_to )
	: RrLogicObject()
{
	m_totalMeshBounds = CalculateTotalMeshBounds(mesh_group);

	// Update the world we add to
	if (world_to_add_to != nullptr)
	{
		this->AddToWorld(world_to_add_to);
	}

	// Instiantiate the RrMeshes we use to render:
	for (rrMeshBuffer* mesh_buffer : mesh_group->m_meshes)
	{
		renderer::Mesh* mesh = new renderer::Mesh(mesh_buffer, false);

		if (world_to_add_to != nullptr)
		{
			mesh->AddToWorld(world_to_add_to);
		}

		m_meshes.push_back(mesh);
	}
}

// Destructor
RrCModel::~RrCModel ( void )
{
	/*for ( unsigned int n = 0; n < vMaterials.size(); ++n )
	{
		if ( vMaterials[n] )
			vMaterials[n]->releaseOwnership();
	}*/

	for ( uint i = 0; i < m_meshes.size(); ++i ) 
	{
		delete m_meshes[i];
		m_meshes[i] = NULL;
	}
	m_meshes.clear();
}


// Begin render
void RrCModel::PreStep ( void ) 
{
	// TODO: Update the bounding box

	for ( uint i = 0; i < m_meshes.size(); ++i )
	{
		//m_meshes[i]->transform.Get(transform);
		m_meshes[i]->transform.world = transform;
		//m_meshes[i]->bUseFrustumCulling = bUseFrustumCulling;
	}
	/*if ( visible )
	{
		for ( uint i = 0; i < m_meshes.size(); ++i )
		{
			m_meshes[i]->visible = true;
		}
		visible = false;
	}*/
}
// End render
void RrCModel::PostStepSynchronus ( void ) 
{
	//if ( pMyAnimation )
	//{
	//	pMyAnimation->UpdateTransform( XTransform( transform.position, transform.rotation, transform.scale ) );
	//	if ( !bReferenceAnimation )
	//	{
	//		pMyAnimation->Update(Time::deltaTime);
	//	}
	//	else
	//	{
	//		if ( m_animRefMode == kAnimReferenceDirect )
	//		{
	//			// Directly copy the transforms
	//			//pReferencedAnimation->GetAnimationSet
	//			//vector<void*>& refList = pReferencedAnimation->animRefs;
	//			// THIS WON'T WORK FOR NORMAL FUCKING ANIMATIONS
	//		}
	//	}
	//}
}

//// Set if to use frustum culling or not
//void RrCModel::SetFrustumCulling ( bool useCulling ) {
//	bUseFrustumCulling = useCulling;
//}
// Forces the model to be drawn the next frame
void RrCModel::SetForcedDraw ( void ) {
	//bCanRender = true;
	for ( uint i = 0; i < m_meshes.size(); ++i ) {
		m_meshes[i]->bCanRender = true;
	}
}

// Change the animation mode to reference another model's animation.
// This mode will save memory, but will completely copy the referenced animation
//  with no ability to change this model's animation.
// If the referenced model is deleted, unexpected behavior will occur.
void RrCModel::SetReferencedAnimationMode ( RrCModel* pReference, const rrAnimReferenceType ref_type )
{
	throw core::DeprecatedCallException();
	//pReferencedAnimation = pReference->GetAnimation();
	//bReferenceAnimation = true;
	//eRefMode = ref_type;
}

// Hides or shows all of the child meshes
void RrCModel::SetVisibility ( const bool n_visibility ) {
	for ( uint i = 0; i < m_meshes.size(); ++i ) {
		m_meshes[i]->visible = n_visibility;
	}
}
// Hides or shows all of the child meshes
//void RrCModel::SetRenderType ( const renderer::eRenderLayer n_type ) {
//	for ( uint i = 0; i < m_meshes.size(); ++i ) {
//		m_meshes[i]->renderLayer = n_type;
//	}
//}

//======================================================//
// GETTERS / FINDERS
//======================================================//

// Get the number of meshes this model manages
size_t RrCModel::GetMeshCount ( void ) const
{
	return m_meshes.size();
}
// Gets the mesh with the index
renderer::Mesh* RrCModel::GetMesh ( const uint n_index ) const
{
	ARCORE_ASSERT(n_index >= 0 && n_index < m_meshes.size());
	return m_meshes[n_index];
}
// Gets the mesh with the name
renderer::Mesh* RrCModel::GetMesh ( const char* n_name ) const
{
	for ( uint i = 0; i < m_meshes.size(); ++i )
	{
		if ( m_meshGroup[i].m_name.compare(n_name) )
		{
			return m_meshes[i];
		}
	}
	return NULL;
}

// Gets the indicated mesh data in the array
arModelData* RrCModel::GetModelData ( int iMeshIndex ) const
{
	renderer::Mesh* mesh = GetMesh( iMeshIndex );
	if ( mesh )
	{
		return mesh->m_mesh->m_modeldata;
	}
	return NULL;
}

// Returns the first matching mesh with the given name in the array
arModelData* RrCModel::GetModelDataByName ( const char* nNameMatch ) const 
{
	renderer::Mesh* mesh = GetMesh( nNameMatch );
	if ( mesh )
	{
		return mesh->m_mesh->m_modeldata;
	}
	return NULL;
}

// Return the first matching material
//RrMaterial*	RrCModel::FindMaterial ( const char* n_name, const int n_offset ) const
//{
//	int foundCount = 0;
//	for ( uint i = 0; i < m_meshes.size(); ++i ) {
//		if ( string(m_meshes[i]->GetMaterial()->getName()).find( n_name ) != string::npos ) {
//			foundCount += 1;
//			if ( foundCount > n_offset ) {
//				return m_meshes[i]->GetMaterial();
//			}
//		}
//	}
//	return NULL;
//}

// Gets if any of the meshes are being rendered
bool RrCModel::GetVisibility ( void )
{
	for ( uint i = 0; i < m_meshes.size(); ++i )
	{
		if ( m_meshes[i]->GetVisible() )
		{
			return true;
		}
	}
	return false;
}