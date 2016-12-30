
//#include "physical/liscensing.cxx" // Include license info

// Includes
#include "core/debug/CDebugConsole.h"
#include "renderer/exceptions/exceptions.h"
#include "renderer/window/COglWindow.h"
#include "glMainSystem.h"

// STD library includes
#include <string>
#include <array>

// Usings
using std::string;

// The Global GL object
//glMainSystem GL;
glMainSystem* glMainSystem::ActiveSystem = NULL;

// == Constructor and Initialization ==
glMainSystem::glMainSystem ( void )
{
	ActiveSystem = this;

	// Extension checking
	ExtensionsReady = false;
	VBOsAvailable = false;
	NPOTsAvailable = false;
	ShadersAvailable = false;
	// GL Values
	MaxTextureSize = 1024;
	MaxVShaderModel = 1.1f;
	MaxPShaderModel = 0.0f;

	// Setup
	OldSchool = false;

	// States
	iCurrentProjectionMode = 0;
	bFogEnabled			= false;
	bDepthWriteEnabled	= true;
}
glMainSystem::~glMainSystem ( void )
{
	if ( ActiveSystem == this )
	{
		ActiveSystem = NULL;
	}
}
// System's current active reference
glMainSystem* glMainSystem::ActiveReference ( void )
{
	return ActiveSystem;
}

// Get what extensions are supported
void glMainSystem::GetExtensions ( void )
{
	const char *str;
	str = (const char *)glGetString( GL_EXTENSIONS );

	typedef std::pair<bool*,std::string> setupPair;
	std::array<setupPair,4> setupTable =
	{
		setupPair(&VBOsAvailable,	"GL_ARB_vertex_buffer_object"),
		setupPair(&NPOTsAvailable,	"GL_ARB_texture_non_power_of_two"),
		setupPair(&ShadersAvailable,	"GL_ARB_shader_objects"),
		setupPair(&FBOsAvailable,	"GL_EXT_framebuffer_object")
	};
	// Start with no features available
	for ( uint i = 0; i < setupTable.size(); ++i )
	{
#	ifdef _ENGINE_DEBUG
		*setupTable[i].first = true;
#	else
		*setupTable[i].first = false;
#	endif
	}

	if ( str != NULL )
	{
		// OpenGL 3.1-
		/*VBOsAvailable		= (strstr((const char *)str, "GL_ARB_vertex_buffer_object") != NULL);
		NPOTsAvailable		= (strstr((const char *)str, "GL_ARB_texture_non_power_of_two") != NULL);
		//ShadersCOREAvailable= (strstr((const char *)str, "GL_shader_objects") != NULL);
		//ShadersARBAvailable	= (strstr((const char *)str, "GL_ARB_shader_objects") != NULL);
		ShadersAvailable	= (strstr((const char *)str, "GL_ARB_shader_objects") != NULL);
		FBOsAvailable	= (strstr((const char *)str, "GL_EXT_framebuffer_object") != NULL);*/
		for ( uint i = 0; i < setupTable.size(); ++i )
		{	// Search the entire string
			if ( strstr( str, setupTable[i].second.c_str() ) != NULL ) {
				*setupTable[i].first = *setupTable[i].first || true;
			}
		}
	}
	else
	{
		// OpenGL 3.3+
		GLint n, i;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);
		// Loop through all the extensions
		for ( i = 0; i < n; i++ )
		{
			str = (const char *)glGetStringi(GL_EXTENSIONS, i);
			if ( str != NULL )
			{
				// And loop through the table
				for ( uint i = 0; i < setupTable.size(); ++i )
				{	// And set up the feature table
					if ( strstr( str, setupTable[i].second.c_str() ) != NULL ) {
						*setupTable[i].first = *setupTable[i].first || true;
					}
				}
			}
		}
	}

	// Print some sexy info
	if ( !VBOsAvailable )
		printf( "Missing VBOs. You must be playing on a rock. Expect errors.\n" );
	if ( !FBOsAvailable )
		printf( "Missing Framebuffer Objects. Expect errors.\n" );
	else
		printf( "Framebuffer Objects found. Expect much rejoicing.\n" );

	// Extensions are now ready
	ExtensionsReady = true;

	// Query max texture size now
	glGetIntegerv( GL_MAX_TEXTURE_SIZE, &MaxTextureSize );

	// If there's no framebuffers nor shaders, expect hell.
	if ( !FBOsAvailable && !ShadersAvailable && !VBOsAvailable ) {
		OldSchool = false;
	}
}

// Load 
bool bOpenGL_InitializeCommonExtentions = false;
void glMainSystem::InitializeCommonExtensions ( void )
{
	if ( !bOpenGL_InitializeCommonExtentions )
	{
		// Load up the actual OpenGL extension
		if ( ogl_LoadFunctions() == ogl_LOAD_FAILED ) {
			throw Core::NullReferenceException();
		}
#		ifdef _WIN32
		if ( wgl_LoadFunctions( COglWindowWin32::pActive->getDevicePointer() ) == ogl_LOAD_FAILED ) {
			throw Core::NullReferenceException();
		}
#		else
		if ( glx_LoadFunctions () == ogl_LOAD_FAILED ) {
			throw Core::NullReferenceException();
		}
#		endif

		// Turn valid extentions into human-readable queries
		GetExtensions();
	}

	bOpenGL_InitializeCommonExtentions = true;
}

#ifndef _ENGINE_RELEASE
void glMainSystem::CheckError ( void )
{
	return;
	GLenum error;
	bool hasError = false;

	// Pop all the errors off the stack to check
	do
	{
		error = glGetError();
		if ( error != GL_NO_ERROR ) {
			hasError = true;
		}

		switch (error)
		{
		case GL_NO_ERROR: break;
		case GL_INVALID_ENUM:
			Debug::Console->PrintError( "gl: GL_INVALID_ENUM\n" );
			//"An unacceptable value is specified for an enumerated argument. The offending function is ignored, having no side effect other than to set the error flag." 
			break;
		case GL_INVALID_VALUE:
			Debug::Console->PrintError( "gl: GL_INVALID_VALUE\n" );
			//"A numeric argument is out of range. The offending function is ignored, having no side effect other than to set the error flag."
			break;
		case GL_INVALID_OPERATION:
			Debug::Console->PrintError( "gl: GL_INVALID_OPERATION\n" );
			//"The specified operation is not allowed in the current state. The offending function is ignored, having no side effect other than to set the error flag."
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			Debug::Console->PrintError( "gl: GL_INVALID_FRAMEBUFFER_OPERATION\n" );
			//"The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag."
			break;
		case GL_OUT_OF_MEMORY:
			Debug::Console->PrintError( "gl: GL_OUT_OF_MEMORY\n" );
			//"There is not enough memory left to execute the function. The state of OpenGL is undefined, except for the state of the error flags, after this error is recorded."
			break;
		//case GL_STACK_UNDERFLOW:
		//	Debug::Console->PrintError( "gl: GL_STACK_UNDERFLOW\n" );
		//	//"This function would cause a stack underflow. The offending function is ignored, having no side effect other than to set the error flag."
		//	break;
		//case GL_STACK_OVERFLOW:
		//	Debug::Console->PrintError( "gl: GL_STACK_OVERFLOW\n" );
		//	//"This function would cause a stack overflow. The offending function is ignored, having no side effect other than to set the error flag."
		//	break;
		//case GL_TABLE_TOO_LARGE:
		//	Debug::Console->PrintError( "gl: GL_TABLE_TOO_LARGE\n" );
		//	//"The specified table exceeds the implementation's maximum supported table size. The offending command is ignored and has no other side effect than to set the error flag."
		//	break;
		default:
			Debug::Console->PrintError( "gl: UNKNOWN ERROR\n" );
			break;
		}
	} while ( error != GL_NO_ERROR );

	// If there's an error, throw exception
	if ( hasError ) {
		throw Renderer::GLStateException();
	}
}
#endif

#ifdef _WIN32
void glMainSystem::SetSwapInterval ( int newSwap )
{
	wglSwapIntervalEXT( newSwap );
}
#endif

// Threading system
void glMainSystem::ThreadGrabLock ( void )
{
	// lock
	mThreadLock.lock();
}
void glMainSystem::ThreadReleaseLock ( void )
{
	// unlock
	mThreadLock.unlock();
}