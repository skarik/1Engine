#include "glDrawing.h"

#include "core/system/Screen.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/texture/RrFontTexture.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/state/Settings.h"

// The Global GL drawing object
glDrawing* glDrawing::ActiveSystem = NULL;

// == Constructor and Initialization ==
glDrawing::glDrawing ( void )
{
	ActiveSystem = this;
}
glDrawing::~glDrawing ( void )
{
	if ( ActiveSystem == this )
	{
		ActiveSystem = NULL;
	}
}
// System's current active reference
glDrawing* glDrawing::ActiveReference ( void )
{
	return ActiveSystem;
}

//===============================================================================================//
// Rendering
//===============================================================================================//

// Draw a screen quad
void		glDrawing::DrawScreenQuad ( RrMaterial* n_material )
{
	GL_ACCESS GLd_ACCESS;

	static glHandle vaoQuad = 0;
	static glHandle vboQuad = 0;
	static uchar attribs [16];

	if ( vaoQuad == 0 )
	{
		glGenVertexArrays( 1, &vaoQuad );
		memset( attribs, 0, sizeof(attribs) );
	}
	if ( vboQuad == 0 )
	{
		glGenBuffers( 1, &vboQuad );
		glBindBuffer( GL_ARRAY_BUFFER, vboQuad );

		arModelVertex buffer [4];
		memset(buffer, 0, sizeof(buffer));
		for (arModelVertex& vert : buffer)
		{
			vert.r = 1.0F;
			vert.g = 1.0F;
			vert.b = 1.0F;
			vert.a = 1.0F;
		}

		// Create quad mesh
		buffer[0].x = 1;
		buffer[0].y = 1;
		buffer[0].u = 1;
		buffer[0].v = 1;

		buffer[1].x = -1;
		buffer[1].y = 1;
		buffer[1].u = 0;
		buffer[1].v = 1;

		buffer[2].x = 1;
		buffer[2].y = -1;
		buffer[2].u = 1;
		buffer[2].v = 0;

		buffer[3].x = -1;
		buffer[3].y = -1;
		buffer[3].u = 0;
		buffer[3].v = 0;

		glBufferData( GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW );
	}

	// Push the material
	{
		n_material->prepareShaderConstants();
		//n_material->bindPass(n_pass);
	}

	// Set up attributes
	{
		glBindVertexArray( vaoQuad );
		glBindBuffer( GL_ARRAY_BUFFER, vboQuad );

		// Then disable all values in the VAO (this part of the engine is the only place where they get reused)
		for ( uchar i = 0; i < 16; ++i )
		{
			if ( attribs[i] )
			{	// TODO: DO MATCHING WITH CURRENT MATERIAL.
				attribs[i] = false;
				glDisableVertexAttribArray(i);
			}
		}

		// Bind the vertex attributes
		n_material->bindPassAtrribs();
		// Mark the enabled attributes
		for ( uchar i = 0; i < 16; ++i )
		{
			attribs[i] = RrPassForward::enabled_attributes[i];
		}
	}

	// Draw the current primitive
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
}
