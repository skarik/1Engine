
#include "module_renderer.h"
#include "core/legacy/linkerfix_legacy.h"

#include "core/settings/CGameSettings.h"
#include "physical/physics/CPhysics.h"

void ModuleRenderer::Sync ( CGameSettings* _gs, CPhysics* _phys )
{
	ARMODULE_SYNC(CGameSettings,_gs);
	ARMODULE_SYNC(CPhysics,_phys);
}