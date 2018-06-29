#include "core/settings/CGameSettings.h"
#include "core/debug/CDebugConsole.h"

#include "core-ext/system/io/Resources.h"
#include "core-ext/system/shell/Status.h"

#include "renderer/system/glMainSystem.h"

#include "RrShader.h"
#include "RrShaderManager.h"

using namespace std;

// takes a shader filename or system built-in as an argument
RrShader::RrShader ( const char* a_sShaderName, const renderer::rrShaderTag a_nShaderTag, const bool a_bCompileOnDemand )
{
	sShaderFilename = a_sShaderName;
	bCompileOnDemand= a_bCompileOnDemand;
	stTag			= a_nShaderTag;

	primed = false;

	RrShader* pResult = ShaderManager.ShaderExists( a_sShaderName, a_nShaderTag );
	if ( pResult == NULL )
	{
		bIsReference = false;
		pParentShader = NULL;

		ShaderManager.AddShader( this );

		sVertexShader = NULL;
		sPixelShader = NULL;

		// == Set the default settings ==
		// shader
		iVertexShaderID = 0;
		iPixelShaderID	= 0;

		iProgramID		= 0;

		stShaderType	= None;
		bIsCompiled		= false;
		bHasCompileError= false;

		iCompileResult	= SCE_NOTCOMPILED;

		// memory
		iRefNumber	= 1;

		// == Create the shader ==
		// Open the shader
		open_shader();
		// Parse the shader into a readable format
		parse_shader();
		// Compile the shader
		if ( !bCompileOnDemand )
			compile_shader();
	}
	else
	{
		// Get the shader reference
		bIsReference = true;
		pParentShader = pResult;

		// Increment references to parent
		pParentShader->AddReference();

		// Change shader contents to reflect a reference
		sVertexShader = NULL;
		iVertexShaderLength = 0;
		sPixelShader = NULL;
		iPixelShaderLength = 0;
	}
}

// Yeah, not sure.
RrShader::~RrShader ( void )
{
	// Probably should remove the OpenGL program

	//ShaderManager.RemoveShader( this );

	// Check if there's references, post a warning if there is.
	if ( iRefNumber > 0 )
	{
		debug::Console->PrintWarning( "WARNING: Shader being deleted with active references!\n" );
	}
}

// Get Uniform Block location
int RrShader::getUniformBlockLocation ( const char* name )
{
	if ( !bIsReference )
	{
		int uniformLocation = glGetUniformBlockIndex( iProgramID, name );
		return uniformLocation;
	}
	else
	{
		return pParentShader->getUniformBlockLocation( name );
	}
}

// == Drawing Program Controls ==
void RrShader::begin ( void )
{
	// If the shader is a reference, begin it's parent instead
	if ( bIsReference )
	{
		pParentShader->begin();
		return;
	}

	// If the shader is set to compile on first use, compile it now if this is the first use
	if ( bCompileOnDemand )
		if ( !bIsCompiled )
			compile_shader();

	// Don't start the program if there's a compile error
	if ( !bHasCompileError )
	{
		// Use the program
		glUseProgram( iProgramID );
	}
	else
	{
		// Alert user that a shader compile failed, especially if we're working in silent mode
		core::shell::SetTaskbarProgressState(NIL, core::shell::kTaskbarStateError);

		// Bind default program
		glUseProgram( 0 );
	}
}
void RrShader::end ( void )
{
	// Reset the program being used
	glUseProgram( 0 );
}

void RrShader::prime ( void )
{
	primed = true;
}
bool RrShader::isPrimed ( void )
{
	return primed;
}

// == Memory Management ==
// Increment number of references
void RrShader::AddReference ( void )
{
	if ( !bIsReference )
		iRefNumber++;
	else
		pParentShader->AddReference();
}
// Decrement number of references
void RrShader::DecrementReference ( void )
{
	if ( !bIsReference )
	{
		if ( iRefNumber == 0 )
		{
			cout << "DECREMENTING BELOW ZERO. THIS IS IMPOSSIBLE. (" << bIsReference << ") " << endl;
			return;
		}
		iRefNumber--;
	}
	else
	{
		pParentShader->DecrementReference();
	}
}
// Returns number of references around. Job of user to properly manage this number.
unsigned int RrShader::ReferenceCount ( void )
{
	return iRefNumber;
}
// Frees the shader
void RrShader::ReleaseReference ( void )
{
	DecrementReference();
	bool result = ShaderManager.RemoveShader( this );

#	ifdef _ENGINE_DEBUG
	if ( result )
	{
		cout << " -parent shader is destroyed." << endl;
	}
#	endif
}
// Increament number of references
void RrShader::GrabReference ( void )
{
	AddReference();
}

// == Shader Construction ==
// Recompile
bool RrShader::recompile ( void )
{
	//mUniformMap.clear();

	if ( bIsCompiled )
	{
		if ( sVertexShader )
			delete [] sVertexShader;
		if ( sPixelShader )
			delete [] sPixelShader;

		// Probably should remove the OpenGL program
	}

	sVertexShader = NULL;
	sPixelShader = NULL;

	// == Set the default settings ==
	// shader
	iVertexShaderID = 0;
	iPixelShaderID	= 0;

	iProgramID		= 0;

	stShaderType	= None;
	bIsCompiled		= false;
	bHasCompileError= false;

	iCompileResult	= SCE_NOTCOMPILED;

	// == Create the shader ==
	// Open the shader
	open_shader();
	// Parse the shader into a readable format
	parse_shader();
	// Compile the shader
	if ( !bCompileOnDemand )
		compile_shader();

	return true;
}

#include <sys/stat.h>
static long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}
#include "core/utils/string.h"
#include "core/system/io/FileUtils.h"
// This loads the shader from the file and into the sRawShader string
void RrShader::open_shader ( void )
{
	// Based on the file extention, get the shader type.
	string sExtension = core::utils::string::GetLower( core::utils::string::GetFileExtension( sShaderFilename ) );

	if (( sExtension == "glsl" )||( sExtension == "frag" )||( sExtension == "vert" )||( sExtension == "fp" )||
		( sExtension == "vp" )||( sExtension == "geom" )||( sExtension == "gl" ))
	{
		stShaderType = GLSL;
	}
	else
	{
		cout << " Warning on shader \"" << sShaderFilename << "\" : only GLSL shaders supported." << endl;
	}

	// Open the file.
	ifstream inFile ( sShaderFilename.c_str() );

	if ( !inFile.is_open() )
	{
		bool fail = false;
		// Then we assume that it's in two separate shader files

		// Generate the shader filenames
		string sVertFilename = sShaderFilename.substr( 0,sShaderFilename.length()-sExtension.length() );
		string sFragFilename = sVertFilename + "frag";
		// Vertex shader filname may need a skinning variant
		if ( stTag == renderer::SHADER_TAG_DEFAULT )
		{
			sVertFilename = sVertFilename + "vert";
			sVertFilename = core::Resources::PathTo( sVertFilename );
		}
		else if ( stTag == renderer::SHADER_TAG_SKINNING )
		{
			string tsVertFilename = sVertFilename + "skinning.vert";
			tsVertFilename = core::Resources::PathTo( tsVertFilename );
			if (!IO::FileExists( tsVertFilename )) {
				debug::Console->PrintWarning( "WARNING: COULD NOT FIND SKINNING TARGET FOR SHADER '" + sVertFilename + "'\n" );
				sVertFilename = sVertFilename + "vert";
			}
			else {
				sVertFilename = tsVertFilename;
			}
		}
		else
		{
			throw std::invalid_argument ( "Invalid stTag" );
		}

		// Look for both files
		sVertFilename = core::Resources::PathTo( sVertFilename );
		if (!IO::FileExists( sVertFilename )) {
			fail = true;
		}
		sFragFilename = core::Resources::PathTo( sFragFilename );
		if (!IO::FileExists( sFragFilename )) {
			fail = true;
		}
		
		if ( fail )
		{
			iCompileResult |= SCE_NOFILE;
			cout << "File '" << sShaderFilename << "' not found!" << endl;
			cout << " backup '" << sFragFilename << "' and '" << sVertFilename << "' not found" << endl;
		}
		else
		{
			std::streamoff length;
			unsigned int i;
			//unsigned uiSize;

			// get length of file:
			length = GetFileSize( sFragFilename );
			inFile.open( sFragFilename.c_str(), ios::in|ios::binary ); //|ios::ate
			if ( !inFile.is_open() )
				cout << "CANNOT OPEN FILE HALP HALP HALP" << endl;
			/*inFile.seekg( 0, std::ios::end );
			length = inFile.tellg();*/
			inFile.seekg( 0, std::ios::beg );
			inFile.clear();
			//cout << "PIXEL SHADER LENGTH: " << length << endl;
			// Just in case
			//uiSize = static_cast<unsigned>(length);
			//cout << length << endl;
			iPixelShaderLength = static_cast<unsigned>(length);
			sPixelShader = new char[iPixelShaderLength+8];
			// read data as a block:
			//cout << "Reading in " << sFragFilename << " instead" << endl;
			i = 0;
			while ( inFile.good() )
			{
				sPixelShader[i] = inFile.get();       // get character from file.
				if ( !inFile.eof() )
					i++;
			}
			iPixelShaderLength = i;
			//cout << "LOADED PIXEL SHADER LENGTH: " << iPixelShaderLength << endl;
			//inFile.read (sPixelShader, length);
			sPixelShader[iPixelShaderLength] = 0;
			inFile.close();

			// get length of file:
			length = GetFileSize( sVertFilename );
			inFile.open( sVertFilename.c_str(), ios::in|ios::binary ); //|ios::ate
			if ( !inFile.is_open() )
				cout << "SWEET HEAVENS BAZZA WHAT DID YOU DO?!?" << endl;
			/*inFile.seekg( 0, std::ios::end );
			length = inFile.tellg();*/
			inFile.seekg( std::ios::beg );
			inFile.clear();
			//cout << "VERTEX SHADER LENGTH: " << length << endl;
			// Just in case
			//uiSize = static_cast<unsigned>(length);
			iVertexShaderLength = static_cast<unsigned>(length);
			sVertexShader = new char[iVertexShaderLength+8];
			// read data as a block:
			//cout << "Reading in " << sVertFilename << " instead" << endl;
			//inFile.read (sVertexShader, length);
			i = 0;
			while ( inFile.good() )
			{
				sVertexShader[i] = inFile.get();       // get character from file.
				if ( !inFile.eof() )
					i++;
			}
			iVertexShaderLength = i;
			//cout << "LOADED VERTEX SHADER LENGTH: " << iVertexShaderLength << endl;
			sVertexShader[iVertexShaderLength] = 0;
			inFile.close();
		}
	}
	else
	// Copy the data from the file into the sRawShader string.
	{
		// get length of file:
		inFile.seekg (0, std::ios::end);
		std::streamoff length = inFile.tellg();
		inFile.seekg (0, std::ios::beg);
		inFile.clear();
		// Just in case
		//assert(length < UINT32_MAX);
		unsigned uiSize = static_cast<unsigned>(length);
		char* szBuffer = new char[uiSize+8];
		// read data as a block:
		inFile.read (szBuffer, length);
		inFile.close();
		// Put stuff into the string
		sRawShader = string( szBuffer, uiSize );
		delete[] szBuffer;
	}
}
// This takes the data in sRawShader and outputs proper shaders into the vertex and pixel shader strings
void RrShader::parse_shader ( void )
{
	// Parse the shader differently based on stShaderType.
}
// This compiles the shaders and adds them to a program object
void RrShader::compile_shader ( void )
{
	GL_ACCESS;
	int compiled = 0;
	int blen = 0;	
	int slen = 0;
	//GLchar * psTempShader;

	if ( !CGameSettings::Active()->b_ro_EnableShaders )
	{
		bHasCompileError = true;
		return;
	}

	// This is just shader compile. Standard.

	// Create vertex shader and compile
	iVertexShaderID = glCreateShader(GL_VERTEX_SHADER);

	if ( sVertexShader != NULL )
	{
		glShaderSource( iVertexShaderID, 1, (const char**)&sVertexShader, &iVertexShaderLength );
		glCompileShader( iVertexShaderID );

		// Check for error in the compiling
		glGetShaderiv( iVertexShaderID, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			debug::Console->PrintMessage("Compile error in vertex shader.\n");
			glGetShaderiv( iVertexShaderID, GL_INFO_LOG_LENGTH , &blen ); 
			if ( blen > 1 )
			{
				GLchar* compiler_log = (GLchar*)malloc(blen);
				glGetShaderInfoLog( iVertexShaderID, blen, &slen, compiler_log );
				std::cout << "Filename: " << sShaderFilename << "\n";
				debug::Console->PrintError( "vertex shader compiler_log:\n" );
				debug::Console->PrintError( compiler_log );
				free ( compiler_log );
			}
			bHasCompileError = true;
			iCompileResult |= SCE_VERTCOMPILE;
		}
#ifdef _ENGINE_DEBUG
		else
		{
			glGetShaderiv( iVertexShaderID, GL_INFO_LOG_LENGTH , &blen ); 
			if ( blen > 1 )
			{
				GLchar* compiler_log = (GLchar*)malloc(blen);
				glGetShaderInfoLog( iVertexShaderID, blen, &slen, compiler_log );
				std::cout << "Filename: " << sShaderFilename << "\n";
				debug::Console->PrintWarning( "vertex shader compiler_log:\n" );
				debug::Console->PrintWarning( compiler_log );
				free ( compiler_log );
			}
		}
#endif
	}
	else
	{
		bHasCompileError = true;
	}

	// Create pixel shader and compile
	iPixelShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	if ( sPixelShader != NULL )
	{
		glShaderSource( iPixelShaderID, 1, (const char**)&sPixelShader, &iPixelShaderLength );
		glCompileShader( iPixelShaderID );

		// Check for error in the compiling
		glGetShaderiv( iPixelShaderID, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			debug::Console->PrintMessage( "Compile error in fragment shader.\n" );
			glGetShaderiv( iPixelShaderID, GL_INFO_LOG_LENGTH , &blen ); 
			if ( blen > 1 )
			{
				GLchar* compiler_log = (GLchar*)malloc(blen);
				glGetShaderInfoLog( iPixelShaderID, blen, &slen, compiler_log );
				std::cout << "Filename: " << sShaderFilename << "\n";
				debug::Console->PrintError( "fragment shader compiler_log:\n" );
				debug::Console->PrintError( compiler_log );
				free ( compiler_log );
			}
			bHasCompileError = true;
			iCompileResult |= SCE_FRAGCOMPILE;
		}
#ifdef _ENGINE_DEBUG
		else
		{
			glGetShaderiv( iPixelShaderID, GL_INFO_LOG_LENGTH , &blen ); 
			if ( blen > 1 )
			{
				GLchar* compiler_log = (GLchar*)malloc(blen);
				glGetShaderInfoLog( iPixelShaderID, blen, &slen, compiler_log );
				std::cout << "Filename: " << sShaderFilename << "\n";
				debug::Console->PrintWarning( "fragment shader compiler_log:\n" );
				debug::Console->PrintWarning( compiler_log );
				free ( compiler_log );
			}
		}
#endif
	}

	bIsCompiled = !bHasCompileError;

	// Create the program object if successfully compiled
	if ( bIsCompiled )
	{
		iProgramID = glCreateProgram();
		if ( iProgramID == 0 ) {
			throw std::exception();
		}

		if ( (iCompileResult&SCE_VERTCOMPILE) == 0 )	// Attach vertex shader if it compiled
			glAttachShader( iProgramID, iVertexShaderID );
		if ( (iCompileResult&SCE_FRAGCOMPILE) == 0 )	// Attach pixel shader if it compiled
			glAttachShader( iProgramID, iPixelShaderID );

		// Create the attribute link points
		for ( uint i = 0; i < sizeof(renderer::AttributeNames)/sizeof(renderer::rrAttributeReservedName); ++i )
		{
			glBindAttribLocation( iProgramID, renderer::AttributeNames[i].id, renderer::AttributeNames[i].token );
		}

		// Link the program
		glLinkProgram( iProgramID );

		GLint linked = 0;
		glGetProgramiv( iProgramID, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			std::cout << "Error in shader program link.\n";
			glGetProgramiv( iProgramID, GL_INFO_LOG_LENGTH , &blen ); 
			if ( blen > 1 )
			{
				GLchar* linker_log = (GLchar*)malloc(blen);
				glGetProgramInfoLog( iProgramID, blen, &slen, linker_log );
				std::cout << "Filename: " << sShaderFilename << "\n";
				debug::Console->PrintError( "program linker_log:\n" );
				debug::Console->PrintError( linker_log );
				free( linker_log );
			}
			bHasCompileError = true;
			iCompileResult |= SCE_PROGRAM;
		}   
	}
	
	// Output errors if the compiling or linking didn't work
	if ( bHasCompileError )
	{
		// Alert user that a shader compile failed, especially if we're working in silent mode
		core::shell::SetTaskbarProgressState(NIL, core::shell::kTaskbarStateError);
		core::shell::FlashTray(NIL, 5);

		// Print out failure
		cout << "In \"" << sShaderFilename << "\" : Shader creation failure. Here's the failures:\n";
		// Print out all the compile error types
		for ( unsigned char i = 0; i < 8; i += 1 )
		{
			unsigned int mask = (1<<i);
			if ( (iCompileResult&mask) > 0 )
			{
				switch ( mask )
				{
					case SCE_NOTCOMPILED:
						cout << "  Shader is not compiled." << endl;
						break;
					case SCE_NOFILE:
						cout << "  Could not open shader file." << endl;
						break;
					case SCE_VERTCOMPILE:
						cout << "  Failure in vertex shader compile." << endl;
						break;
					case SCE_FRAGCOMPILE:
						cout << "  Failure in pixel/fragment shader compile." << endl;
						break;
					case SCE_PROGRAM:
						cout << "  Failure in OpenGL program creation or shader linking." << endl;
				}
			}
			// End if
		}
		// End loop
	}
	
	// Got this far. Let's update bindings.
	createConstantBufferBindings();
}

//	createConstantBufferBindings() : Internal post-compile step, grab engine refs.
// Locates constant buffer bindings and links them to their index
void RrShader::createConstantBufferBindings ( void )
{
	auto kEntryCount = sizeof(renderer::CBufferNames) / sizeof(renderer::rrCBufferReservedName);
	for (unsigned i = 0; i < kEntryCount; ++i)
	{
		int uboIndex = getUniformBlockLocation( renderer::CBufferNames[i].token );
		if (uboIndex >= 0)
		{
			glUniformBlockBinding( get_program(), uboIndex, (GLuint)renderer::CBufferNames[i].id );
		}
	}
}