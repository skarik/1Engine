
#ifndef GL_MAIN_SYSTEM_H_
#define GL_MAIN_SYSTEM_H_

// Need the standard types
#include "core/types/types.h"
#include "core/math/Color.h"
#include "core-ext/transform/CTransform.h"
// Needs the OpenGL functions
#include "renderer/ogl/GLCommon.h"
#include "renderer/types/types.h"
#include "renderer/types/textureFormats.h"
#include "renderer/types/blendModes.h"
//#include "renderer/state/Settings.h"

// Need some C Standard functions
#include <cstdio>
#include <cstdarg>
// Need some C++ types
#include <vector>
#include <list>
// Need threading locks
#include <mutex>

//#include "glExtentions.h"

// Extension checking
/*extern bool glVBOsAvailable;
extern bool glNPOTsAvailable;
// GL Values
extern int	glMaxTextureSize;

// Function Definitions
void OpenGL_InitializeCommonExtentions ( void );
void OpenGL_SetSwapInterval ( int );
*/

#define GL_ACCESS glMainSystem& GL = *glMainSystem::ActiveReference();

// Streaming mesh data class
//#include "glVBufferStreaming.h"
// For the vertex class
//#include "core/types/ModelData.h"
class CRenderTexture;

class glMainSystem
{
public:
	//	Constructor
	// System initialization
	glMainSystem ( void );
	~glMainSystem ( void );

private:
	// Current instance of the system
	static glMainSystem* ActiveSystem;
public:
	// System's current active reference
	RENDER_API static glMainSystem* ActiveReference ( void );

public:
	//==============================================================================================//
	// FEATURES AVAILABLE
	//==============================================================================================//
	bool		ExtensionsReady;
	bool		VBOsAvailable;
	bool		NPOTsAvailable;		// Should this still be supported?
	bool		ShadersAvailable;
	bool		FBOsAvailable;
	int			MaxTextureSize;
	float		MaxVShaderModel;
	float		MaxPShaderModel;
	bool		OldSchool;			// Are we in a non-shader system?

public:
	//==============================================================================================//
	// INITIALIZATION
	//==============================================================================================//

	// Load up OpenGL into memory
	void		InitializeCommonExtensions ( void );
	void		GetExtensions ( void );

	//==============================================================================================//
	// FRAME SETUP
	//==============================================================================================//

	// Prepare system for a new frame
	void		BeginFrame ( void );
	// Cleanup after old frame
	void		EndFrame ( void );
	// Present frame
	void		Present ( void );

	//==============================================================================================//
	// 
	//==============================================================================================//

	// Get the current main screen buffer
	RENDER_API CRenderTexture* GetMainScreenBuffer ( void );
	// Tell the entire system to restart
	RENDER_API void		FullRedraw ( void );

	// Push matrix state
	RENDER_API void		prepareDraw ( void );
	// Pop matrix state
	RENDER_API void		cleanupDraw ( void );

	RENDER_API void		setupFog ( void );
	RENDER_API void		setupAmbient ( void );

	// Set Windows sync option
	void		SetSwapInterval ( int );

	//==============================================================================================//
	// THREADING
	//==============================================================================================//

	//	ThreadGrabLock
	RENDER_API void		ThreadGrabLock ( void );
	RENDER_API void		ThreadReleaseLock ( void );

	//==============================================================================================//
	// VIEWPORT AND PROJECTION SETUP
	//==============================================================================================//

	RENDER_API void		setupViewport ( int x, int y, int width, int height );

	RENDER_API void		beginOrtho ( void );
	RENDER_API void		beginOrtho ( ftype left, ftype top, ftype width, ftype height, ftype minz, ftype maxz, bool flipped=true );
	RENDER_API void		endOrtho ( void );
	RENDER_API bool		inOrtho ( void );

	RENDER_API void		pushProjection ( const Matrix4x4& );
	RENDER_API void		popProjection ( );

	RENDER_API const Matrix4x4& getProjection ( void );

	RENDER_API bool		hasModelMatrix ( void );
	RENDER_API void		pushModelMatrix ( const Matrix4x4& );
	RENDER_API void		popModelMatrix ( void );

	RENDER_API const Matrix4x4& getModelMatrix ( void );

#ifdef _ENGINE_RELEASE
	RENDER_API FORCE_INLINE void CheckError ( void ) {}
#else
	RENDER_API void		CheckError ( void );
#endif

	// == Transformations and lazy transforms ==
	RENDER_API void		Transform ( CTransform* );
	RENDER_API void		Transform ( const Matrix2x2& );
	RENDER_API void		Transform ( const Matrix4x4& );

	RENDER_API void		Translate ( Vector3d const& );
	RENDER_API void		Translate ( Vector2d const& );
	RENDER_API void		Rotate ( float );

	//==============================================================================================//
	// CURRENT RENDER STATES
	//==============================================================================================//

	// Fog
	RENDER_API bool		FogEnabled ( void );
	RENDER_API void		DisableFog ( void );
	RENDER_API void		EnableFog ( void );

	// Buffer write type
	RENDER_API void		clearColor( void );
	RENDER_API void		clearColor( const Color& );

	RENDER_API bool		DepthWriteEnabled ( void );
	RENDER_API void		EnableDepthWrite ( void );
	RENDER_API void		DisableDepthWrite ( void );

	RENDER_API void		BlendMode( const eBlendModes src, const eBlendModes dest );

private:
	std::list<Matrix4x4>	viewprojection_stack;
	std::list<Matrix4x4>	modelmatrix_stack;

public:
	// == Screen and Pixels ==
	RENDER_API void		ClearBuffer ( unsigned int bitmask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	RENDER_API void		UpdateBuffer ( void );
	
	RENDER_API Color		GetPixel ( int x, int y );
	RENDER_API float*		GetPixels( int x, int y, int w, int h );

public:
	//==============================================================================================//
	// FORMATS (specific to openGL)
	//==============================================================================================//

	RENDER_API glEnum		Enum ( const eColorFormat );
	RENDER_API glEnum		Enum ( const eTextureType );
	RENDER_API glEnum		Enum ( const eDepthFormat );
	RENDER_API glEnum		Enum ( const eStencilFormat );
	RENDER_API glEnum		Enum ( const eWrappingType );
	RENDER_API glEnum		Enum ( const eSamplingFilter );

	//==============================================================================================//
	// TEXTURES
	//==============================================================================================//

	/*glHandle		GetNewTexture( );
	void			FreeTexture( glHandle );

	void			TextureSetWrapping( const glHandle, const eWrappingType wrapX, const eWrappingType wrapY );
	void			TextureSetWrapping( const glHandle, const eWrappingType wrapX, const eWrappingType wrapY, const eWrappingType wrapZ );*/

	//==============================================================================================//
	// BUFFERS
	//==============================================================================================//

	RENDER_API void			CreateBuffer ( uint* index, uint count = 1 );
	RENDER_API void			FreeBuffer ( uint* index, uint count = 1 );
	RENDER_API void			BindBuffer ( uint target, uint index );
	RENDER_API void			UploadBuffer ( uint target, uint size, void* data, uint usage );
	RENDER_API void			UnbindBuffer ( uint target );

	RENDER_API void			BindVertexArray ( uint target );

	//==============================================================================================//
	// RENDERING FROM BUFFER
	//==============================================================================================//

	RENDER_API void			DrawElements ( uint type, int count, uint data, void* offset );

private:
	unsigned int	iCurrentProjectionMode;

	bool	bFogEnabled;
	bool	bDepthWriteEnabled;

	std::mutex		mThreadLock;
};

//extern glMainSystem GL;

#endif