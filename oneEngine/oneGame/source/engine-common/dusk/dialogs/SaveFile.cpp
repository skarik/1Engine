#include "SaveFile.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::dialogs::SaveFile::Update ( const UIStepInfo* stepinfo )
{
	FileViewer::Update(stepinfo);
}
void dusk::dialogs::SaveFile::Render ( UIRendererContext* uir )
{
	uir->setFocus(dusk::kFocusStyleAutomatic);
	uir->setColor(dusk::kColorStyleBackground);
	uir->drawRectangle(this, m_absoluteRect);
}