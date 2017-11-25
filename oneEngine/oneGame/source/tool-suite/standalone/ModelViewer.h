#ifndef TOOL_SUITE_STANDALONE_MODEL_VIEWER_H_
#define TOOL_SUITE_STANDALONE_MODEL_VIEWER_H_

#include "engine/behavior/CGameBehavior.h"

class CCamera;
class CModel;

namespace toolsuite
{
	class ModelViewer : public CGameBehavior
	{
	public:
		explicit		ModelViewer ( void );
						~ModelViewer ( void );

		void			Update ( void ) override;

	private:
		void			UpdateControlsAnalog ( void );
		void			UpdateControlsKeyboard ( void );

		void			ResetCameraOrientation ( void );

	private:
		CCamera*		camera;
		CModel*			model;

		// Position camera orbits around.
		Vector3d		cameraCenter;
		// Rotated offset of the camera.
		Vector3d		cameraPanning;
		// Camera's current rotation.
		Rotator			cameraRotation;
		// Camera's persistent velocity for mouse input.
		Vector3d		cameraRotationVelocity;
	};
}

#endif//TOOL_SUITE_STANDALONE_MODEL_VIEWER_H_