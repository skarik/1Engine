#include "ModelViewer.h"

#include "core/settings/CGameSettings.h"
#include "core/debug/CDebugConsole.h"
#include "core/math/Math.h"
#include "core/input/CInput.h"
#include "core/input/CXboxController.h"
#include "core/system/io/FileUtils.h"
#include "core/utils/StringUtils.h"

#include "core-ext/system/io/Resources.h"
#include "core-ext/system/shell/DragAndDrop.h"

#include "engine/utils/CDeveloperConsole.h"

#include "renderer/camera/CCamera.h"
#include "renderer/object/shapes/CPrimitiveCube.h"
#include "renderer/object/model/Model.h"
#include "renderer/object/mesh/CMesh.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/texture/CBitmapFont.h"

#include "engine-common/entities/CRendererHolder.h"
#include "engine-common/dusk/CDuskGUI.h"

toolsuite::ModelViewer::ModelViewer ( void )
	: CGameBehavior()
{
	camera = new CCamera;
	camera->fov = 75.0F;
	camera->transform.rotation = Rotator();

	// Create temp cube
	{
		cube = new CPrimitiveCube(1.0F, 1.0F, 1.0F);
		RrMaterial* mat = RrMaterial::Default->copy();
		cube->SetMaterial(mat);
		mat->removeReference();
	}

	for (std::string cmd : CGameSettings::Active()->m_cmd)
	{
		printf("\"%s\"\n", cmd.c_str());
	}

	// Load in command line
	string filename = "";//"models/demos/female elf.fbx";
	auto& cmd_list = CGameSettings::Active()->m_cmd;
	if ( !cmd_list.empty() )
	{
		string temp_filename = IO::FilenameStandardize(cmd_list[0]);
		temp_filename = temp_filename.substr(temp_filename.find_last_of("/"));
		temp_filename = temp_filename.substr(0, temp_filename.find_last_of("."));
		temp_filename = ".game/" + temp_filename + ".fbx";
		if ( IO::CopyFileTo(temp_filename.c_str(), cmd_list[0].c_str()) )
		{
			filename = temp_filename;
		}
		else
		{
			debug::Console->PrintError("Could not copy file to temporary directory.");
		}
	}

	// Create character model
	if (filename.length() > 1)
	{
		model = new CModel( filename.c_str() );
		model->transform.scale = Vector3d(1,1,1);
		model->transform.position = Vector3d(0.0F, 0.0F, 0.0F);

		// Create a material for each mesh
		for (uint i = 0; i < model->GetMeshCount(); ++i)
		{
			RrMaterial* material = RrMaterial::Default->copy();

			model->GetMesh(i)->SetMaterial(NULL);
			model->GetMesh(i)->SetMaterial(material);

			material->removeReference();
		}
	}
	else
	{
		model = NULL;
	}

	// Create UI
	{
		dusk = new DuskGUI(
			new CBitmapFont( "YanoneKaffeesatz-R.otf", 21 )
		);
		dusk->SetPixelMode(true);
		uiCreate();
	}

	ResetCameraOrientation();
}

toolsuite::ModelViewer::~ModelViewer ( void )
{
	delete_safe(camera);
	delete_safe(model);
	delete_safe(cube);
	delete_safe_decrement(dusk);
}

void toolsuite::ModelViewer::Update ( void )
{
	UpdateControlsAnalog();
	UpdateControlsKeyboard();

	// Limit the depth-panning
	cameraPanning.x = math::clamp(cameraPanning.x, -5.0F, -1.0F);
	
	// Set final location
	camera->transform.position = cameraCenter + cameraRotation * cameraPanning;
	camera->transform.rotation = cameraRotation;

	core::shell::arDragAndDropEntry dndEntry;
	while (core::shell::PopDragAndDropEntry(dndEntry))
	{
		string extension = StringUtils::ToLower(StringUtils::GetFileExtension(dndEntry.filename.c_str()));
		if (extension == "fbx" || extension == "pad")
		{
			// Load up new model (move to separate function w/ hotspots)
			delete_safe(model);
			model = new CModel(dndEntry.filename.c_str());
			model->transform.scale = Vector3d(1,1,1);
			model->transform.position = Vector3d(0.0F, 0.0F, 0.0F);
			
			// Create a material for each mesh
			for (uint i = 0; i < model->GetMeshCount(); ++i)
			{
				RrMaterial* material = RrMaterial::Default->copy();

				model->GetMesh(i)->SetMaterial(NULL);
				model->GetMesh(i)->SetMaterial(material);

				material->removeReference();
			}

			// Reset camera to center without changing rotation
			ResetCameraCentering();
		}
		else if (extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "bpd")
		{
			// Load up new texture (move to separate function w/ hotspots)
			/*CTexture* tex = new CTexture(dndEntry.filename.c_str());
			cube->GetMaterial()->setTexture(TEX_DIFFUSE, core::Orphan(tex));

			for (uint i = 0; i < model->GetMeshCount(); ++i)
			{
				model->GetMesh(i)->GetMaterial()->setTexture(TEX_DIFFUSE, tex);
			}*/

			if (model != NULL)
			{
				// If it's a texture, check positioning, and loop through the UI elements
				for (uint i = 0; i < model->GetMeshCount(); ++i)
				{
					CTexture* tex = new CTexture(dndEntry.filename.c_str());
					Vector2d mouse_pos ((Real)dndEntry.point.x, (Real)dndEntry.point.y);

					if (ui_meshblocks[i].btn_texDiffuse.GetRect().Contains(mouse_pos))
					{
						model->GetMesh(i)->GetMaterial()->setTexture(TEX_DIFFUSE, core::Orphan(tex));
					}
					if (ui_meshblocks[i].btn_texNormals.GetRect().Contains(mouse_pos))
					{
						model->GetMesh(i)->GetMaterial()->setTexture(TEX_NORMALS, core::Orphan(tex));
					}
					if (ui_meshblocks[i].btn_texSurface.GetRect().Contains(mouse_pos))
					{
						model->GetMesh(i)->GetMaterial()->setTexture(TEX_SURFACE, core::Orphan(tex));
					}
					if (ui_meshblocks[i].btn_texOverlay.GetRect().Contains(mouse_pos))
					{
						model->GetMesh(i)->GetMaterial()->setTexture(TEX_OVERLAY, core::Orphan(tex));
					}
				}
			}
			//
		}
	}

	// Update UI:
	uiUpdate();
}

void toolsuite::ModelViewer::UpdateControlsAnalog ( void )
{
	Input::xboxControl->UpdateState();

	// Grab analog input from controller 
	Vector3f analogRotationInput = Input::xboxControl->GetAnalog(InputControl::kAnalogIndexLeftStick);
	Vector3f analogPanningInput  = Input::xboxControl->GetAnalog(InputControl::kAnalogIndexRightStick);
	Vector3f analogZoomingInput  = Input::xboxControl->GetAnalog(InputControl::kAnalogIndexTriggers);
	// Modify input curve
	for (int i = 0; i < 3; ++i)
	{
		analogRotationInput[i] = powf(fabsf(analogRotationInput[i]), 2.0F) * math::sgn(analogRotationInput[i]);
		analogPanningInput[i]  = powf(fabsf(analogPanningInput[i]),  2.0F) * math::sgn(analogPanningInput[i]);
		analogZoomingInput[i]  = powf(fabsf(analogZoomingInput[i]),  2.0F) * math::sgn(analogZoomingInput[i]);
	}
	// Update the panning and rotation based on the controller input.
	cameraPanning += Vector3f(
		fabsf(cameraPanning.x) * (analogZoomingInput.x - analogZoomingInput.y) * 1.41F,
		0.0F, /*fabsf(cameraPanning.x) *  analogPanningInput.x,*/
		0.0F /*fabsf(cameraPanning.x) * -analogPanningInput.y*/) * Time::deltaTime;
	cameraCenter += cameraRotation *Vector3f(
		0.0F,
		fabsf(cameraPanning.x) *  analogPanningInput.x,
		fabsf(cameraPanning.x) * -analogPanningInput.y ) * Time::deltaTime;
	cameraRotation *= Rotator(Vector3f(0, analogRotationInput.y, analogRotationInput.x) * Time::deltaTime * 360.0F);

	// Input to reset camera?
	if ( (Input::xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0 )
	{
		ResetCameraOrientation();
	}
}
void toolsuite::ModelViewer::UpdateControlsKeyboard ( void )
{
	// Grab mouse input...
	Vector3f mouseMotionInput = Vector3f(Input::DeltaMouseX(), Input::DeltaMouseY(), 0);

	// TODO: Apply filter and add velocity.
	if ( Input::Mouse(Input::MBLeft) || Input::Mouse(Input::MBRight) )
	{

	}

	// Zoom in and out for both buttons held.
	if ( Input::Mouse(Input::MBLeft) && Input::Mouse(Input::MBRight) )
	{
		cameraPanning += Vector3f(
			fabsf(cameraPanning.x) *  mouseMotionInput.y * 1.41F,
			0.0F,
			0.0F) * 0.002F;
	}
	// Rotate around target when left mouse button used.
	else if ( Input::Mouse(Input::MBLeft) )
	{
		cameraRotation *= Rotator(Vector3f(0, -mouseMotionInput.y, mouseMotionInput.x) * 0.5F);
	}
	// Pan target when right mouse button used.
	else if ( Input::Mouse(Input::MBRight) )
	{
		/*cameraPanning += Vector3f(
			0.0F,
			fabsf(cameraPanning.x) * mouseMotionInput.x,
			fabsf(cameraPanning.x) * mouseMotionInput.y)  * 0.002F;*/
		cameraCenter += cameraRotation * Vector3f(
			0.0F,
			fabsf(cameraPanning.x) * mouseMotionInput.x,
			fabsf(cameraPanning.x) * mouseMotionInput.y)  * 0.002F;
	}

	// Input to reset camera?
	if ( Input::Key('Z') )
	{
		ResetCameraOrientation();
	}
}

void toolsuite::ModelViewer::ResetCameraOrientation ( void )
{
	cameraCenter = Vector3d(0,0,0);
	cameraPanning = Vector3d(-3,0,0);
	cameraRotation = Rotator();
	cameraRotationVelocity = Vector3d();

	// Move the camera center back to the model's center.
	ResetCameraCentering();
}

void toolsuite::ModelViewer::ResetCameraCentering ( void )
{
	if (model != NULL)
	{
		cameraCenter = model->GetBoundingBox().GetCenterPoint().mulComponents(model->transform.scale);
	}
}


void toolsuite::ModelViewer::uiCreate ( void )
{
	ui_lbl_startHint = dusk->CreateText(-1, "To get started, drag an FBX or PAD file in.");
	ui_lbl_startHint.SetRect( Rect( (Screen::Info.width - 256) * 0.5F, Screen::Info.height * 0.75F, 256, 24 ) );

	ui_meshblocks = NULL;
	ui_meshblocksCount = 0;
}

void toolsuite::ModelViewer::uiUpdate ( void )
{
	if (model != NULL)
	{
		// Hide the hint:
		ui_lbl_startHint.SetVisible(false);

		// Create the needed mesh blocks for the UI:
		if (ui_meshblocksCount != model->GetMeshCount())
		{
			UIBlockMesh* new_listing = new UIBlockMesh[model->GetMeshCount()];
			uint copy_count = std::min<uint>(ui_meshblocksCount, model->GetMeshCount());
			uint create_count = model->GetMeshCount();
			for (uint i = 0; i < copy_count; ++i)
			{
				new_listing[i] = ui_meshblocks[i];
			}
			for (uint i = copy_count; i < create_count; ++i)
			{
				new_listing[i].lbl_meshName = dusk->CreateText(-1, "temp");
				new_listing[i].btn_texDiffuse = dusk->CreateButton(-1);
				new_listing[i].btn_texNormals = dusk->CreateButton(-1);
				new_listing[i].btn_texOverlay = dusk->CreateButton(-1);
				new_listing[i].btn_texSurface = dusk->CreateButton(-1);
			}
			for (uint i = create_count; i < ui_meshblocksCount; ++i)
			{
				dusk->DeleteElement(ui_meshblocks[i].lbl_meshName);
				dusk->DeleteElement(ui_meshblocks[i].btn_texDiffuse);
				dusk->DeleteElement(ui_meshblocks[i].btn_texNormals);
				dusk->DeleteElement(ui_meshblocks[i].btn_texOverlay);
				dusk->DeleteElement(ui_meshblocks[i].btn_texSurface);
			}
			delete[] ui_meshblocks;
			ui_meshblocks = new_listing;
			ui_meshblocksCount = model->GetMeshCount();
		}

		// Set the blocks:
		for (uint i = 0; i < ui_meshblocksCount; ++i)
		{
			float ybase = 20.0F + 100.0F * i;

			ui_meshblocks[i].lbl_meshName.SetText( model->GetMesh(i)->GetName() );
			ui_meshblocks[i].lbl_meshName.SetRect( Rect( 10.0F, ybase - 6.0F, 256.0F, 18.0F ) );

			ui_meshblocks[i].btn_texDiffuse.SetText( "Diffuse" );
			ui_meshblocks[i].btn_texDiffuse.SetRect( Rect( 10.0F, ybase + 20.0F, 75.0F, 75.0F ) );

			ui_meshblocks[i].btn_texNormals.SetText( "Normals" );
			ui_meshblocks[i].btn_texNormals.SetRect( Rect( 10.0F + 80.0F, ybase + 20.0F, 75.0F, 75.0F ) );

			ui_meshblocks[i].btn_texOverlay.SetText( "Overlay" );
			ui_meshblocks[i].btn_texOverlay.SetRect( Rect( 10.0F + 80.0F*2, ybase + 20.0F, 75.0F, 75.0F ) );

			ui_meshblocks[i].btn_texSurface.SetText( "Surface" );
			ui_meshblocks[i].btn_texSurface.SetRect( Rect( 10.0F + 80.0F*3, ybase + 20.0F, 75.0F, 75.0F ) );
		}
	}
	else
	{
		// Show the hint:
		ui_lbl_startHint.SetVisible(true);
	}
}