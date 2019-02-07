//===============================================================================================//
//
//		Dusk UI : class UIRenderer
//
// UIRenderer is the main renderer class. It's instantiated internally by UserInterface.
// All style options are configured through the UIRenderer object.
//
//===============================================================================================//
#ifndef ENGINE_COMMON_DUSK_UI_RENDERER_H_
#define ENGINE_COMMON_DUSK_UI_RENDERER_H_

#include "renderer/object/CRenderableObject.h"

namespace Dusk
{
	class UserInterface;

	class UIRenderer : public CRenderableObject
	{
	public:
		explicit				UIRenderer (UserInterface* ui);
	private:

		UserInterface*		m_interface;
	};
}

#endif//ENGINE_COMMON_DUSK_UI_RENDERER_H_