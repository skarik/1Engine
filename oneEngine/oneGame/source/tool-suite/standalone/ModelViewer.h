#ifndef TOOL_SUITE_STANDALONE_MODEL_VIEWER_H_
#define TOOL_SUITE_STANDALONE_MODEL_VIEWER_H_

#include "core/math/Vector3.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine-common/dusk/Handle.h"

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
		Vector3f		cameraCenter;
		// Rotated offset of the camera.
		Vector3f		cameraPanning;
		// Camera's current rotation.
		Rotator			cameraRotation;
		// Camera's persistent velocity for mouse input.
		Vector3f		cameraRotationVelocity;

	private:
		// Dusk UI Elements:

		dusk::Handle	ui_lbl_startHint;

		struct UIBlockMesh
		{
			dusk::Handle	lbl_meshName;
			dusk::Handle	btn_texDiffuse;
			dusk::Handle	btn_texNormals;
			dusk::Handle	btn_texSurface;
			dusk::Handle	btn_texOverlay;
		};

		UIBlockMesh*	ui_meshblocks;
		uint			ui_meshblocksCount;
	};
}

#endif//TOOL_SUITE_STANDALONE_MODEL_VIEWER_H_