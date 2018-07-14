#include "renderer/gpuw/GraphicsContext.h"

#include "renderer/ogl/GLCommon.h"
#include "renderer/types/types.h"

//	DeviceSetFillMode( device, fillMode ) : Set device's fill mode.
// Controls how to fill polygons for given device. (glPolygonMode in OpenGL)
// NULL device sets for current active device.
int gpu::GraphicsContext::setFillMode( const FillMode fillMode )
{
	if ( fillMode == kFillModeWireframe )
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	else if ( fillMode == kFillModeSolid )
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

int gpu::GraphicsContext::setViewport ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom )
{
	glViewport( left, top, right-left, bottom-top );
	return 0;
}

int gpu::GraphicsContext::setScissor ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom )
{
	glScissor( left, top, right-left, bottom-top );
	return 0;
}
