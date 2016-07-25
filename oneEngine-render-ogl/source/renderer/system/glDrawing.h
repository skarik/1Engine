
#ifndef GL_DRAWING_H_
#define GL_DRAWING_H_

// Need the standard types
#include "core/types/types.h"
#include "core-ext/transform/CTransform.h"
#include "core/math/Color.h"
// Needs the OpenGL functions
#include "renderer/ogl/GLCommon.h"

// Streaming mesh data class
#include "glVBufferStreaming.h"
// For the vertex class
#include "core/types/ModelData.h"

#define GLd_ACCESS glDrawing& GLd = *glDrawing::ActiveReference();

class glDrawing
{
public:
	//	Constructor
	// System initialization
	glDrawing ( void );

private:
	// Current instance of the system
	static glDrawing* ActiveSystem;
public:
	// System's current active reference
	RENDER_API static glDrawing* ActiveReference ( void );

public:
	int32_t		vertexCount;
	int32_t		triangleCount;

public:
	// Marks all used primitive buffers as not in use
	void MarkBuffersFreeUsage ( void );

	// == Functions that Actually Draw Things ==
	RENDER_API void		DrawText ( float, float, const char*, ... );
	RENDER_API void		DrawTextP ( int, int, const char*, ... );

	RENDER_API void		DrawAutoText ( float, float, const char*, ... );
	RENDER_API void		DrawAutoTextCentered ( float, float, const char*, ... );
	RENDER_API void		DrawAutoTextWrapped ( float, float, float, const char*, ... );

	RENDER_API Real		GetAutoTextWidth ( const char*, ... );

	// Primitive 2D shape settings
	enum e2DDrawMode {
		D2D_WIRE	= 0,
		D2D_FLAT	= 1,
	};
	RENDER_API void		DrawSet2DMode ( e2DDrawMode drawMode = D2D_WIRE );

	enum e2DScaleMode {
		SCALE_DEFAULT	= 0,
		SCALE_WIDTH		= 1,
		SCALE_HEIGHT	= 2,
	};
	RENDER_API void		DrawSet2DScaleMode ( e2DScaleMode drawMode = SCALE_DEFAULT );

	enum e2DRoundingMode {
		RND_DONT_ROUND	= 0,
		RND_ROUND		= 1,
	};
	RENDER_API void		DrawSet2DRounding ( e2DRoundingMode roundMode = RND_DONT_ROUND );

	// OpenGL set color
	//void		SetColor ( const Color& newColor );
	//void		SetColor ( const Real& red, const Real& green, const Real& blue, const Real& alpha=1 );

	// Draw an Autorectangle
	RENDER_API void		DrawRectangleA ( float x, float y, float w, float h );
	RENDER_API void		DrawRectangleATex ( float x, float y, float w, float h, float tx, float ty, float tw, float th );
	// Draw a pixel rectangle
	RENDER_API void		DrawRectangle( float x, float y, float w, float h );
	RENDER_API void		DrawRectangleTex( float x, float y, float w, float h, float tx, float ty, float tw, float th );

	// Draw an Autocircle
	RENDER_API void		DrawCircleA ( float x, float y, float r );
	// Draw a pixel circle
	RENDER_API void		DrawCircle ( float x, float y, float r );

	// Draw an Autoline
	RENDER_API void		DrawLineA ( float x1, float y1, float x2, float y2 );
	// Draw a pixel line
	RENDER_API void		DrawLine ( float x1, float y1, float x2, float y2 );

	// Draw a screen quad
	RENDER_API void		DrawScreenQuad ( void );

	// Set primitive options
	RENDER_API void		SetLineWidth	( const Real width=1.0f );

	// Begin drawing a primitive. This doesn't not actually draw the primitive, but only starts it.
	RENDER_API void		BeginPrimitive ( unsigned int n_primitiveType );

	RENDER_API void		P_AddVertex		( const Vector3d& n_position );
		RENDER_API void	P_AddVertex		( const ftype& n_x, const ftype& n_y, const ftype& n_z )	{ P_AddVertex( Vector3d(n_x,n_y,n_z) ); }
		RENDER_API void	P_AddVertex		( const ftype& n_x, const ftype& n_y )						{ P_AddVertex( Vector3d(n_x,n_y,0) ); }
	RENDER_API void		P_PushTexcoord	( const Vector3d& n_texcoord );
		RENDER_API void	P_PushTexcoord	( const ftype& n_x, const ftype& n_y, const ftype& n_z )	{ P_PushTexcoord( Vector3d(n_x,n_y,n_z) ); }
		RENDER_API void	P_PushTexcoord	( const ftype& n_x, const ftype& n_y )						{ P_PushTexcoord( Vector3d(n_x,n_y,0) ); }
	RENDER_API void		P_PushColor		( const Color& n_color );
		RENDER_API void	P_PushColor		( const ftype& n_r, const ftype& n_g, const ftype& n_b, const ftype& n_a )	{ P_PushColor( Color(n_r,n_g,n_b,n_a) ); }
		RENDER_API void	P_PushColor		( const ftype& n_r, const ftype& n_g, const ftype& n_b )					{ P_PushColor( Color(n_r,n_g,n_b,1) ); }
	RENDER_API void		P_PushNormal	( const Vector3d& n_normal );
		RENDER_API void	P_PushNormal	( const ftype& n_x, const ftype& n_y, const ftype& n_z )	{ P_PushNormal( Vector3d(n_x,n_y,n_z) ); }
	RENDER_API void		P_PushTangent	( const Vector3d& n_tangent );
		RENDER_API void	P_PushTangent	( const ftype& n_x, const ftype& n_y, const ftype& n_z )	{ P_PushTangent( Vector3d(n_x,n_y,n_z) ); }

	// Sends the primitive created with the last BeginPrimitive call to the device.
	RENDER_API void		EndPrimitive ( void );

private:
	// The following are used for keeping track of the model created with the BeginPrimitive...EndPrimitive calls.
	Vector3d	prim_next_texcoord;
	Vector3d	prim_next_normal;
	Vector3d	prim_next_tangent;
	Color		prim_next_color;

	CModelVertex*	prim_vertex_list;

	std::vector<glVBufferStreaming>	prim_list;
	std::vector<glVBufferStreaming>	prim_list_swap;
	unsigned int	prim_count;

private:
	unsigned int	iPrim2DDrawMode;
	unsigned int	iPrim2DScalMode;
	unsigned int	iPrim2DRoundMode;
};

//extern glDrawing GLd;

#endif//GL_DRAWING_H_