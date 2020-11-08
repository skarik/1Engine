#ifndef UI_EVENTIDE_USER_INTERFACE_H_
#define UI_EVENTIDE_USER_INTERFACE_H_

#include "../eventide/Common.h"

#include "engine/behavior/CGameBehavior.h"
#include "renderer/logic/RrLogicObject.h"

class RrTexture;

namespace dusk
{
	class UserInterface;
}
namespace dawn
{
	class UserInterface;
}

namespace ui {
namespace eventide {

	struct Texture
	{
		// Eventually index to something but for now, it's just another texture
		RrTexture*				reference = NULL;
		// We use this to pass into params so things get the right texture
		uint32_t				index;
	};

	class Element;

	//	Manager class for the Eventide UI system. Can be automatically created by controls.
	class UserInterface : public CGameBehavior, public RrLogicObject
	{
	public:

		//	Get() : Gets current active manager. Allocates new one if it doesn't yet exist.
		EVENTIDE_API static UserInterface*
								Get ( void );

		//	ReleaseActive() : releases active manager, if any
		EVENTIDE_API static void
								ReleaseActive ( void );

		//	Constructor : creates manager that can defer to the given items
		EVENTIDE_API			UserInterface ( dusk::UserInterface*, dawn::UserInterface* );

		EVENTIDE_API			~UserInterface ( void );

		//	Update() : Per-frame update
		EVENTIDE_API void		Update ( void ) override;

		EVENTIDE_API void		AddElement ( Element* element );
		EVENTIDE_API void		RemoveElement ( Element* element );

		EVENTIDE_API Texture	LoadTexture ( const char* filename );
		EVENTIDE_API Texture	LoadTextureFont ( const char* filename );
		EVENTIDE_API void		ReleaseTexture ( const Texture& texture );

	public:
		dusk::UserInterface*	m_duskUI = NULL;
		dawn::UserInterface*	m_dawnUI = NULL;
		bool					m_shuttingDown = false;

		std::vector<Element*>	m_elements;
		bool					m_elementsDirty = false;

		std::vector<RrTexture*>	m_textures;
	};

}}

#endif//UI_EVENTIDE_USER_INTERFACE_H_