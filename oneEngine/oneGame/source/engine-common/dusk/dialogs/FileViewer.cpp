#include "FileViewer.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::dialogs::FileViewer::Update ( const UIStepInfo* stepinfo )
{
	m_localRect = Rect(100, 100, 500, 300);

	DialogElement::Update(stepinfo);
}
void dusk::dialogs::FileViewer::Render ( UIRendererContext* uir )
{
	uir->setFocus(dusk::kFocusStyleAutomatic);
	uir->setColor(dusk::kColorStyleBackground);
	uir->drawRectangle(this, m_absoluteRect);
}