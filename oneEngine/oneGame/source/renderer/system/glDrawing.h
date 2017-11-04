#ifndef GL_DRAWING_H_
#define GL_DRAWING_H_

// Need the standard types
#include "core/types/types.h"
#include "core-ext/transform/Transform.h"
#include "core/math/Color.h"
// Needs the OpenGL functions
#include "renderer/ogl/GLCommon.h"

// Streaming mesh data class
#include "rrVBufferStreaming.h"
// For the vertex class
#include "core/types/ModelData.h"
// Misc types
#include "renderer/types/types.h"

#define GLd_ACCESS glDrawing& GLd = *glDrawing::ActiveReference();

class glDrawing
{
public:
	//	Constructor
	// System initialization
	glDrawing ( void );
	~glDrawing ( void );

private:
	// Current instance of the system
	static glDrawing* ActiveSystem;
public:
	// System's current active reference
	RENDER_API static glDrawing* ActiveReference ( void );

public:
	// Draw a screen quad
	RENDER_API void		DrawScreenQuad ( RrMaterial* n_material );
};

#endif//GL_DRAWING_H_