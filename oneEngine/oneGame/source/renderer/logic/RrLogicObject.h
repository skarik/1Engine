// Logic Object class
// base class for all renderer-specific logic objects.
// all step code in them must be thread safe

#ifndef _C_RENDERER_LOGIC_OBJECT_
#define _C_RENDERER_LOGIC_OBJECT_

// Includes
#include "core/types/types.h"
#include "core/math/Vector3.h"
#include "core/containers/araccessor.h"
#include "core-ext/transform/Transform.h"

//#include "GLCommon.h"
//#include "glMainSystem.h"
//#include "renderer/material/RrMaterial.h"

#include "renderer/types/RrObjectMaterialProperties.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/types/id.h"

// Class prototypes
class RrRenderer;
class RrMaterial;
class RrPassForward;
class RrPassDeferred;
class RrWorld;

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
class RrLogicObject
{
private:
	// No copying with "="
	RrLogicObject & operator= (const RrLogicObject & other) = delete;

public:
	//	RrLogicObject (Constructor)
	// Calls to the RenderState to add set and get assigned an ID.
	RENDER_API explicit		RrLogicObject ( void );
	//	~RrLogicObject (Destructor)
	// Calls to the RenderState to force any threaded operation to stop.
	RENDER_API virtual		~RrLogicObject ( void );

	RENDER_API void			AddToWorld ( RrWorld* world );

	// == Step Prototypes ==

	//	PreStep()
	// Executed before the renderer starts. Is guaranteed to be called and finish before any PreRender call.
	// The code within must be thread-safe.
	RENDER_API virtual void	PreStep ( void ) {}
	//	PreStepSynchronus()
	// Executed before the renderer starts. Is guaranteed to finish before PreRender, but may run concurrently with PreStep.
	// The code within does not need to be thread-safe.
	RENDER_API virtual void	PreStepSynchronus ( void ) {}
	//	PostStep()
	// Executed after the frame has rendered (after EndRender, to be exact).
	// Is guaranteed to be called and finish before and PreStep call.
	// The code within must be thread-safe.
	RENDER_API virtual void	PostStep ( void ) {}
	//	PostStepSynchronus()
	// Executed after PostStep jobs requests are started (after EndRender).
	// The code within does not need to be thread-safe.
	RENDER_API virtual void	PostStepSynchronus ( void ) {}


	// == Active Set ==

	//	SetActive()
	// Sets if the current object should have its step queued next frame.
	RENDER_API void			SetActive ( const bool value ) { active = value; }
	//	GetActive()
	// Returns if this object is active and currently is added to the step queue every frame.
	RENDER_API bool			GetActive ( void ) const { return active; }

public:
	// Positional transform
	//CTransform	transform;
	// Character name
	string		name;

protected:
	// Is this logic active
	bool		active;

private:
	rrId id;
public:
	const rrId& GetId ( void ) const
		{ return id; }
	ARACCESSOR_PRIVATE_EXCLUSIVE_SET(RrRenderer, RrLogicObject, rrId, id);
	ARACCESSOR_PRIVATE_EXCLUSIVE_SET(RrWorld, RrLogicObject, rrId, id);
};

// typedef for persons coming from Unity
typedef RrLogicObject LogicObject;

#endif//_C_RENDERER_LOGIC_OBJECT_