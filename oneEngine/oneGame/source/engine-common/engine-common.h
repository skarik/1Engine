#ifndef _ENGINE_COMMMON_ENGINE_COMMON_H_
#define _ENGINE_COMMMON_ENGINE_COMMON_H_

//		engine-common
// Headers and code reusable across projects.
//
// Relies on the following:
//	core
//	core-ext
//	audio
//	renderer
//	engine
//

#include "core/types/types.h"

//===============================================================================================//
//	ENGINE-COMMON INITIALIZATION
//===============================================================================================//

//===============================================================================================//
//	EngineCommonInitialize
//
// After the main engine components have been started, this function is called.
// It adds the default engine bindings to the console, Lua system, and other systems.
//===============================================================================================//
ENGCOM_API	int EngineCommonInitialize ( void );



//===============================================================================================//
//	ENGINE-COMMON FUNCTIONALITY
//===============================================================================================//

//===============================================================================================//
//	namespace EngineCommon
//
// Contains global functions useful across engines.
//===============================================================================================//
namespace EngineCommon
{
	//	LoadScene(scene name)
	// Given an input, loads the given scene.
	// Also performs necessary network setup calls.
	// Returns 0 on success.
	ENGCOM_API int LoadScene ( const std::string& sceneName );

	//	RegisterScene()
	// Registers a scene instantiation and swap method with the internal list used for LoadScene.
	template <class Scene>
	void RegisterScene ( const std::string& sceneName );
};


#endif//_ENGINE_COMMMON_ENGINE_COMMON_H_