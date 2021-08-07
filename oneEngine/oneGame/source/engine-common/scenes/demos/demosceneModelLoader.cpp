#include "demosceneModelLoader.h"

#include "core/settings/CGameSettings.h"
#include "core/debug/Console.h"
#include "core-ext/system/io/Resources.h"

#include "engine/utils/CDeveloperConsole.h"

#include "renderer/camera/RrCamera.h"
#include "renderer/object/shapes/RrShapeCube.h"
#include "renderer/object/model/Model.h"

#include "engine-common/entities/CPlayer.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
#include "engine-common/entities/CRendererHolder.h"

void demosceneModelLoader::LoadScene ( void )
{
	debug::Console->PrintMessage( "Demoscene: Model Loader\n" );

	// Hide the cursor
	ActiveCursor->SetVisible(false);

	// Create camera
	/*{
		RrCamera* cam = new RrCamera();
		cam->transform.position = Vector3f(-3.0F, +3.0F, 1.0F);
		cam->transform.rotation = Rotator(Vector3f(0, -10.0F, 45.0F));
		cam->SetActive();
		(new CRenderCameraHolder(cam))->RemoveReference();
	}*/

	// Create player
	{
		CPlayer* player = new CPlayer();
		player->transform.world.position = Vector3f(-3.0F, +3.0F, 1.0F);
		player->transform.world.rotation = Rotator(Vector3f(0, -10.0F, 45.0F));
		player->RemoveReference();
	}

	// Create tmep cube
	{
		RrShapeCube* cube = new RrShapeCube();
		cube->transform.local.scale = Vector3f(0.5F, 0.5F, 0.5F); // 0.5F across
		(new CRendererHolder(cube))->RemoveReference();
	}

	// Create acorn
	{
		RrCModel* model = RrCModel::Load("models/demos/female elf.fbx", NULL);
		model->transform.scale = Vector3f(1,1,1) / 304.8F * 2.7F;
		model->transform.rotation = Vector3f(0.0F, 0, 135.0F);
		//model->transform.position = Vector3f(-1.0F, +1.0F, -1.3F);
		model->transform.position = Vector3f(0.0F, 0.0F, 0.0F);
		(new CRenderLogicHolder(model))->RemoveReference();
	}


	debug::Console->PrintMessage( "You are running a test of 1Engine.\n" );
	debug::Console->PrintMessage( "This test is meant to show both the basic 3D rendering capabilities and model IO.\n" );
}