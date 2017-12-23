#ifndef TOOL_SUITE_STANDALONE_MODEL_VIEWER_H_
#define TOOL_SUITE_STANDALONE_MODEL_VIEWER_H_

#include "engine/behavior/CGameBehavior.h"
#include "engine-common/dusk/CDuskGUIHandle.h"

class CCamera;
class CModel;
class CRenderableObject;
class CDuskGUI;

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
		void			ResetCameraCentering ( void );

		//		uiCreate () : create the dusk UI
		// create entirety of the dusk gui shit
		void			uiCreate ( void );

		//		uiUpdate () : dusk UI logic
		// performs input & other toggle logic
		void			uiUpdate ( void );

	private:
		CCamera*		camera;
		CModel*			model;
		CRenderableObject*	cube;
		CDuskGUI*		dusk;

		// Position camera orbits around.
		Vector3d		cameraCenter;
		// Rotated offset of the camera.
		Vector3d		cameraPanning;
		// Camera's current rotation.
		Rotator			cameraRotation;
		// Camera's persistent velocity for mouse input.
		Vector3d		cameraRotationVelocity;

	private:
		// Dusk UI Elements:

		Dusk::Handle	ui_lbl_startHint;

		struct UIBlockMesh
		{
			Dusk::Handle	lbl_meshName;
			Dusk::Handle	btn_texDiffuse;
			Dusk::Handle	btn_texNormals;
			Dusk::Handle	btn_texSurface;
			Dusk::Handle	btn_texOverlay;
		};

		UIBlockMesh*	ui_meshblocks;
		uint			ui_meshblocksCount;
	};
}

#endif//TOOL_SUITE_STANDALONE_MODEL_VIEWER_H_