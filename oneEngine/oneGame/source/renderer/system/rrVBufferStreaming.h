#ifndef GL_V_BUFFER_STREAMING_H_
#define GL_V_BUFFER_STREAMING_H_

#include "core/types/types.h"

class RrMaterial;

// Used by main system to store temporary objects.
struct rrVBufferStreaming
{
public:
	unsigned int	drawMode;
	unsigned int	vboObject;	// VBO object
	unsigned int	vaoObject;	// VAO object
	unsigned char	enabledAttributes [16]; // Attributes being used
	bool			inUse;
	unsigned int	elementCount;
	RrMaterial*		material;

	rrVBufferStreaming ( void )
		: drawMode(0), vboObject(0), vaoObject(0),
		/*enabledAttributes(),*/
		inUse(false), elementCount(0),
		material(NULL)
	{
		//enabledAttributes = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
		memset( enabledAttributes, 0, sizeof(unsigned char)*16 );
	}
};

#endif//GL_V_BUFFER_STREAMING_H_