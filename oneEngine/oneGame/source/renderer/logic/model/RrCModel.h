//=onengine/renderer=============================================================================//
//
//		class RrCModel : A way to display loaded models.
//
// A RrCModel is a container for renderable objects, their contents of which are loaded from disk.
// Fun fact: this is the oldest displaying class in the renderer, and has seen many revisions.
//
//===============================================================================================//
#ifndef RENDERER_LOGIC_MODEL_CONTAINER_
#define RENDERER_LOGIC_MODEL_CONTAINER_

#include "core/types/ModelData.h"
#include "core/math/Cubic.h"
#include "core/math/BoundingBox.h"
#include "core-ext/types/sHitbox.h"
#include "renderer/logic/RrLogicObject.h"
#include "renderer/types/ModelStructures.h"
#include "gpuw/Buffers.h"

namespace renderer
{
	class Mesh;
}
class AnimationControl;
class rrMeshBuffer;
class physMesh;
class RrAnimatedMeshGroup;

//	rrAnimReferenceType : Animation referencing mode.
// Used for when copying skeletons over to the model.
enum rrAnimReferenceType
{
	kAnimReferenceNone,
	kAnimReferenceDirect,
	kAnimReferenceMatch,
};

//	rrModelLoadParams : Parameter struct for loading models.
// Used for when only parts of the model data is needed.
struct rrModelLoadParams
{
	const char*			resource_name = nullptr;
	bool				geometry = true;
	bool				morphs = false;
	bool				animation = false;
	bool				skeleton = false;
	bool				collision = false;
};

//	rrCModelBuffers : Structure holding buffers created by the model and their status.
struct rrCModelConstantBuffers
{
	bool				m_sbufSkinningMajorValid;
	gpu::Buffer			m_sbufSkinningMajor;
	bool				m_sbufSkinningMinorValid;
	gpu::Buffer			m_sbufSkinningMinor;

	rrCModelConstantBuffers ( void )
		: m_sbufSkinningMajorValid(false), m_sbufSkinningMinorValid(false)
		{}
};

// TODO: Add global function that loads RrAnimatedMeshGroup. For instance, foliage could benefit from sharing the model pool.

//	RrCModel : Container for handling animated and static meshes.
// Is the basis of the "Mesh Trio," which are:
//	* RrCModel : This class which loads the data and links renderable and animation systems.
//	* RrAnimatedMeshGroup : The "raw" container, managed by resource system.
//	* renderer::Mesh : The actual renderable for the model.
class RrCModel : public RrLogicObject
{
protected:
	// Constructor. Creates model.
	explicit				RrCModel ( RrAnimatedMeshGroup* mesh_group, const rrModelLoadParams& params, RrWorld* world_to_add_to );

public: // Creation Interface

	//	Load ( filename ), full : Creates a model loaded from file, and returns it.
	// May create a model using previously loaded data.
	// The model is automatically added to the active objects list.
	// Physics and animation data will also be added to the resource system.
	static RENDER_API RrCModel*
							Load ( const char* resource_name, RrWorld* world_to_add_to );

	//	Load ( options ), partial : Creates a model loaded from file, and returns it.
	// 
	// Load(filename), full, calls this with all options enabled.
	static RENDER_API RrCModel*
							Load ( const rrModelLoadParams& load_params, RrWorld* world_to_add_to );

	//	Upload ( data, id )
	static RENDER_API RrCModel*
							Upload ( arModelData& model_data, RrWorld* world_to_add_to );

//protected:
	// Get the model bounding box
	//void					CalculateBoundingBox ( void );

public:
	//	Destructor : Frees used references.
	RENDER_API virtual		~RrCModel ( void );

	// Executed before the renderer starts. Is guaranteed to be called and finish before any PreRender call.
	void					PreStep ( void ) override;

	// Executed after PostStep jobs requests are started (after EndRender).
	void					PostStepSynchronus ( void ) override;

	//======================================================//
	// SETTERS
	//======================================================//

	// Set if to use frustum culling or not
	RENDER_API void			SetFrustumCulling ( bool useCulling = true )
		{ m_useFrustumCulling = useCulling; }
	// Forces the model to be drawn the next frame
	RENDER_API void			SetForcedDraw ( void );

	// Change the animation mode to reference another model's animation.
	// This mode will save memory, but will completely copy the referenced animation
	//  with no ability to change this model's animation.
	// If the referenced model is deleted, unexpected behavior will occur.
	RENDER_API void			SetReferencedAnimationMode ( RrCModel*, const rrAnimReferenceType = kAnimReferenceDirect );

	// Hides or shows all of the child meshes
	RENDER_API void			SetVisibility ( const bool n_visibility );

	//======================================================//
	// GETTERS / FINDERS
	//======================================================//

	// Get the number of meshes this model manages
	RENDER_API size_t		GetMeshCount ( void ) const;
	// Gets the mesh with the index
	RENDER_API renderer::Mesh*
							GetMesh ( const uint n_index ) const;
	// Gets the mesh with the name
	RENDER_API renderer::Mesh*
							GetMesh ( const char* n_name ) const;
	// Gets the indicated mesh data in the array
	RENDER_API arModelData*	GetModelData ( int iMeshIndex ) const;
	// Returns the first matching mesh with the given name in the array
	RENDER_API arModelData*	GetModelDataByName ( const char* nNameMatch ) const;

	// Get the hitbox list
	//std::vector<sHitbox>*	GetHitboxes ( void )
	//	{ return &m_physHitboxes; }

	// Gets the bounding box of the model
	const core::math::BoundingBox&
							GetBoundingBox ( void ) const
		{ return m_worldBoundingBox; }

	// Gets if any of the meshes are being rendered
	RENDER_API bool			GetVisibility ( void );

	//	GetBuffers() : Returns internal buffer structure for access & binding.
	// Contains the skinning buffers & other data.
	rrCModelConstantBuffers&
							GetBuffers ( void )
		{ return m_buffers; }

public:
	XrTransform			transform;

protected:
	// Rendering Options
	//======================================================//

	bool				m_useFrustumCulling = true;
	bool				m_useInstancing = false;
	bool				m_useSkinning = false;

	// Rendering state
	//======================================================//

	core::math::BoundingBox
						m_worldBoundingBox;
	rrCModelConstantBuffers
						m_buffers;

	// Mesh Data
	//======================================================//

	// procedurally created by either cpu or gpu
	bool				m_procedural;

	std::vector<renderer::Mesh*>
						m_meshes;
	core::math::Cubic	m_totalMeshBounds;

	// Referenced Mesh Data
	RrAnimatedMeshGroup*
						m_meshGroup;

	//// Animation data
	//AnimationControl*	pMyAnimation;
	//AnimationControl*	pReferencedAnimation;
	//bool				bReferenceAnimation;
	//rrAnimReferenceType	m_animRefMode;

	//// File Info
	//arstring128			m_resourceName;

	//// Frustum Culling
	//Vector3f			vCheckRenderPos;
	//float				fCheckRenderDist;
	//Vector3f			vMinExtents;
	//Vector3f			vMaxExtents;
	//core::math::BoundingBox
	//					m_renderBoundingBox;
	
};

#endif//RENDERER_LOGIC_MODEL_CONTAINER_