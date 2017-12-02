#include "renderer/system/glSystem.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/gpuw/Device.h"

//	DeviceSetFillMode( device, fillMode ) : Set device's fill mode.
// Controls how to fill polygons for given device. (glPolygonMode in OpenGL)
// NULL device sets for current active device.
void gpu::DeviceSetFillMode( Device* device, const FillMode fillMode )
{
	if ( fillMode == kFillModeWireframe )
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	else if ( fillMode == kFillModeSolid )
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}