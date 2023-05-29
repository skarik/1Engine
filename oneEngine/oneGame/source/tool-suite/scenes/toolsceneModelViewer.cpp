#include "toolsceneModelViewer.h"

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

#include "tool-suite/standalone/ModelViewer.h"

void toolsceneModelViewer::LoadScene ( void )
{
	// Show the cursor
	ActiveCursor->SetVisible(true);

	// Create camera
	/*{
		CCamera* cam = new CCamera();
		cam->transform.position = Vector3d(-3.0F, +3.0F, 1.0F);
		cam->transform.rotation = Rotator(Vector3d(0, -10.0F, 45.0F));
		cam->SetActive();
		(new CRenderCameraHolder(cam))->RemoveReference();
	}*/

	// Create player
	/*{
		CPlayer* player = new CPlayer();
		player->transform.world.position = Vector3d(-3.0F, +3.0F, 1.0F);
		player->transform.world.rotation = Rotator(Vector3d(0, -10.0F, 45.0F));
		player->RemoveReference();
	}

	// Create temp cube
	{
		CPrimitiveCube* cube = new CPrimitiveCube(0.5F, 0.5F, 0.5F);
		(new CRendererHolder(cube))->RemoveReference();
	}

	// Create character model
	{
		CModel* model = new CModel("models/demos/female elf.fbx");
		model->transform.scale = Vector3d(1,1,1) / 304.8F * 2.7F;
		model->transform.rotation = Vector3d(0.0F, 0, 135.0F);
		//model->transform.position = Vector3d(-1.0F, +1.0F, -1.3F);
		model->transform.position = Vector3d(0.0F, 0.0F, 0.0F);
		(new CRenderLogicHolder(model))->RemoveReference();
	}*/

	// Create modelviewer tool:
	{
		toolsuite::ModelViewer* modelViewer = new toolsuite::ModelViewer();
		modelViewer->RemoveReference();
	}

	debug::Console->PrintMessage( "You are running a test of 1Engine.\n" );
	debug::Console->PrintMessage( "This test is meant to show both the basic 3D rendering capabilities and model IO.\n" );
}