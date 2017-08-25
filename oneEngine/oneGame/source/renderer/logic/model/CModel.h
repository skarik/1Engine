
#ifndef _C_MODEL_
#define _C_MODEL_

// Includes
#include "core/types/ModelData.h"
#include "core/math/BoundingBox.h"

#include "core-ext/types/sHitbox.h"

#include "renderer/logic/CLogicObject.h"
#include "renderer/types/ModelStructures.h"


class CMesh;
class CAnimation;
class rrMesh;
class physMesh;

//#include "renderer/object/mesh/CMesh.h"
/*
#include "arModelData.h"
#include "CRenderableObject.h"
#include "RrMaterial.h"
#include <vector>
//using std::vector;

#include "rrMesh.h"
#include "physMesh.h"

#include "CSegmentedFile.h"

#include "StringUtils.h"

#include "CModelMaster.h"

#include "BoundingBox.h"

#include "CAnimationSet.h"
#include "CAnimation.h"

#include "sHitbox.h"

#include "CMesh.h"*/

// Include hash tables
/*#if __cplusplus > 199711L
	#include <unordered_map>
	using std::unordered_map;
#else
	#include <unordered_map>
	using std::tr1::unordered_map;
#endif*/
#include <unordered_map>

// Class Definition
class CModel : public CLogicObject
{
public:
	// Constructor

	// Loads up model file
	RENDER_API explicit CModel ( const char* sFilename );	
	// Load up model data struct
	RENDER_API explicit CModel ( arModelData& mdInModelData, const char* sModelName = "_sys_override_" );
	// Create empty model
	RENDER_API explicit CModel ( void ) : CLogicObject() {
		// Clear out uniform lists
		uniformMapFloat = NULL;
		uniformMapVect2d = NULL;
		uniformMapVect3d = NULL;
		uniformMapColor = NULL;
	};
	// Destructor
	RENDER_API virtual ~CModel ( );

	enum AnimRefType {
		ANIM_REF_NONE = 0,
		ANIM_REF_DIRECT = 1,
		ANIM_REF_MATCH = 2
	};
private:
	// Load some shit
	void LoadModel ( const string& sFilename );

protected:
	// Get the model bounding box
	void CalculateBoundingBox ( void );

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
	// Send shader uniform list
	RENDER_API void SendShaderUniforms ( void );

	// Begin render
	void PreStep ( void ) override;
	// End render
	void PostStepSynchronus ( void ) override;

	// Frustom Culling Check
	//bool PreRender ( const char pass );
	// Public Render Routine
	//bool Render ( const char pass );

	//======================================================//
	// MATERIAL HANDLING
	//======================================================//

	// Recalcuate normals on all owned meshes. Normally not wanted, unless you're creating
	// procedural meshes.
	RENDER_API void RecalculateNormals ( void );
	// Duplicates the mesh materials to a local copy.
	// This prevents results from GetPassMaterial from affecting the original mesh.
	RENDER_API void DuplicateMaterials ( void );

	// Change the material the given material array thing.
	// Give the ownership of the material to this mesh if the materials have been "released"
	RENDER_API void SetMaterial ( RrMaterial* n_pNewMaterial );

	// Returns material of mesh, or throws error if has multiple submeshes
	RENDER_API RrMaterial* GetMaterial ( void );

	//======================================================//
	// SETTERS
	//======================================================//

	// Set if to use frustum culling or not
	RENDER_API void SetFrustumCulling ( bool useCulling = true );
	// Forces the model to be drawn the next frame
	RENDER_API void SetForcedDraw ( void );
	// Adds an entry to the uniform list to be set on render
	RENDER_API void SetShaderUniform ( const char*, float const );
	RENDER_API void SetShaderUniform ( const char*, Vector2d const& );
	RENDER_API void SetShaderUniform ( const char*, Vector3d const& );
	RENDER_API void SetShaderUniform ( const char*, Color const& );
	RENDER_API void SetShaderUniform ( const char*, Matrix4x4 const& );
	RENDER_API void SetShaderUniformV ( const char*, unsigned int, const Matrix4x4* );
	RENDER_API void SetShaderUniformV ( const char*, unsigned int, const Matrix3x3* );
	RENDER_API void SetShaderUniformV ( const char*, unsigned int, const Vector3d* );

	// Change the animation mode to reference another model's animation.
	// This mode will save memory, but will completely copy the referenced animation
	//  with no ability to change this model's animation.
	// If the referenced model is deleted, unexpected behavior will occur.
	RENDER_API void SetReferencedAnimationMode ( CModel*, const AnimRefType = ANIM_REF_DIRECT );

	// Hides or shows all of the child meshes
	RENDER_API void SetVisibility ( const bool n_visibility );
	// Sets child mesh render types
	RENDER_API void SetRenderType ( const renderer::RenderingType n_type );

	//======================================================//
	// GETTERS / FINDERS
	//======================================================//

	// Get the number of meshes this model manages
	RENDER_API uint	GetMeshCount ( void ) const;
	// Gets the mesh with the index
	RENDER_API CMesh*	GetMesh ( const uint n_index ) const;
	// Gets the mesh with the name
	RENDER_API CMesh*	GetMesh ( const char* n_name ) const;
	// Gets the filename of the model
	string GetFilename ( void ) {
		return myModelFilename;
	}
	// Gets the indicated mesh data in the array
	RENDER_API arModelData* GetModelData ( int iMeshIndex ) const;
	// Returns the first matching mesh with the given name in the array
	RENDER_API arModelData* GetModelDataByName ( const char* nNameMatch ) const;
	// Return the first matching material
	RENDER_API RrMaterial*	FindMaterial ( const char* n_name, const int n_offset=0 ) const;

	// Get the animation reference
	/*CAnimation*	GetAnimation ( void ) {
		return pMyAnimation;
	}*/
	// Get the hitbox list
	std::vector<sHitbox>*	GetHitboxes ( void ) {
		return &vHitboxes;
	}
	// Gets the bounding box of the model
	BoundingBox GetBoundingBox ( void ) const {
		return bbCheckRenderBox;
	}

	// Gets if any of the meshes are being rendered
	RENDER_API bool GetVisibility ( void );

public:
	XrTransform transform;

protected:
	// Rendering Options
	bool					bUseFrustumCulling;
	//bool					bUseSeparateMaterialBatches;
	//bool					bCanRender;

	// Mesh Data
	//vector<rrMesh*>			vMeshes;
	//vector<physMesh*>		vPhysMeshes;
	std::vector<CMesh*>			m_meshes;
	std::vector<rrMesh*>		m_glMeshlist;
	std::vector<physMesh*>		m_physMeshlist;
	// Collision data
	std::vector<sHitbox>			vHitboxes;

	// Animation data
	CAnimation*				pMyAnimation;
	CAnimation*				pReferencedAnimation;
	bool					bReferenceAnimation;
	AnimRefType				eRefMode;

	// File Info
	string				myModelFilename;

	// Frustum Culling
	Vector3d	vCheckRenderPos;
	float		fCheckRenderDist;
	Vector3d	vMinExtents;
	Vector3d	vMaxExtents;
	BoundingBox bbCheckRenderBox;

	// Shader uniforms
	std::unordered_map<arstring128,float>*			uniformMapFloat;
	std::unordered_map<arstring128,Vector2d>*		uniformMapVect2d;
	std::unordered_map<arstring128,Vector3d>*		uniformMapVect3d;
	std::unordered_map<arstring128,Color>*			uniformMapColor;

private:
	//bool bUseBoneVertexBlending;
	
};

#endif