
#ifndef _ENGINE_COMMON_ENGINE_COMMON_SCENES_H_
#define _ENGINE_COMMON_ENGINE_COMMON_SCENES_H_

#include "core/types/types.h"
#include "engine/server.h"
#include "engine/utils/CDeveloperConsole.h"

//===============================================================================================//
// Engine-common-scenes
// 
// engine-common functionality defined in header that refer to the scene management system
//===============================================================================================//

#include <vector>
#include <string>
#include <algorithm>
#include <functional>

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
	typedef std::pair<std::string,std::function<void(void)>> _sceneEntry_t;
	ENGCOM_API extern std::vector<_sceneEntry_t> _sceneListing;

	//	RegisterScene()
	// Registers a scene instantiation and swap method with the internal list used for LoadScene.
	template <class Scene>
	void RegisterScene ( const std::string& sceneName )
	{
		_sceneListing.push_back(
			_sceneEntry_t (
				sceneName,
				[] () {
					CGameScene::SceneGoto( CGameScene::NewScene<Scene> () );
					Network::Host();
				}
			)
		);
		engine::Console->AddConsoleMatch( "scene "+sceneName );
	}

};


#endif//_ENGINE_COMMON_ENGINE_COMMON_SCENES_H_