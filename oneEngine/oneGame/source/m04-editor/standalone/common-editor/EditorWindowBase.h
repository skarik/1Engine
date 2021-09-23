#ifndef M04_EDITOR_STANDALONE_COMMON_EDITOR_EDITOR_WINDOW_BASE_H_
#define M04_EDITOR_STANDALONE_COMMON_EDITOR_EDITOR_WINDOW_BASE_H_

#include "engine/behavior/CGameBehavior.h"

#include "m04/eventide/UserInterface.h"
#include "engine-common/dusk/Dusk.h"

class RrWindow;
class RrWorld;
class RrCamera;
class RrRenderObject;
class RrOutputInfo;

namespace m04 {
namespace editor {

	class WindowBase : public CGameBehavior
	{
	protected:
		EDITOR_API explicit		WindowBase ( const Vector2i& position, const Vector2i& size, const char* outputName );
		EDITOR_API				~WindowBase ( void );

		EDITOR_API void			Update ( void ) override;

		EDITOR_API RrOutputInfo&
								GetRenderOutput ( void );
		EDITOR_API const RrOutputInfo&
								GetRenderOutput ( void ) const;

	protected:
		RrWindow*			window = NULL;
		RrWorld*			editor_world = NULL;
		RrCamera*			editor_camera = NULL;

		ui::eventide::UserInterface*
							user_interface = NULL;
		dusk::UserInterface*
							dusk_interface = NULL;
	};

}}

#endif//M04_EDITOR_STANDALONE_COMMON_EDITOR_EDITOR_WINDOW_BASE_H_