#ifndef RENDERER_LOGIC_MODEL_CONTAINER_
#define RENDERER_LOGIC_MODEL_CONTAINER_

#include "core/types/ModelData.h"
#include "core/math/BoundingBox.h"

#include "core-ext/types/sHitbox.h"

#include "renderer/logic/RrLogicObject.h"
#include "renderer/types/ModelStructures.h"

//#include "renderer/logic/model/RrAnimatedMeshGroup.h"

namespace renderer
{
	class Mesh;
}
class AnimationControl;
class rrMeshBuffer;
class physMesh;
class RrAnimatedMeshGroup;

#include <unordered_map>

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
	const char*	resource_name;
	bool	morphs;
	bool	animation;
	bool	hitboxes;
	bool	collision;
};

//	CModel : Container for handling animated and static meshes.
// Is the basis of the "Mesh Trio," which are:
//	* CModel : This class
//	* RrAnimatedMeshGroup : The "raw" container, managed by resource system.
//	* 
class CModel : public RrLogicObject
{
protected:
	// Constructor. Creates model.
	explicit				CModel ( void );

public: // Creation Interface

	//	LoadFile ( filename ) : Creates a model loaded from file, and returns it.
	// May create a model using previously loaded data.
	// The model is automatically added to the active objects list.
	// Physics and animation data will also be added to the resource system.
	static RENDER_API CModel*
							Load ( const char* resource_name );
	//	LoadFilePartial ( filename, ...options... ) : Creates a model loaded from file, and returns it.
	// 
	// LoadFile(...) calls this with all options enabled.
	static RENDER_API CModel*
							Load ( const rrModelLoadParams& load_params );

	//	Upload ( data, id )
	static RENDER_API CModel*
							Upload ( arModelData& model_data );

private:
	//	LoadModel ( filename ) : Load some shit
	//void					LoadModel ( const char* resource_name );

protected:
	// Get the model bounding box
	void					CalculateBoundingBox ( void );

public:
	// == RENDERABLE OBJECT INTERFACE ==
	// Get the number of passes (as models will need a lot more than one pass)
	/*virtual unsigned char	GetPassNumber ( void );
	// Returns the material associated with the given pass
	virtual RrMaterial*		GetPassMaterial ( const char pass );
protected:
	// Returns of material placement can be edited
	virtual bool			GetPassMaterialConst ( const char pass );
	// Returns the material placement
	virtual RrMaterial**	GetPassMaterialPosition ( const char pass );*/

public:
	//	Destructor : Frees used references.
	RENDER_API virtual		~CModel ( void );

	// Send shader uniform list
	//RENDER_API void SendShaderUniforms ( void );

	// Executed before the renderer starts. Is guaranteed to be called and finish before any PreRender call.
	void					PreStep ( void ) override;
	// Executed after PostStep jobs requests are started (after EndRender).
	void					PostStepSynchronus ( void ) override;

	// Frustom Culling Check
	//bool PreRender ( const char pass );
	// Public Render Routine
	//bool Render ( const char pass );

	//======================================================//
	// MATERIAL HANDLING
	//======================================================//

	// Recalcuate normals on all owned meshes. Normally not wanted, unless you're creating
	// procedural meshes.
	//RENDER_API void			RecalculateNormals ( void );
	// Duplicates the mesh materials to a local copy.
	// This prevents results from GetPassMaterial from affecting the original mesh.
	//RENDER_API void			DuplicateMaterials ( void );

	// Change the material the given material array thing.
	// Give the ownership of the material to this mesh if the materials have been "released"
	//RENDER_API void			SetMaterial ( RrMaterial* n_pNewMaterial );

	// Returns material of mesh, or throws error if has multiple submeshes
	//RENDER_API RrMaterial*	GetMaterial ( void );

	//======================================================//
	// SETTERS
	//======================================================//

	// Set if to use frustum culling or not
	RENDER_API void			SetFrustumCulling ( bool useCulling = true );
	// Forces the model to be drawn the next frame
	RENDER_API void			SetForcedDraw ( void );
	// Adds an entry to the uniform list to be set on render
	//RENDER_API void SetShaderUniform ( const char*, float const );
	//RENDER_API void SetShaderUniform ( const char*, Vector2d const& );
	//RENDER_API void SetShaderUniform ( const char*, Vector3d const& );
	//RENDER_API void SetShaderUniform ( const char*, Color const& );
	//RENDER_API void SetShaderUniform ( const char*, Matrix4x4 const& );
	//RENDER_API void SetShaderUniformV ( const char*, unsigned int, const Matrix4x4* );
	//RENDER_API void SetShaderUniformV ( const char*, unsigned int, const Matrix3x3* );
	//RENDER_API void SetShaderUniformV ( const char*, unsigned int, const Vector3d* );

	// Change the animation mode to reference another model's animation.
	// This mode will save memory, but will completely copy the referenced animation
	//  with no ability to change this model's animation.
	// If the referenced model is deleted, unexpected behavior will occur.
	RENDER_API void			SetReferencedAnimationMode ( CModel*, const rrAnimReferenceType = kAnimReferenceDirect );

	// Hides or shows all of the child meshes
	RENDER_API void			SetVisibility ( const bool n_visibility );
	// Sets child mesh render types
	//RENDER_API void			SetRenderType ( const renderer::rrRenderLayer n_type );

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
	// Gets the filename of the model
	//string					GetFilename ( void )
	//	{ return myModelFilename; }
	// Gets the indicated mesh data in the array
	RENDER_API arModelData*	GetModelData ( int iMeshIndex ) const;
	// Returns the first matching mesh with the given name in the array
	RENDER_API arModelData*	GetModelDataByName ( const char* nNameMatch ) const;
	// Return the first matching material
	//RENDER_API RrMaterial*	FindMaterial ( const char* n_name, const int n_offset=0 ) const;

	// Get the animation reference
	/*AnimationControl*	GetAnimation ( void ) {
		return pMyAnimation;
	}*/

	// Get the hitbox list
	//std::vector<sHitbox>*	GetHitboxes ( void )
	//	{ return &m_physHitboxes; }
	// Gets the bounding box of the model
	core::math::BoundingBox	GetBoundingBox ( void ) const
		{ return m_renderBoundingBox; }

	// Gets if any of the meshes are being rendered
	RENDER_API bool			GetVisibility ( void );

public:
	XrTransform			transform;

protected:
	// Rendering Options
	bool				bUseFrustumCulling;
	//bool				bUseSeparateMaterialBatches;
	//bool				bCanRender;

	// Mesh Data
	//vector<rrMesh*>	vMeshes;
	//vector<physMesh*>	vPhysMeshes;
	std::vector<renderer::Mesh*>
						m_meshes;
	//std::vector<rrMesh*>
	//					m_glMeshlist;

	// physmesh listing
	/*std::vector<physMesh*>
						m_physMeshlist;
	std::vector<sHitbox>
						m_physHitboxes;*/

	// mesh data
	RrAnimatedMeshGroup*
						m_meshGroup;
	// procedurally created by either cpu or gpu
	bool				m_procedural;

	// Animation data
	AnimationControl*	pMyAnimation;
	AnimationControl*	pReferencedAnimation;
	bool				bReferenceAnimation;
	rrAnimReferenceType	m_animRefMode;

	// File Info
	arstring128			m_resourceName;

	// Frustum Culling
	Vector3d			vCheckRenderPos;
	float				fCheckRenderDist;
	Vector3d			vMinExtents;
	Vector3d			vMaxExtents;
	core::math::BoundingBox
						m_renderBoundingBox;

	// Shader uniforms
	//std::unordered_map<arstring128,float>*	uniformMapFloat;
	//std::unordered_map<arstring128,Vector2d>*	uniformMapVect2d;
	//std::unordered_map<arstring128,Vector3d>*	uniformMapVect3d;
	//std::unordered_map<arstring128,Color>*	uniformMapColor;

//private:
	//bool bUseBoneVertexBlending;
	
};

#endif//RENDERER_LOGIC_MODEL_CONTAINER_