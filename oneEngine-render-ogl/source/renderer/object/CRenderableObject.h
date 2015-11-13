// Renderable Object class
// base class for all objects that need to be rendered by the device

#ifndef _C_RENDERABLE_OBJECT_
#define _C_RENDERABLE_OBJECT_

// Includes
#include "core/types/types.h"
#include "core/math/Vector3d.h"
#include "core-ext/transform/CTransform.h"

//#include "GLCommon.h"
//#include "glMainSystem.h"
//#include "renderer/material/glMaterial.h"

#include "renderer/types/glShaderConstants.h"
#include "renderer/types/ObjectSettings.h"

//#include "CCamera.h"

// Class prototypes
class CRenderState;
class glMaterial;
class glPass;
class glPass_Deferred;

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
		typedef Renderer::RenderingType RenderingType;
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
	virtual glMaterial**	GetPassMaterialPosition ( const char pass ) { return &(vMaterials[pass]); }*/

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
	RENDER_API virtual void			SetMaterial		( glMaterial* n_pNewMaterial );
	// Change the object's render type
	RENDER_API void					SetRenderType	( RenderingType );
	// Change visible state
	RENDER_API virtual void			SetVisible ( const bool nextState ) {
		visible = nextState;
	}
	// == Getters ==
	// Searches for the first material with the string in its name
	//glMaterial*				FindMaterial( const string & strToFind, int skipAmount=0 );
	RENDER_API virtual glMaterial*	GetMaterial ( void ) {
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
	RENDER_API virtual glPass*		GetPass ( const uchar pass );
	// Returns the associated deferred rendering pass. This is used for ordering.
	RENDER_API virtual glPass_Deferred*GetPassDeferred ( const uchar pass );


private:
	// == Update Prototypes ==
	void UpdateRenderInfo ( void ) {
		if ( renderType == Renderer::V2D ) {
			renderDistance = ( transform.position.z * 50.0f ) + ( ( _activeCameraPosition - transform.position ).sqrMagnitude() * 0.005f );
		}
		else {
			renderDistance = ( _activeCameraPosition - transform.position ).sqrMagnitude();
		}
	};

public:
	// Positional transform
	CTransform	transform;
	// Rendering states
	//bool						visible;			// if object should be drawn or not
	Renderer::objectSettings	renderSettings;		// miscelleneous render settings
	glShaderConstants			shaderConstants;

protected:
	// Rendering States
	RenderingType			renderType;
	//vector<glMaterial*>	vMaterials;
protected:
	glMaterial*					m_material;
	bool						visible;
private:
	//vector<Renderer::new_passinfo_t>	m_passinfo;	
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
	static glMaterial* GLoutlineMaterial;

	static void InitMaterials ( void )
	{
		if ( !bStaticMaterialsInit )
		{
			GLoutlineMaterial = new glMaterial();
			GLoutlineMaterial->iFaceMode = GLoutlineMaterial->FM_BACK;
			GLoutlineMaterial->setShader( new glShader( ".res/shaders/d/outline.glsl" ) );

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