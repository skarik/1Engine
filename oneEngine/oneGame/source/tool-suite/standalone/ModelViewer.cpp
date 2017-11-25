#include "ModelViewer.h"

#include "core/settings/CGameSettings.h"
#include "core/debug/CDebugConsole.h"
#include "core-ext/system/io/Resources.h"
#include "core/input/CInput.h"
#include "core/input/CXboxController.h"

#include "engine/utils/CDeveloperConsole.h"

#include "renderer/camera/CCamera.h"
#include "renderer/object/shapes/CPrimitiveCube.h"
#include "renderer/object/model/Model.h"
#include "engine-common/entities/CRendererHolder.h"

#include "core/math/Math.h"

toolsuite::ModelViewer::ModelViewer ( void )
	: CGameBehavior()
{
	camera = new CCamera;
	camera->fov = 75.0F;
	camera->transform.rotation = Rotator();

	// Create temp cube
	{
		CPrimitiveCube* cube = new CPrimitiveCube(0.5F, 0.5F, 0.5F);
		(new CRendererHolder(cube))->RemoveReference();
	}

	// Create character model
	/*{
		model = new CModel("models/demos/female elf.fbx");
		model->transform.scale = Vector3d(1,1,1) / 304.8F * 2.7F;
		//model->transform.rotation = Vector3d(0.0F, 0, 135.0F);
		//model->transform.position = Vector3d(-1.0F, +1.0F, -1.3F);
		model->transform.position = Vector3d(0.0F, 0.0F, 0.0F);
		//(new CRenderLogicHolder(model))->RemoveReference();
	}*/

	ResetCameraOrientation();
}

toolsuite::ModelViewer::~ModelViewer ( void )
{
	delete_safe(camera);
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
	if (model != NULL)
	{
		cameraCenter = model->GetBoundingBox().GetCenterPoint().mulComponents(model->transform.scale);
	}
}