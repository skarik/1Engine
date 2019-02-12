#include "CameraControllerPlatformer.h"

#include "engine/behavior/CGameBehavior.h"
#include "renderer/camera/RrCamera.h"

M04::CameraControllerPlatformer::CameraControllerPlatformer ( void* owner )
	: m_owner(owner),
	m_camera(NULL), m_tracked_position(NULL), m_tracked_velocity(NULL)
{

}

M04::CameraControllerPlatformer::~CameraControllerPlatformer ( void )
{
	delete_safe(m_camera);
}

void M04::CameraControllerPlatformer::Step ( void )
{
	if (m_tracked_position != NULL)
	{
		m_camera->transform.position.x = m_tracked_position->x;
		m_camera->transform.position.y = m_tracked_position->y;
	}
}