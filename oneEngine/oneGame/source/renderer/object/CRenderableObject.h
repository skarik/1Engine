//===============================================================================================//
//
//	class CRenderableObject
//
// base class for all objects that need to be rendered by the device
//
//===============================================================================================//

#ifndef C_RENDERABLE_OBJECT_
#define C_RENDERABLE_OBJECT_

// Includes
#include "core/types/types.h"
#include "core/math/Vector3d.h"
#include "core-ext/transform/Transform.h"

//#include "GLCommon.h"
//#include "glMainSystem.h"
//#include "renderer/material/RrMaterial.h"

#include "renderer/types/RrObjectMaterialProperties.h"
#include "renderer/types/ObjectSettings.h"

#include "renderer/material/RrPass.h"
#include "renderer/gpuw/Pipeline.h"
#include "renderer/gpuw/Buffers.h"

//#include "RrCamera.h"

// Class prototypes
class RrRenderer;
//class RrMaterial;
//class RrPassForward;
//class RrPassDeferred;
class RrPass;
class RrCamera;

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
#ifndef delete_safe_array
#	define delete_safe_array(_ptr) { if ( _ptr ) { delete[] (_ptr); (_ptr) = NULL; } };
#endif

// Base class, virtual/abstract
class CRenderableObject
{
private:
	// No copying with "="
	CRenderableObject & operator= (const CRenderableObject & other);

public:
	RENDER_API explicit		CRenderableObject ( void );
	RENDER_API virtual		~CRenderableObject ( void );

protected:
	// Returns of material placement can be edited
	/*virtual bool			GetPassMaterialConst ( const char pass ) { return false; }
	// Returns the material placement
	virtual RrMaterial**	GetPassMaterialPosition ( const char pass ) { return &(vMaterials[pass]); }*/

public:
	// == Rendering Prototypes ==
	// If any of the rendering functions return false, the renderer will throw an exception.

	//	PreRender() : Called before the internal render-loop executes.
	// Can be called multiple times per frame.
	RENDER_API virtual bool	PreRender ( RrCamera* camera )
		{ return true; }
	//	Render(const int pass) : Current pass
	RENDER_API virtual bool	Render ( const char pass ) =0;
	//	PostRender() : Called after the render-loop executes.
	// Can be called multiple times per frame.
	RENDER_API virtual bool	PostRender ( RrCamera* camera )
		{ return true; }
	//	BeginRender() : Called before the render-loop executes.
	// Called once per frame.
	RENDER_API virtual bool	BeginRender ( void )
		{ return true; }
	//	EndRender() : Called after the render-loop executes.
	// Called once per frame.
	RENDER_API virtual bool	EndRender ( void )
		{ return true; }

	// == Setters ==

	// Change the material the given material array thing.
	// Give the ownership of the material to this mesh if the materials have been "released"
	//RENDER_API virtual void			SetMaterial		( RrMaterial* n_pNewMaterial );
	// Change the object's render type
	//RENDER_API void			SetRenderType ( eRenderLayer newRenderType )
	//	{ renderLayer = newRenderType; }
	// Change visible state
	RENDER_API virtual void	SetVisible ( const bool nextState )
		{ visible = nextState; }

	// == Getters ==

	// Searches for the first material with the string in its name
	//RrMaterial*				FindMaterial( const string & strToFind, int skipAmount=0 );
	//RENDER_API virtual RrMaterial*	GetMaterial ( void ) {
	//	return m_material;
	//}
	// Returns visible state
	RENDER_API virtual bool	GetVisible ( void ) const
		{ return visible; }

	// == Object Passes ==
	
	//	PassInitWithInput(pass, passData) : Sets up a new pass on the given slot.
	// Creates a copy of passData without changing reference counts.
	RENDER_API void			PassInitWithInput ( int pass, RrPass* passData );
	RENDER_API void			PassFree ( int pass );
	RENDER_API renderer::cbuffer::rrPerObjectSurface&
							PassGetSurface ( int pass );

	// == Culling/Prerendering Prototypes ==

	//	GetPassNumber : number of passes to add to render list
	// Get the number of passes required to render the model in the current pipeline.
	// When implementing your own overrides, only return the amount of passes for one pipeline.
	// If there is both a deferred and a forward set of shaders, but the pipeline is deferred, only the deferred pass will be used.
	// If this returns zero, the renderer will instead pull directly from the forward pass list in the material.
	/*RENDER_API virtual uchar
							GetPassNumber ( void );
	//	GetPass : Return forward pass info
	// Returns the associated pass. This is used for ordering.
	RENDER_API virtual RrPassForward*
							GetPass ( const uchar pass );
	//	GetPassDeferred : Return deferred pass info
	// Returns the associated deferred rendering pass. This is used for ordering.
	RENDER_API virtual RrPassDeferred*
							GetPassDeferred ( const uchar pass );*/


private:
	// == Update Prototypes ==


	//	UpdateRenderOrderToCamera : generate data needed for sorting
	// Generated data needed for sorting, namely distance from the camera. Is not fast.
	/*void					UpdateRenderOrderToCamera ( RrCamera* camera )
	{
		if ( renderLayer == renderer::kRLV2D )
		{
			renderDistance = ( transform.world.position.z * 50.0f ) + ( ( camera->transform.position - transform.world.position ).sqrMagnitude() * 0.005f );
		}
		else
		{
			renderDistance = ( camera->transform.position - transform.world.position ).sqrMagnitude();
		}
	};*/

public:
	// Positional transform
	core::Transform			transform;
	// miscelleneous render settings
	renderer::objectSettings
							renderSettings;		
	RrObjectMaterialProperties
							shaderConstants;

protected:
	// Rendering States
	//eRenderLayer			renderLayer;
	//vector<RrMaterial*>	vMaterials;
protected:
	//RrMaterial*				m_material;
	bool					visible;
private:
	RrPass					m_passes [kPass_MaxPassCount];
	bool					m_passEnabled [kPass_MaxPassCount];

	//vector<renderer::new_passinfo_t>	m_passinfo;	
	/*uint*		m_vao_info;
	uint		m_vao_count;
	uint		m_vao_maxcount;

	RENDER_API void PassinfoClear ( void );
	RENDER_API void PassinfoGenerate ( void );
	
	RENDER_API void PassinfoRegenerate ( void );*/

	gpu::Pipeline			m_pipelines [kPass_MaxPassCount];
	bool					m_pipelineReady [kPass_MaxPassCount];

	gpu::ConstantBuffer		m_cbufPerObjectMatrices;

protected:
	// ==Render Setup==
	
	// BindVAO()
	// Must be called after the vertex buffer is bound, as will bind materials to render.
	//RENDER_API bool BindVAO ( const uchar pass, const uint vbo, const uint eab=0, const bool userDefinedAttribs=false );
	RENDER_API gpu::Pipeline*
							GetPipeline ( const uchar pass );

	RENDER_API void			FreePipelines ( void );


	RENDER_API void			PushCbufferPerObject ( const XrTransform& worldTransform, const RrCamera* camera );

	// ==Render Status==
	RENDER_API float		GetRenderDistance ( void );

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
	//static Vector3d _activeCameraPosition;
	//static Rotator	_activeCameraRotation;

private:
	// Distance from the render pivot
	float renderDistance;
	// ID values
	uint32_t id;
	void SetId ( unsigned int );
	// Give storage class access to SetId
	friend RrRenderer;
};

// typedef for persons coming from Unity
typedef CRenderableObject RenderObject;

#endif//C_RENDERABLE_OBJECT_