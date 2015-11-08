
#include "glMaterial.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "core/utils/StringUtils.h"
#include "core/debug/CDebugConsole.h"
#include "renderer/system/glMainSystem.h"

#include "renderer/texture/CTextureCube.h"
//#include <boost/algorithm/string.hpp>

// static glMaterial::isValidFile ( fname )
// Searches for the material file with the given name.
// When found, it will return true. Otherwise, returns false.
bool glMaterial::isValidFile ( const char* n_materialfile )
{
	// Quick return to prevent memory access errors
	if ( strlen(n_materialfile) == 0 ) {
		return false;
	}

	// Trim input string name
	string trimmedMatname = StringUtils::FullTrim( n_materialfile );
	//boost::trim( trimmedMatname );
	char str [128];
	char*tok;
	strcpy( str, trimmedMatname.c_str() );
	tok = strtok( str, "\0 ");
	trimmedMatname.clear();
	trimmedMatname = tok;

	// Generate filenames to try
	string sBasefilename = ".res/materials/" + trimmedMatname;
	string sTextfilename = sBasefilename + ".txt";
	string sMatrfilename = sBasefilename + ".mat";

	// Try opening the file
	std::ifstream ifile;
	ifile.open( sTextfilename.c_str(), std::ios_base::in );
	if ( !(ifile.is_open()) ) {
		ifile.open( sMatrfilename.c_str(), std::ios_base::in );
	}

	// Check for success
	if ( !(ifile.is_open()) ) {
		return false;
	}
	else {
		return true;
	}
}

// glMaterial::loadFromFile ( fname )
// First searches for the material file with the given name.
// If it finds it, it then will load it and create all needed pass objects
void glMaterial::loadFromFile ( const char* n_materialfile )
{
	GL_ACCESS;

	// Quick return to prevent memory access errors
	if ( strlen(n_materialfile) == 0 ) {
		m_filename = "";
		return;
	}

	// Trim input string name
	string trimmedMatname = StringUtils::FullTrim( n_materialfile );
	char str [128];
	char*tok;
	strcpy( str, trimmedMatname.c_str() );
	tok = strtok( str, "\0 ");
	trimmedMatname.clear();
	trimmedMatname = tok;

	eMipmapGenerationStyle mipmapMode = MipmapNormal;

	// Generate filenames to try
	string sBasefilename = ".res/materials/" + trimmedMatname;
	string sTextfilename = sBasefilename + ".txt";
	string sMatrfilename = sBasefilename + ".mat";

	// Try opening the file
	std::ifstream ifile;
	ifile.open( sTextfilename.c_str(), std::ios_base::in );
	if ( !(ifile.is_open()) ) {
		ifile.open( sMatrfilename.c_str(), std::ios_base::in );
	}

	if ( !(ifile.is_open()) )
	{	// Spit error if cannot open material
		Debug::Console->PrintError( "Could not open material " );
		std::cout << '"' << sBasefilename << '"' << std::endl;
	}
	else 
	{
		// Set material name
		m_filename = n_materialfile;
		// Generate shader targets
		GLE::shader_tag_t targetTag = GLE::SHADER_TAG_DEFAULT;
		if ( useSkinning ) {
			targetTag = GLE::SHADER_TAG_SKINNING;
		}

		// Start parsing the file
		enum ELoadState
		{
			LOOKING_FOR_PASS,
			LOOKING_FOR_BEGIN_PASS,
			LOADING_PASS_PROPERTIES,
			FINISHED_MAIN_PASS,

			LOOKING_FOR_BEGIN_EYE_PASS,
			LOADING_EYE_PASS_PROPERTIES

		} currentLoadState	= LOOKING_FOR_PASS;

		// Current line
		char	cmd [256];
		string	sCommand;

		// Load in while not end of file
		while ( !ifile.eof() )	// First, only load in the primary pass (since this is the old render system)
		{
			// Load in the next line
			ifile.getline( cmd, 256 );
			std::stringstream ss; 
			ss.str( string( cmd ) );

			// Make case insentive
			sCommand.clear();
			ss >> sCommand;
			sCommand = StringUtils::ToLower( sCommand );

			switch ( currentLoadState )
			{
			case LOOKING_FOR_PASS:
				// Keep loading in entire lines until we find an actual name (either main or diffuse)
				if ( sCommand == "main" || sCommand == "diffuse" ) {
					currentLoadState = LOOKING_FOR_BEGIN_PASS;
				}
				else if ( sCommand == "main_eyes" ) {
					if ( GL.ShadersAvailable ) {
						currentLoadState = LOOKING_FOR_BEGIN_EYE_PASS;
					}
				}
				break;

			case LOOKING_FOR_BEGIN_PASS:
			case LOOKING_FOR_BEGIN_EYE_PASS:
				// If start paren, then the pass definition is beginning
				if ( sCommand == "{" ) {
					// Check for specific pass types
					if ( currentLoadState == LOOKING_FOR_BEGIN_EYE_PASS ) {
						currentLoadState = LOADING_EYE_PASS_PROPERTIES;
					}
					else {
						currentLoadState = LOADING_PASS_PROPERTIES;
					}
				}
				break;

			case LOADING_PASS_PROPERTIES:
				// If end paren, then the pass definition is over
				if ( sCommand == "}" ) {
					currentLoadState = FINISHED_MAIN_PASS;
				}
				else // Parse the properties
				{
					ftype t_float;
					char str [256];

					// Put the pass definition keywords here.
					if ( sCommand == "shader" ) {
						ss >> sCommand;
						if ( sCommand == "default" ) {
							setShader( new glShader( ".res\\shaders\\d\\diffuse.glsl", targetTag ) );
							if ( pTextures[1] == NULL ) {	// Set default textures if non allocated
								pTextures[1] = new CTexture ( ".res\\textures\\black.jpg" );
							}
						}
						else if ( sCommand == "flora" ) {
							setShader( new glShader( ".res/shaders/world/foliage.glsl", targetTag ) );
							if ( pTextures[1] == NULL ) {	// Set default textures if non allocated
								pTextures[1] = new CTexture ( ".res/textures/black.jpg" );
							}
						}
						else if ( sCommand == "fur_single_pass" ) {
							setShader( new glShader( ".res\\shaders\\d\\fur_single_pass.glsl", targetTag ) );
							isTransparent = true;
							if ( pTextures[1] == NULL ) {	// Set default color mask
								pTextures[1] = new CTexture ( ".res/textures/white.jpg" );
							}
						}
						else if ( sCommand == "skin" ) {
							setShader( new glShader( ".res/shaders/d/skin.glsl", targetTag ) );
							if ( pTextures[1] == NULL ) {	// Set default inverse cutout mask
								pTextures[1] = new CTexture ( ".res/textures/black.jpg" );
							}
							if ( pTextures[2] == NULL ) {	// Set default fademap
								pTextures[2] = new CTexture ( ".res/textures/black.jpg" );
							}
							if ( pTextures[3] == NULL ) {	// Set default transparent tattoo map
								pTextures[3] = new CTexture ( ".res/textures/transparent.png" );
							}
							if ( pTextures[4] == NULL ) {
								pTextures[4] = new CTextureCube( "__m_reflectCubemap",
									".res/textures/sky/sky3side.jpg",".res/textures/sky/sky3side.jpg",
									".res/textures/sky/sky3side.jpg",".res/textures/sky/sky3side.jpg",
									".res/textures/sky/sky3top.jpg",".res/textures/sky/sky3bottom.jpg"
									);
							}
						}
						else if ( sCommand == "particle" ) {
							setShader( new glShader( ".res\\shaders\\particles\\colorblended.glsl" ) );
						}
						else if ( sCommand == "particle_softadd" ) {
							setShader( new glShader( ".res\\shaders\\particles\\colorblendedsoftadd.glsl" ) );
						}
						else if ( sCommand == "terrain" ) {
							setShader( new glShader( ".res\\shaders\\world\\terrainDefault.glsl" ) );
							if ( pTextures[1] == NULL ) {	// Set default textures if non allocated
								pTextures[1] = new CTexture ( ".res\\textures\\black.jpg" );
							}
						}
						else if ( sCommand == "custom" ) {
							if ( pTextures[0] == NULL ) {	// Set default textures if non allocated
								pTextures[0] = new CTexture ( ".res/textures/white.jpg" );
							}
							if ( pTextures[1] == NULL ) {	// Set default textures if non allocated
								pTextures[1] = new CTexture ( ".res/textures/black.jpg" );
							}
							if ( pTextures[2] == NULL ) {	// Set default textures if non allocated
								pTextures[2] = new CTexture ( ".res/textures/black.jpg" );
							}
						}
					}
					else if ( sCommand == "mipmaps" ) {
						ss >> sCommand;
						if ( sCommand == "nearest" ) {
							mipmapMode = MipmapNearest;
						}
						else if ( sCommand == "none" ) {
							mipmapMode = MipmapNone;
						}
					}
					else if ( sCommand == "shadername" ) {
						ss.getline( str, 256, '\n' );
						sCommand = StringUtils::TrimLeft( str );
						if ( sCommand.length() > 1 ) {
							// Load the texture
							setShader( new glShader( ".res/" + sCommand, targetTag ) );
						}
					}
					else if ( sCommand == "blendmode" ) { // Set blendmode
						ss >> sCommand;
						if ( sCommand == "additive" ) {
							iBlendMode = BM_ADD;
							isTransparent = true;
							useAlphaTest = false;
							useDepthMask = false;
						}
						else if ( sCommand == "normal" || sCommand == "default" ) {
							iBlendMode = BM_NORMAL;
							isTransparent = false;
							useAlphaTest = false;
							useDepthMask = true;
						}
						else if ( sCommand == "alphatest" ) {
							iBlendMode = BM_NORMAL;
							isTransparent = true;
							useAlphaTest = true;
							useDepthMask = true;
						}
						else if ( sCommand == "alphablend" ) {
							iBlendMode = BM_NORMAL;
							isTransparent = true;
							useAlphaTest = false;
							useDepthMask = false;
						}
						else if ( sCommand == "softalphablend" ) {
							iBlendMode = BM_SOFT_ADD;
							isTransparent = true;
							useAlphaTest = false;
							useDepthMask = false;
						}
						else {
							Debug::Console->PrintWarning( "Invalid blendmode matval.\n" );
						}
					}
					else if ( sCommand == "lighting" ) { // Set lighting
						ss >> sCommand;
						if ( sCommand == "disabled" ) {
							useLighting = false;
						}
						else if ( sCommand == "enabled" || sCommand == "default" ) {
							useLighting = true;
						}
						else {
							Debug::Console->PrintWarning( "Invalid lighting matval.\n" );
						}
					}
					else if ( sCommand == "shadows" ) { // Set shadows
						ss >> sCommand;
						//Debug::Console->PrintWarning( "Shadow matvals are ALWAYS invalid, fool.\n" );
					}
					else if ( sCommand == "blended_diffuse" ) { // Set blended diffuse
						ss >> sCommand;
						if ( sCommand == "disabled" || sCommand == "default" ) {
							useBlendedDiffuse = false;
						}
						else if ( sCommand == "enabled"  ) {
							useBlendedDiffuse = true;
						}
						else {
							Debug::Console->PrintWarning( "Invalid blended_diffuse matval.\n" );
						}
						Debug::Console->PrintWarning( "Matval 'blended_diffuse' is depreceated. Use sys_DiffuseColor in shaders instead.\n" );
					}
					else if (( sCommand == "texture" )||( sCommand == "texture0" )) { // Set texture
						ss.getline( str, 256, '\n' );
						sCommand = StringUtils::TrimLeft( str );
						//cout << "sCommand: -" << sCommand << "-" << (int)(sCommand[0]) << endl;
						if ( sCommand.length() > 1 ) {
							// Load the texture
							//loadTexture( ".res\\" + sCommand );
							//if ( pTextures[0] ) delete pTextures[0];
							pTextures[0] = new CTexture( ".res\\"+sCommand, Texture2D, RGBA8, 1024,1024, Repeat, Repeat, mipmapMode );
							useTexture = true;
						}
					}
					else if (( sCommand == "ftexture" )||( sCommand == "texture2" )) { // Set texture
						ss.getline( str, 256, '\n' );
						sCommand = StringUtils::TrimLeft( str );
						if ( sCommand.length() > 1 ) {
							//if ( pTextures[2] ) delete pTextures[2];
							pTextures[2] = new CTexture ( ".res\\" + sCommand, Texture2D, RGBA8, 1024,1024, Repeat, Repeat, mipmapMode );
						}
					}
					else if (( sCommand == "glowmap" )||( sCommand == "fur_colormask" )||( sCommand == "texture1" )) { // Set texture
						ss.getline( str, 256, '\n' );
						sCommand = StringUtils::TrimLeft( str );
						if ( sCommand.length() > 1 ) { // Load the texture
							//if ( pTextures[1] ) delete pTextures[1];
							pTextures[1] = new CTexture ( ".res\\" + sCommand, Texture2D, RGBA8, 1024,1024, Repeat, Repeat, mipmapMode );
						}
					}
					else if ( sCommand == "tattoomap" ) { // Set tattoo texture
						ss.getline( str, 256, '\n' );
						sCommand = StringUtils::TrimLeft( str );
						if ( sCommand.length() > 1 ) {
							//if ( pTextures[3] ) delete pTextures[3];
							pTextures[3] = new CTexture ( ".res/" + sCommand, Texture2D, RGBA8, 1024,1024, Repeat, Repeat, mipmapMode );
						}
					}
					else if ( sCommand == "diffuse" ) { // Set diffuse color
						ss >> t_float;	diffuse.red		= t_float;
						ss >> t_float;	diffuse.green	= t_float;
						ss >> t_float;	diffuse.blue	= t_float;
						useColors = false;
					}
					else if ( sCommand == "emissive" ) { // Set emissive color
						ss >> t_float;	emissive.red	= t_float;
						ss >> t_float;	emissive.green	= t_float;
						ss >> t_float;	emissive.blue	= t_float;
					}
					else if ( sCommand == "facemode" ) {
						ss >> sCommand;
						if ( sCommand == "twosided" ) {
							iFaceMode = FM_FRONTANDBACK;
						}
						else if ( sCommand == "back" ) {
							iFaceMode = FM_BACK;
						}
						else if ( sCommand == "front" ) {
							iFaceMode = FM_FRONT;
						}
					}
				}
				break;

			case LOADING_EYE_PASS_PROPERTIES:
				// If end paren, then the pass definition is over
				if ( sCommand == "}" ) {
					currentLoadState = FINISHED_MAIN_PASS;
				}
				else // Parse the properties
				{
					//ftype t_float;
					char str [256];

					// Put the pass definition keywords here.
					if ( sCommand == "begin_eyepass" ) {
						setShader( new glShader( ".res\\shaders\\d\\eye_shading.glsl", targetTag ) );
						useTexture = true;
					}
					else if ( sCommand == "base" ) { // Set base texture
						ss.getline( str, 256, '\n' );
						sCommand = StringUtils::TrimLeft( str );
						if ( sCommand.length() > 1 ) {
							// Load the texture
							//if ( pTextures[0] ) delete pTextures[0];
							pTextures[0] = new CTexture ( ".res\\" + sCommand );
						}
					}
					else if ( sCommand == "iris" ) { // Set forground texture
						ss.getline( str, 256, '\n' );
						sCommand = StringUtils::TrimLeft( str );
						if ( sCommand.length() > 1 ) {
							// Load the texture
							//if ( pTextures[1] ) delete pTextures[1];
							pTextures[1] = new CTexture ( ".res\\" + sCommand );
						}
					}
					else if ( sCommand == "pupil" ) { // Set pupil texture
						ss.getline( str, 256, '\n' );
						sCommand = StringUtils::TrimLeft( str );
						if ( sCommand.length() > 1 ) {
							// Load the texture
							//if ( pTextures[2] ) delete pTextures[2];
							pTextures[2] = new CTexture ( ".res\\" + sCommand );
						}
					}
					else if ( sCommand == "specular" ) {
						useSpecular = true;
					}
				}
				break;

			default:
				break;
			} // End load state
		} // End file loading

		ifile.close();
	}

	useLighting = true;
}