#ifndef M04_INTERFACES_CAMERA_CONTROLLER_PLATFORMER_H_
#define M04_INTERFACES_CAMERA_CONTROLLER_PLATFORMER_H_

#include "core/math/Math3d.h"

class CCamera;

namespace M04
{
	class CameraControllerPlatformer
	{
	public:
		explicit	CameraControllerPlatformer ( void* owner );
					~CameraControllerPlatformer ( void );

		void		Step ( void );

	public:
		void*		m_owner;
		CCamera*	m_camera;

		Vector3d*	m_tracked_position;
		Vector3d*	m_tracked_velocity;
	};
}

#endif//M04_INTERFACES_CAMERA_CONTROLLER_PLATFORMER_H_