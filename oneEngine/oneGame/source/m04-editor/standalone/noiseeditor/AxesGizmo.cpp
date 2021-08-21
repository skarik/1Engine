#include "core/system/Screen.h"
#include "core/input/CInput.h"
#include "renderer/camera/RrCamera.h"
#include "m04/eventide/UserInterface.h"
//#include "./Constants.h"
//#include "./SequenceEditor.h"

#include "AxesGizmo.h"

m04::editor::noise::AxesGizmo::AxesGizmo ( ui::eventide::UserInterface* ui )
	: Element( ui )
	//, main_editor(editor)
{
	m_frameUpdate = FrameUpdate::kPerFrame;
	m_mouseInteract = MouseInteract::kNone;
}

m04::editor::noise::AxesGizmo::~AxesGizmo ( void )
{
}

void m04::editor::noise::AxesGizmo::BuildMesh ( void )
{
	ParamsForCube cubeParams;

	cubeParams = {};
	cubeParams.box = core::math::Cubic::ConstructCenterExtents(Vector3f(0, 0, 0), Vector3f(16.0F, 1.0F, 1.0F));
	cubeParams.color = Color(0.75F, 0.0F, 0.0F, 1.0F);
	buildCube(cubeParams);

	cubeParams = {};
	cubeParams.box = core::math::Cubic::ConstructCenterExtents(Vector3f(0, 0, 0), Vector3f(1.0F, 16.0F, 1.0F));
	cubeParams.color = Color(0.0F, 0.5F, 0.0F, 1.0F);
	buildCube(cubeParams);

	cubeParams = {};
	cubeParams.box = core::math::Cubic::ConstructCenterExtents(Vector3f(0, 0, 0), Vector3f(1.0F, 1.0F, 16.0F));
	cubeParams.color = Color(0.0F, 0.0F, 1.0F, 1.0F);
	buildCube(cubeParams);
}

void m04::editor::noise::AxesGizmo::OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame )
{
	; // Nothing
}