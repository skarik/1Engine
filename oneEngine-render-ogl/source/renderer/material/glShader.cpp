
#include "glShader.h"
#include "glShaderManager.h"
#include "core/settings/CGameSettings.h"
#include "core/debug/CDebugConsole.h"
#include "core-ext/system/io/Resources.h"

#include "renderer/system/glMainSystem.h"

using namespace std;

// == Constructor ==
// takes a shader filename or system built-in as an argument
//glShader::glShader ( const string& a_sShaderName, bool a_bCompileOnDemand )
glShader::glShader ( const string& a_sShaderName, const GLE::shader_tag_t a_nShaderTag, const bool a_bCompileOnDemand )
{
	sShaderFilename = a_sShaderName;
	bCompileOnDemand= a_bCompileOnDemand;
	stTag			= a_nShaderTag;

	glShader* pResult = ShaderManager.ShaderExists( a_sShaderName, a_nShaderTag );
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

// == Destructor ==
// Yeah, not sure.
glShader::~glShader ( void )
{
	// Probably should remove the OpenGL program

	//ShaderManager.RemoveShader( this );

	// Check if there's references, post a warning if there is.
	if ( iRefNumber > 0 )
	{
		Debug::Console->PrintWarning( "WARNING: Shader being deleted with active references!\n" );
	}
}

// == Uniform Grabbing ==
int	glShader::get_uniform_location ( const char* name )
{
	if ( !bIsReference )
	{
		/*if ( mUniformMap.count( name ) )
		{
			return mUniformMap[name];
		}
		else*/
		arstring<128> cname ( name );
		unordered_map<arstring<128>,int>::iterator result = mUniformMap.find( cname );
		if ( result != mUniformMap.end() )
		{
			return result->second;
		}
		else
		{
			int uniformLocation = glGetUniformLocation( iProgramID, name );
			mUniformMap[cname] = uniformLocation;
			if ( uniformLocation < 0 && CGameSettings::Active()->b_dbg_ro_ShowMissingLinks )
			{
#ifdef _ENGINE_DEBUG
				cout << "Warning in: " << this << ": can't find shader uniform '" << name << "'" << endl;
#endif
			}
			return uniformLocation;
		}
	}
	else
	{
		return pParentShader->get_uniform_location( name );
	}
}
// Get Uniform Block location
int glShader::get_uniform_block_location ( const char* name )
{
	if ( !bIsReference )
	{
		arstring<128> cname ( name );
		unordered_map<arstring<128>,int>::iterator result = mUniformMap.find( cname );
		if ( result != mUniformMap.end() )
		{
			return result->second;
		}
		else
		{
			int uniformLocation = glGetUniformBlockIndex( iProgramID, name );
			mUniformMap[cname] = uniformLocation;
			if ( uniformLocation < 0 && CGameSettings::Active()->b_dbg_ro_ShowMissingLinks )
			{
#ifdef _ENGINE_DEBUG
				cout << "Warning in: " << this << ": can't find shader uniform block '" << name << "'" << endl;
#endif
			}
			return uniformLocation;
		}
	}
	else
	{
		return pParentShader->get_uniform_block_location( name );
	}
}
// Vertex Attribute grabbing
int	glShader::get_attrib_location ( const char* name )
{
	if ( !bIsReference )
	{
		arstring<128> cname ( name );
		unordered_map<arstring<128>,int>::iterator result = mUniformMap.find( cname );
		if ( result != mUniformMap.end() )
		{
			return result->second;
		}
		else
		{
			int uniformLocation = glGetAttribLocation( iProgramID, name );
			mUniformMap[cname] = uniformLocation;
			if ( uniformLocation < 0 && CGameSettings::Active()->b_dbg_ro_ShowMissingLinks )
			{
#ifdef _ENGINE_DEBUG
				cout << "Warning in: " << this << ": can't find shader attribute '" << name << "'" << endl;
#endif
			}
			return uniformLocation;
		}
	}
	else
	{
		return pParentShader->get_attrib_location( name );
	}
}

// == Drawing Program Controls ==
void glShader::begin ( void )
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
		glUseProgram( 0 );
		//throw std::exception( "Shader compile error" );
	}
}
void glShader::end ( void )
{
	// Reset the program being used
	glUseProgram( 0 );
}

// == Memory Management ==
// Increment number of references
void glShader::AddReference ( void )
{
	if ( !bIsReference )
		iRefNumber++;
	else
		pParentShader->AddReference();
}
// Decrement number of references
void glShader::DecrementReference ( void )
{
	if ( !bIsReference ) {
		if ( iRefNumber == 0 )
		{
			cout << "DECREMENTING BELOW ZERO. THIS IS IMPOSSIBLE. (" << bIsReference << ") " << endl;
			return;
		}
		iRefNumber--;
	}
	else {
		pParentShader->DecrementReference();
	}
}
// Returns number of references around. Job of user to properly manage this number.
unsigned int glShader::ReferenceCount ( void )
{
	return iRefNumber;
}
// Frees the shader
void glShader::ReleaseReference ( void )
{
	DecrementReference();
	bool result = ShaderManager.RemoveShader( this );
	if ( result )
		cout << " -parent shader is destroyed." << endl;
}
// Increament number of references
void glShader::GrabReference ( void )
{
	AddReference();
}

// == Shader Construction ==
// Recompile
bool glShader::recompile ( void )
{
	mUniformMap.clear();

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
long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}
#include "core/utils/StringUtils.h"
#include "core/system/io/FileUtils.h"
// This loads the shader from the file and into the sRawShader string
void glShader::open_shader ( void )
{
	// Based on the file extention, get the shader type.
	string sExtension = StringUtils::ToLower( StringUtils::GetFileExtension( sShaderFilename ) );

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
		if ( stTag == GLE::SHADER_TAG_DEFAULT )
		{
			sVertFilename = sVertFilename + "vert";
			sVertFilename = Core::Resources::PathTo( sVertFilename );
		}
		else if ( stTag == GLE::SHADER_TAG_SKINNING )
		{
			string tsVertFilename = sVertFilename + "skinning.vert";
			tsVertFilename = Core::Resources::PathTo( tsVertFilename );
			if (!IO::FileExists( tsVertFilename )) {
				Debug::Console->PrintWarning( "WARNING: COULD NOT FIND SKINNING TARGET FOR SHADER '" + sVertFilename + "'\n" );
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
		sVertFilename = Core::Resources::PathTo( sVertFilename );
		if (!IO::FileExists( sVertFilename )) {
			fail = true;
		}
		sFragFilename = Core::Resources::PathTo( sFragFilename );
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
void glShader::parse_shader ( void )
{
	// Parse the shader differently based on stShaderType.
}
// This compiles the shaders and adds them to a program object
void glShader::compile_shader ( void )
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
		//cout << string( sVertexShader, iVertexShaderLength ) << "--2end" << endl;
		//cout << sVertexShader << "--2end" << endl;
		//psTempShader = new GLchar [sVertexShader.size()+1];
		//strcpy ( psTempShader, sVertexShader.c_str() );
		//cout << iVertexShaderLength << endl;
		glShaderSource( iVertexShaderID, 1, (const char**)&sVertexShader, &iVertexShaderLength );
		glCompileShader( iVertexShaderID );
		//delete psTempShader;

		// Check for error in the compiling
		//glGetObjectParameterivARB( iVertexShaderID, GL_COMPILE_STATUS, &compiled );
		glGetShaderiv( iVertexShaderID, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			std::cout << "Compile error in vertex shader.\n";
			glGetShaderiv( iVertexShaderID, GL_INFO_LOG_LENGTH , &blen ); 
			if ( blen > 1 )
			{
				GLchar* compiler_log = (GLchar*)malloc(blen);
				glGetShaderInfoLog( iVertexShaderID, blen, &slen, compiler_log );
				//cout << "vertex shader compiler_log:\n" << compiler_log;
				std::cout << "Filename: " << sShaderFilename << "\n";
				Debug::Console->PrintError( "vertex shader compiler_log:\n" );
				Debug::Console->PrintError( compiler_log );
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
				//cout << "vertex shader compiler_log:\n" << compiler_log;
				std::cout << "Filename: " << sShaderFilename << "\n";
				Debug::Console->PrintWarning( "vertex shader compiler_log:\n" );
				Debug::Console->PrintWarning( compiler_log );
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
		//psTempShader = new GLchar [sPixelShader.size()+1];
		//strcpy ( psTempShader, sPixelShader.c_str() );
		glShaderSource( iPixelShaderID, 1, (const char**)&sPixelShader, &iPixelShaderLength );
		glCompileShader( iPixelShaderID );
		//delete psTempShader;

		// Check for error in the compiling
		glGetShaderiv( iPixelShaderID, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			std::cout << "Compile error in pixel/fragment shader.\n";
			glGetShaderiv( iPixelShaderID, GL_INFO_LOG_LENGTH , &blen ); 
			if ( blen > 1 )
			{
				GLchar* compiler_log = (GLchar*)malloc(blen);
				//glGetInfoLogARB( iPixelShaderID, blen, &slen, compiler_log );
				glGetShaderInfoLog( iPixelShaderID, blen, &slen, compiler_log );
				//cout << "pixel/fragment shader compiler_log:\n" << compiler_log;
				std::cout << "Filename: " << sShaderFilename << "\n";
				Debug::Console->PrintError( "pixel/fragment shader compiler_log:\n" );
				Debug::Console->PrintError( compiler_log );
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
				//cout << "pixel/fragment shader compiler_log:\n" << compiler_log;
				std::cout << "Filename: " << sShaderFilename << "\n";
				Debug::Console->PrintWarning( "pixel/fragment shader compiler_log:\n" );
				Debug::Console->PrintWarning( compiler_log );
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

		glLinkProgram( iProgramID );
		GL.CheckError();

		int linked = 0;
		glGetProgramiv( iProgramID, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			cout << "Error in shader program link.\n";
			bHasCompileError = true;
			iCompileResult |= SCE_PROGRAM;
		}   
	}
	
	// Output errors if the compiling or linking didn't work
	if ( bHasCompileError )
	{
		// Print out failure
		cout << "Shader creation failure. Here's the failures:\n";
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
}