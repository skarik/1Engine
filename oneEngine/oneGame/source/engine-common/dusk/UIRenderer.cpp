#include "UIRenderer.h"
#include "engine-common/dusk/UI.h"

Dusk::UIRenderer::UIRenderer (UserInterface* ui)
	: CRenderableObject(),
	m_interface(ui)
{
}
