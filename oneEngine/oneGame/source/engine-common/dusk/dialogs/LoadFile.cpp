#include "LoadFile.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::dialogs::LoadFile::Update ( const UIStepInfo* stepinfo )
{
	SaveFile::Update(stepinfo);
}
void dusk::dialogs::LoadFile::Render ( UIRendererContext* uir )
{
	SaveFile::Render(uir);
}