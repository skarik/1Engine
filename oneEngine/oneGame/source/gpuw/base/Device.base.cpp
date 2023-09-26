#include "gpuw/base/Device.base.h"

static gpu::base::Device* m_TargetDisplayDevice = NULL;

gpu::base::Device* gpu::getDevice ( void )
{
	return m_TargetDisplayDevice;
}
void gpu::setActiveDevice ( gpu::base::Device* device )
{
	m_TargetDisplayDevice = device;
}