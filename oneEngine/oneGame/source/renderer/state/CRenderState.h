// Game Renderer class
// Does not take pointer ownership of renderable objects

#ifndef _C_TO_BE_SEEN_H_
#define _C_TO_BE_SEEN_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "renderer/types/types.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/types/RrGpuTexture.h"
#include "renderer/state/InternalSettings.h"

#include <vector>

class CRenderableObject;
class CLogicObject;
class CLight;
class CCamera;
class CResourceManager;
class RrMaterial;
class CRenderTexture;
class CMRTTexture;

//	CRenderState : main render state & swapchain manager class
class CRenderState
{
public:
	// Structure Definitions
	// ================================

	//	tRenderRequest : Render request
	struct tRenderRequest
	{
		CRenderableObject*	obj;
		char				pass;
		bool				transparent;
		bool				forward;
		bool				screenshader;
		uchar				renderType;

		// Default constructor
		tRenderRequest ( void ) :
			obj(NULL), pass(0), transparent(false), forward(true), screenshader(false), renderType(2)
		{ ; }
	};

	//	tReplacementRule : Render material replacement rule
	// Material settings must be done before the Render() call
	struct tReplacementRule
	{
		arstring<32>	hintToReplace;
		RrMaterial*		materialToUse;
	};

	//	rrInternalBufferChain : internal storage of buffers used for rendering
	struct rrInternalBufferChain
	{
		glHandle		buffer_depth;
		glHandle		buffer_stencil;
		CRenderTexture*	buffer_forward_rt;

		CMRTTexture*	buffer_deferred_mrt;
		CRenderTexture*	buffer_deferred_rt;
	};

public:
	// Constructor and Destructor
	// ================================

	RENDER_API CRenderState ( CResourceManager* nResourceManager );
	RENDER_API ~CRenderState ( void );


	// Buffer management
	// ================================

	RENDER_API void CreateBuffer ( void );
	// Recreates buffers for the given chain
	RENDER_API void CreateBufferChain ( rrInternalBufferChain& bufferChain );
	RENDER_API CRenderTexture* GetForwardBuffer ( void );
	RENDER_API CRenderTexture* GetDeferredBuffer ( void );
	RENDER_API RrGpuTexture GetDepthTexture ( void );
	RENDER_API RrGpuTexture GetStencilTexture ( void );


	// Public Render routine
	// ================================

	RENDER_API void Render ( void );


	// Full Scene Rendering Routines
	// ================================

	// Normal rendering routine.
	RENDER_API void RenderSceneForward ( const uint32_t n_renderHint );
	// Deferred rendering routine.
	RENDER_API void RenderSceneDeferred ( const uint32_t n_renderHint );


	// Specialized Rendering Routines
	// ================================

	RENDER_API void PreRenderSetLighting ( std::vector<CLight*> & lightsToUse );
	// RenderSingleObject renders an object, assuming the projection has been already set up.
	RENDER_API void RenderSingleObject ( CRenderableObject* objectToRender );
	// RenderObjectArray() renders a null terminated list of objects, assuming the projection has been already set up.
	RENDER_API void RenderObjectArray ( CRenderableObject** objectsToRender );


	// Rendering configuration
	// ================================

	// Returns the material used for rendering a screen's pass in the given effect
	RENDER_API RrMaterial* GetScreenMaterial ( const eRenderMode mode, const renderer::eSpecialModes mode_type );


	// Settings and query
	// ================================

	// Returns internal settings that govern the current render setup
	RENDER_API const renderer::internalSettings_t& GetSettings ( void ) const;

private:
	bool bSpecialRender_ResetLights;
	std::vector<CLight*> vSpecialRender_LightList;

public:
	// Public active instance pointer
	static CRenderState* Active;
	// Resource manager
	CResourceManager*	mResourceManager;

private:
	// Renderable object access and management
	// ================================

	// Give RO constructor and destructor access to adding and removing
	friend CRenderableObject;
	// Adding and removing renderable objects
	unsigned int AddRO ( CRenderableObject* );
	void RemoveRO ( unsigned int );
	void CleanROList ( void );
	// Reorder the render list
	void ReorderList ( void );

	// Give RO constructor and destructor access to adding and removing
	friend CLogicObject;
	// Adding and removing renderable objects
	unsigned int AddLO ( CLogicObject* );
	void RemoveLO ( unsigned int );

private:
	// Render list
	// ================================
	std::vector<CRenderableObject*> pRenderableObjects;
	unsigned int iCurrentIndex;
	unsigned int iListSize;

	// Render list sorting
	// ================================
	struct structRenderComparison_old {
		bool operator() ( CRenderableObject* i, CRenderableObject* j);
	} RenderOrderComparison_old;
	struct render_forward_comparator_t {
		bool operator() ( tRenderRequest& i, tRenderRequest& j);
	} OrderComparatorForward;
	struct render_deferred_comparator_t {
		bool operator() ( tRenderRequest& i, tRenderRequest& j);
	} OrderComparatorDeferred;

	// Render state
	// ================================
	CCamera*	mainBufferCamera;

	// Logic list
	// ================================
	std::vector<CLogicObject*> mLogicObjects;
	unsigned int mLoCurrentIndex;
	unsigned int mLoListSize;

	// Internal setup state
	// ================================
	renderer::internalSettings_t	internal_settings;
	std::vector<rrInternalBufferChain>	internal_chain_list;
	rrInternalBufferChain*				internal_chain_current;
	uint								internal_chain_index;

	/*glHandle						internal_buffer_depth;
	glHandle						internal_buffer_stencil;
	CRenderTexture*					internal_buffer_forward_rt;

	CMRTTexture*					internal_buffer_deferred_mrt;
	CRenderTexture*					internal_buffer_deferred_rt;*/

	// Deferred pass materials
	// ================================
	RrMaterial*	CopyScaled;
	RrMaterial* LightingPass;
	RrMaterial* EchoPass;
	RrMaterial* ShaftPass;
	RrMaterial* Lighting2DPass;
};

// Global instance
RENDER_API extern CRenderState* SceneRenderer;

#endif//_C_TO_BE_SEEN_H_