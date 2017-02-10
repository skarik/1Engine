// Game Renderer class
// Does not take pointer ownership of renderable objects

#ifndef _C_TO_BE_SEEN_H_
#define _C_TO_BE_SEEN_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "renderer/types/ObjectSettings.h"

#include <vector>

// Prototypes
class CRenderableObject;
class CLogicObject;
class CLight;
class CCamera;
class CResourceManager;
class glMaterial;

// Class Definition
class CRenderState
{
public:
	// Structure Definitions
	// ================================
	// Render request
	struct tRenderRequest {
		CRenderableObject*	obj;
		char				pass;
		// should glMaterial be passed in as well? (should renderer handle materials, not objects?)
		//glMaterial*			replacement;
		bool				transparent;
		bool				forward;
		bool				screenshader;
		uchar				renderType;
	};
	// Render material replacement rule
	struct tReplacementRule {
		arstring<32>	hintToReplace;
		glMaterial*		materialToUse;
		// Meaning material settings must be done before the Render() call
	};

public:
	// Constructor and Destructor
	// ================================
	RENDER_API CRenderState ( CResourceManager* nResourceManager );
	RENDER_API ~CRenderState ( void );

	// Public Render routine
	// ================================
	RENDER_API void Render ( void );

	// Full Scene Rendering Routines
	// ================================
	// Normal rendering routine.
	RENDER_API void RenderScene ( const uint32_t n_renderHint );
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
	RENDER_API glMaterial* GetScreenMaterial ( const eRenderMode mode, const Renderer::eSpecialModes mode_type );

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

	// Deferred pass materials
	// ================================
	glMaterial* LightingPass;
	glMaterial* EchoPass;
	glMaterial* ShaftPass;
	glMaterial* Lighting2DPass;
};

// Backwards compatibility
typedef CRenderState CToBeSeen;

// Global instance
RENDER_API extern CRenderState* SceneRenderer;

#endif//_C_TO_BE_SEEN_H_