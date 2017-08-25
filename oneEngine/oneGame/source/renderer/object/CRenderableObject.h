// Renderable Object class
// base class for all objects that need to be rendered by the device

#ifndef _C_RENDERABLE_OBJECT_
#define _C_RENDERABLE_OBJECT_

// Includes
#include "core/types/types.h"
#include "core/math/Vector3d.h"
#include "core-ext/transform/Transform.h"

//#include "GLCommon.h"
//#include "glMainSystem.h"
//#include "renderer/material/RrMaterial.h"

#include "renderer/types/RrObjectMaterialProperties.h"
#include "renderer/types/ObjectSettings.h"

//#include "CCamera.h"

// Class prototypes
class CRenderState;
class RrMaterial;
class RrPassForward;
class RrPassDeferred;

// Defines
#ifndef RegisterRenderClassName
#	define RegisterRenderClassName(_strname) public: virtual string GetTypeName ( void ) { return string(_strname); };
#endif
#ifndef RegisterRenderBaseClass
#	define RegisterRenderBaseClass(_strname) public: virtual string GetBaseClassName ( void ) { return string(_strname); };
#endif
#ifndef delete_safe
#	define delete_safe(_ptr) { if ( _ptr ) { delete (_ptr); (_ptr) = NULL; } };
#endif

// Base class, virtual/abstract
class CRenderableObject
{
public:
		typedef renderer::RenderingType RenderingType;
private:
	// No copying with "="
	CRenderableObject & operator= (const CRenderableObject & other);

public:
	RENDER_API explicit				CRenderableObject ( void );
	RENDER_API virtual				~CRenderableObject ( void );

protected:
	// Returns of material placement can be edited
	/*virtual bool			GetPassMaterialConst ( const char pass ) { return false; }
	// Returns the material placement
	virtual RrMaterial**	GetPassMaterialPosition ( const char pass ) { return &(vMaterials[pass]); }*/

public:
	// == Rendering Prototypes ==
	// If any of the rendering functions return false, the renderer will throw an exception.
	RENDER_API virtual bool			PreRender ( const char pass ) {
		return true;
	}
	RENDER_API virtual bool			Render ( const char pass ) =0;
	RENDER_API virtual bool			PostRender ( const char pass ) {
		return true;
	}

	RENDER_API virtual bool			BeginRender ( void ) {
		return true;
	}
	RENDER_API virtual bool			EndRender ( void ) {
		return true;
	}

	// == Setters ==
	// Change the material the given material array thing.
	// Give the ownership of the material to this mesh if the materials have been "released"
	RENDER_API virtual void			SetMaterial		( RrMaterial* n_pNewMaterial );
	// Change the object's render type
	RENDER_API void					SetRenderType	( RenderingType );
	// Change visible state
	RENDER_API virtual void			SetVisible ( const bool nextState ) {
		visible = nextState;
	}
	// == Getters ==
	// Searches for the first material with the string in its name
	//RrMaterial*				FindMaterial( const string & strToFind, int skipAmount=0 );
	RENDER_API virtual RrMaterial*	GetMaterial ( void ) {
		return m_material;
	}
	// Returns visible state
	RENDER_API virtual bool			GetVisible ( void ) const {
		return visible;
	}

	// == Culling/Prerendering Prototypes ==
	// Since culling is done before adding to the rendering list for a speed gain (OR WILL BE DONE)
	/*virtual bool			GetVisibility ( void ) {
		return true;
	}*/
	// Get the number of passes (as models will need a lot more than one pass)
	RENDER_API virtual uchar		GetPassNumber ( void );

	// Returns the associated pass. This is used for ordering.
	RENDER_API virtual RrPassForward*		GetPass ( const uchar pass );
	// Returns the associated deferred rendering pass. This is used for ordering.
	RENDER_API virtual RrPassDeferred*GetPassDeferred ( const uchar pass );


private:
	// == Update Prototypes ==


	//		UpdateRenderInfo : generate data needed for sorting
	// Generated data needed for sorting, namely distance from the camera. Is not fast.
	void UpdateRenderInfo ( void )
	{
		if ( renderType == renderer::V2D )
		{
			renderDistance = ( transform.world.position.z * 50.0f ) + ( ( _activeCameraPosition - transform.world.position ).sqrMagnitude() * 0.005f );
		}
		else
		{
			renderDistance = ( _activeCameraPosition - transform.world.position ).sqrMagnitude();
		}
	};

public:
	// Positional transform
	core::Transform	transform;
	// Rendering states
	//bool						visible;			// if object should be drawn or not
	renderer::objectSettings	renderSettings;		// miscelleneous render settings
	RrObjectMaterialProperties			shaderConstants;

protected:
	// Rendering States
	RenderingType			renderType;
	//vector<RrMaterial*>	vMaterials;
protected:
	RrMaterial*					m_material;
	bool						visible;
private:
	//vector<renderer::new_passinfo_t>	m_passinfo;	
	uint*		m_vao_info;
	uint		m_vao_count;
	uint		m_vao_maxcount;

	RENDER_API void PassinfoClear ( void );
	RENDER_API void PassinfoGenerate ( void );

	RENDER_API void PassinfoRegenerate ( void );

protected:
	// ==Render Setup==
	
	// BindVAO()
	// Must be called after the vertex buffer is bound, as will bind materials to render.
	RENDER_API bool BindVAO ( const uchar pass, const uint vbo, const uint eab=0, const bool userDefinedAttribs=false );

	// ==Render Status==
	RENDER_API float GetRenderDistance ( void );

	// ==Materials==
	// Clears up the entire material list.
	//void ClearMaterialList ( void );

	// Public materials that are reused
	/*static bool bStaticMaterialsInit;
	static RrMaterial* GLoutlineMaterial;

	static void InitMaterials ( void )
	{
		if ( !bStaticMaterialsInit )
		{
			GLoutlineMaterial = new RrMaterial();
			GLoutlineMaterial->iFaceMode = GLoutlineMaterial->FM_BACK;
			GLoutlineMaterial->setShader( new RrShader( ".res/shaders/d/outline.glsl" ) );

			bStaticMaterialsInit = true;
		}
	};*/
	static Vector3d _activeCameraPosition;
	static Rotator	_activeCameraRotation;

private:
	// Distance from the render pivot
	float renderDistance;
	// ID values
	uint32_t id;
	void SetId ( unsigned int );
	// Give storage class access to SetId
	friend CRenderState;
};

// typedef for persons coming from Unity
typedef CRenderableObject RenderObject;

#endif