
#include "RrMaterial.h"
#include "core/debug/CDebugConsole.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "core/utils/string.h"
#include "core-ext/system/io/Resources.h"
//#include <boost/algorithm/string.hpp>
#include "renderer/texture/CTextureCube.h"


string trimMaterialName ( const char* n_name )
{
	// Trim input string name
	string trimmedMatname = core::utils::string::FullTrim( n_name );
	//boost::trim( trimmedMatname );
	char  str [256];
	char* tok;
	strcpy( str, trimmedMatname.c_str() );
	tok = strtok( str, "\0 ");
	trimmedMatname.clear();
	trimmedMatname = tok;
	return trimmedMatname;
}

// static RrMaterial::isValidFile ( fname )
// Searches for the material file with the given name.
// When found, it will return true. Otherwise, returns false.
bool RrMaterial::isValidFile ( const char* n_materialfile )
{
	// Quick return to prevent memory access errors
	if ( strlen(n_materialfile) <= 1 ) {
		return false;
	}

	string trimmedMatname = trimMaterialName( n_materialfile );

	// Generate filenames to try
	string sBasefilename = "materials/" + trimmedMatname;
	string sTextfilename = core::Resources::PathTo( sBasefilename + ".txt" );
	string sMatrfilename = core::Resources::PathTo( sBasefilename + ".mat" );

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

enum ELoadState
{
	LOOKING_FOR_PASS,
	LOOKING_FOR_BEGIN_PASS,
	LOADING_PASS_PROPERTIES,
	FINISHED_MAIN_PASS,

	SKIP_TO_END_BRACKET,

	LOOKING_FOR_BEGIN_EYE_PASS,
	LOADING_EYE_PASS_PROPERTIES,

	LOOKING_FOR_BEGIN_DEFERRED_PASS,
	LOADING_DEFERRED_PASS_PROPERTIES
}; 
ELoadState loadPassProperties ( RrMaterial* material,
							   const string& command, std::stringstream& ss,
							   const renderer::rrShaderTag targetTag, eMipmapGenerationStyle& mipmapMode );
ELoadState loadEyePassProperties ( RrMaterial* material,
							   const string& command, std::stringstream& ss,
							   const renderer::rrShaderTag targetTag );
ELoadState loadDeferredPassProperties ( RrMaterial* material,
								const string& command, std::stringstream& ss ); 

// RrMaterial::loadFromFile ( fname )
// First searches for the material file with the given name.
// If it finds it, it then will load it and create all needed pass objects
void RrMaterial::loadFromFile ( const char* n_materialfile )
{	
	// Quick return to prevent memory access errors
	if ( strlen(n_materialfile) <= 1 ) {
		return;
	}

	string trimmedMatname = trimMaterialName( n_materialfile );
	m_name = trimmedMatname.c_str(); // Copy name over

	// Generate filenames to try
	string sBasefilename = "materials/" + trimmedMatname;
	string sTextfilename = core::Resources::PathTo( sBasefilename + ".txt" );
	string sMatrfilename = core::Resources::PathTo( sBasefilename + ".mat" );

	// Try opening the file
	std::ifstream ifile;
	ifile.open( sTextfilename.c_str(), std::ios_base::in );
	// Check to make sure not loading same
	if ( m_filename.compare( sTextfilename.c_str() ) ) {
		return;
	}
	m_filename = sTextfilename.c_str(); // Set material filename
	if ( !(ifile.is_open()) ) {
		ifile.open( sMatrfilename.c_str(), std::ios_base::in );
		if ( m_filename.compare( sMatrfilename.c_str() ) ) {
			return;
		}
		m_filename = sMatrfilename.c_str(); // Set material filename
	}
	// Check for success
	if ( !(ifile.is_open()) )
	{	// Spit error if cannot open material
		debug::Console->PrintError( "Could not open material " );
		std::cout << '"' << sBasefilename << '"' << std::endl;
	}
	else 
	{
		// Demolish old pass information
		passinfo.clear();
		deferredinfo.clear();

		// Set default mipmap mode
		eMipmapGenerationStyle mipmapMode = MipmapNormal;

		// Generate shader targets
		renderer::rrShaderTag targetTag = renderer::SHADER_TAG_DEFAULT;
		if ( m_isSkinnedShader ) {
			targetTag = renderer::SHADER_TAG_SKINNING;
		}

		// Start parsing the file
		ELoadState currentLoadState	= LOOKING_FOR_PASS;
		bool t_hasDiffusePass = false;
		bool t_hasDeferredPass = false;
		bool t_automaticDeferred = true;

		// Current line
		char	cmd [256];
		string	sCommand;

		// Load in while not end of file
		while ( !ifile.eof() )	// First, only load in the primary pass (since this is the old render system)
		{
			// Load in the next line
			ifile.getline( cmd, 256 );
			// Parse out comments
			{
				char* commentPosition;
				commentPosition= strstr( cmd, "##" );
				if ( commentPosition != NULL ) {
					*commentPosition = 0;
				}
				commentPosition = strstr( cmd, "//" );
				if ( commentPosition != NULL ) {
					*commentPosition = 0;
				}
			}

			// Use stringstream to parse the command
			std::stringstream ss;
			ss.str( string( cmd ) );

			// Make case insentive
			sCommand.clear();
			ss >> sCommand;
			sCommand = core::utils::string::GetLower( sCommand );

			// If finished main pass, load in alternative passes
			if ( currentLoadState == FINISHED_MAIN_PASS ) {
				currentLoadState = LOOKING_FOR_PASS;
			}

			switch ( currentLoadState )
			{
			case LOOKING_FOR_PASS:
				// Reset mipmap mode
				mipmapMode = MipmapNormal;
				// Keep loading in entire lines until we find an actual name (either main or diffuse)
				if ( sCommand == "main" || sCommand == "diffuse" ) {
					currentLoadState = LOOKING_FOR_BEGIN_PASS;
				}
				// Eye materials have a special 5 layer material that has specific options with a lot of color settings
				else if ( sCommand == "main_eyes" ) {
					currentLoadState = LOOKING_FOR_BEGIN_EYE_PASS;
				}
				// Deferred pass
				else if ( sCommand == "deferred" ) {
					currentLoadState = LOOKING_FOR_BEGIN_DEFERRED_PASS;
				}
				// Ignore fallback states
				else if ( sCommand == "fallback" || sCommand == "main_fallback" ) {
					currentLoadState = SKIP_TO_END_BRACKET;
				}
				// All other non-whitespace inputs
				else if ( sCommand.length() >= 1 ) {
					if ( sCommand == "{" ) {
						// Invalid, skip to next brace
						currentLoadState = SKIP_TO_END_BRACKET;
					}
					else {
						// Invalid command, remove
						debug::Console->PrintWarning( "Invalid command '"+sCommand+"' in material "+string(m_filename) );
					}
				}
				break;
			case SKIP_TO_END_BRACKET:
				// Continue loading things until hit end bracket
				if ( sCommand == "}" ) {
					currentLoadState = LOOKING_FOR_PASS;
				}
				break;

			case LOOKING_FOR_BEGIN_PASS:
				// Look for start brace
				if ( sCommand == "{" ) {
					currentLoadState = LOADING_PASS_PROPERTIES;
					// Add new pass.
					passinfo.push_back( RrPassForward() );
					t_hasDiffusePass = true;
				}
				break;
			case LOADING_PASS_PROPERTIES:
				if ( sCommand == "nodeferred" ) {
					t_automaticDeferred = false;
				}
				else {
					currentLoadState = loadPassProperties( this, sCommand, ss, targetTag, mipmapMode );
				}
				break;

			case LOOKING_FOR_BEGIN_EYE_PASS:
				// Look for start brace
				if ( sCommand == "{" ) {
					currentLoadState = LOADING_EYE_PASS_PROPERTIES;
					// Add new pass.
					passinfo.push_back( RrPassForward() );
					t_hasDiffusePass = true;
				}
				break;
			case LOADING_EYE_PASS_PROPERTIES:
				currentLoadState = loadEyePassProperties( this, sCommand, ss, targetTag );
				break;

			case LOOKING_FOR_BEGIN_DEFERRED_PASS:
				// Look for start brace
				if ( sCommand == "{" ) {
					currentLoadState = LOADING_DEFERRED_PASS_PROPERTIES;
					// Add new pass.
					deferredinfo.push_back( RrPassDeferred() );
					// This pass defaults to the forward pass options though
					if ( t_hasDiffusePass )
					{
						deferredinfo.back().m_transparency_mode = passinfo[0].m_transparency_mode;
						//deferredinfo.back().m_blend_mode = passinfo[0].m_blend_mode;
						//deferredinfo.back().m_blend_mode = renderer::BM_NONE;
						//deferredinfo.back().m_lighting_mode = passinfo[0].m_lighting_mode;
					}
					t_hasDeferredPass = true;
				}
				break;
			case LOADING_DEFERRED_PASS_PROPERTIES:
				currentLoadState = loadDeferredPassProperties( this, sCommand, ss );
				break;
			}

		}
		// End of file

		// Add default deferred pass based on forward pass if none found
		if ( t_automaticDeferred && !t_hasDeferredPass && t_hasDiffusePass )
		{
			// Note we can't handle anything that's not BM_NORMAL and non-transparent
			if ( passinfo[0].m_blend_mode == renderer::BM_NORMAL || passinfo[0].m_blend_mode == renderer::BM_NONE && passinfo[0].m_transparency_mode != renderer::ALPHAMODE_TRANSLUCENT )
			{
				deferredinfo.push_back( RrPassDeferred() );
				deferredinfo[0].m_transparency_mode = passinfo[0].m_transparency_mode;
				//deferredinfo[0].m_blend_mode = passinfo[0].m_blend_mode;
				//deferredinfo[0].m_blend_mode = renderer::BM_NONE;
				//deferredinfo[0].m_lighting_mode = passinfo[0].m_lighting_mode;
				//deferredinfo[0].m_diffuse_method = passinfo[0].m;
			}
		}

		// Perform checks on the passes generated
		for ( uint i = 0; i < passinfo.size(); ++i ) 
		{
			if ( passinfo[i].shader == NULL ) {
				throw std::exception();
			}
		}
		for ( uint i = 0; i < deferredinfo.size(); ++i )
		{
			/*if ( deferredinfo[i].shader == NULL ) {
				//m_isReady = false;
			}*/
		}
	}
	// End of loader

	// Set material name
	m_name = n_materialfile;
}

ELoadState loadPassProperties ( RrMaterial* material,
							   const string& command, std::stringstream& ss,
							   const renderer::rrShaderTag targetTag, eMipmapGenerationStyle& mipmapMode )
{
	Real t_float;
	char str [256];
	string subCommand;
	RrPassForward* t_pass = &(material->passinfo.back());

	if ( command == "}" )
	{
		// If done, and no shader loaded, load default shader
		if ( t_pass->shader == NULL )
		{
			t_pass->shader = new RrShader( "shaders/d/diffuse.glsl", targetTag );
		}
		return FINISHED_MAIN_PASS;
	}
	else if ( command == "shader" )
	{
		ss >> subCommand;
		
		throw core::NotYetImplementedException();

		if ( subCommand == "default" )
		{
			t_pass->shader = new RrShader( "shaders/d/diffuse.glsl", targetTag );
			// Set default textures if non allocated
			if ( material->getTexture(TEX_OVERLAY) == NULL ) {	// Glowmap	
				material->setTexture( TEX_OVERLAY, new CTexture ( "textures/black.jpg" ) );
			}
			if ( material->getTexture(TEX_SURFACE) == NULL ) {	// Specular map
				material->setTexture( TEX_SURFACE, new CTexture ( "textures/default_specular.jpg" ) );
			}
			if ( material->getTexture(TEX_NORMALS) == NULL ) {	// Normal map
				material->setTexture( TEX_NORMALS, new CTexture ( "textures/default_normals.jpg" ) );
			}
		}
		else if ( subCommand == "flora" ) {
			t_pass->shader = new RrShader( "shaders/world/foliage.glsl", targetTag );
			if ( material->getTexture(TEX_OVERLAY) == NULL ) {	// Set default textures if non allocated
				material->setTexture( TEX_OVERLAY, new CTexture ( "textures/black.jpg" ) );
			}
		}
		else if ( subCommand == "fur_single_pass" ) {
			t_pass->shader = new RrShader( "shaders/d/fur_single_pass.glsl", targetTag );
			t_pass->m_transparency_mode = renderer::ALPHAMODE_ALPHATEST;
			if ( material->getTexture(TEX_OVERLAY) == NULL ) {	// Set default color mask
				material->setTexture( TEX_OVERLAY, new CTexture ( "textures/white.jpg" ) );
			}
		}
		else if ( subCommand == "skin" ) {
			t_pass->shader = new RrShader( "shaders/d/skin.glsl", targetTag );
			if ( material->getTexture(TEX_OVERLAY) == NULL ) {	// Set default inverse cutout mask
				material->setTexture( TEX_OVERLAY, new CTexture ( "textures/black.jpg" ) );
			}
			if ( material->getTexture(TEX_OVERLAY) == NULL ) {	// Set default fademap
				material->setTexture( TEX_OVERLAY, new CTexture ( "textures/black.jpg" ) );
			}
			if ( material->getTexture(TEX_OVERLAY) == NULL ) {	// Set default transparent tattoo map
				material->setTexture( TEX_OVERLAY, new CTexture ( "textures/transparent.png" ) );
			}
			if ( material->getTexture(TEX_SLOT4) == NULL ) {
				material->setTexture( TEX_SLOT4, new CTextureCube( "__m_reflectCubemap",
					"textures/sky/sky3side.jpg","textures/sky/sky3side.jpg",
					"textures/sky/sky3side.jpg","textures/sky/sky3side.jpg",
					"textures/sky/sky3top.jpg","textures/sky/sky3bottom.jpg"
					) );
			}
		}
		else if ( subCommand == "particle" ) {
			t_pass->shader = new RrShader( "shaders/particles/colorblended.glsl" );
		}
		else if ( subCommand == "particle_softadd" ) {
			t_pass->shader = new RrShader( "shaders/particles/colorblendedsoftadd.glsl" );
		}
		else if ( subCommand == "terrain" ) {
			t_pass->shader = new RrShader( "shaders/world/terrainDefault.glsl" );
			if ( material->getTexture(TEX_SLOT1) == NULL ) {	// Set default textures if non allocated
				material->setTexture( TEX_SLOT1, new CTexture ( "textures/black.jpg" ) );
			}
		}
		else if ( subCommand == "custom" ) {
			if ( material->getTexture(TEX_SLOT0) == NULL ) {	// Set default textures if non allocated
				material->setTexture( TEX_SLOT0, new CTexture ( "textures/white.jpg" ) );
			}
			if ( material->getTexture(TEX_SLOT1) == NULL ) {	// Set default textures if non allocated
				material->setTexture( TEX_SLOT1, new CTexture ( "textures/black.jpg" ) );
			}
			if ( material->getTexture(TEX_SLOT2) == NULL ) {	// Set default textures if non allocated
				material->setTexture( TEX_SLOT2, new CTexture ( "textures/black.jpg" ) );
			}
		}
	}
	else if ( command == "mipmaps" )
	{
		ss >> subCommand;
		if ( subCommand == "nearest" ) {
			mipmapMode = MipmapNearest;
		}
		else if ( subCommand == "none" ) {
			mipmapMode = MipmapNone;
		}
	}
	else if ( command == "shadername" )
	{
		ss.getline( str, 256, '\n' );
		subCommand = core::utils::string::TrimLeft( str );
		if ( subCommand.length() > 1 ) {
			// Load the texture
			t_pass->shader = new RrShader( subCommand.c_str(), targetTag );
		}
	}
	else if ( command == "blendmode" )
	{ // Set blendmode
		ss >> subCommand;
		if ( subCommand == "additive" ) {
			t_pass->m_blend_mode = renderer::BM_ADD;
		}
		else if ( subCommand == "normal" || subCommand == "default" ) {
			t_pass->m_blend_mode = renderer::BM_NORMAL;
		}
		else if ( subCommand == "alphatest" ) {
			/*iBlendMode = BM_NORMAL;
			isTransparent = true;
			useAlphaTest = true;
			useDepthMask = true;*/
			debug::Console->PrintWarning( "\"blendmode alphatest\" is deprecated. Use transparency enumerator." );
		}
		else if ( subCommand == "alphablend" ) {
			/*iBlendMode = BM_NORMAL;
			isTransparent = true;
			useAlphaTest = false;
			useDepthMask = false;*/
			debug::Console->PrintWarning( "\"blendmode alphablend\" is deprecated. Use transparency enumerator." );
		}
		else if ( subCommand == "softalphablend" ) {
			t_pass->m_blend_mode = renderer::BM_SOFT_ADD;
		}
		else if ( subCommand == "subtractive" || subCommand == "invmul" ) {
			t_pass->m_blend_mode = renderer::BM_INV_MULTIPLY;
		}
		else {
			debug::Console->PrintWarning( "Invalid blendmode matval.\n" );
		}
	}
	else if ( command == "transparency" || command == "alphamode" )
	{
		ss >> subCommand;
		if ( subCommand == "normal" || subCommand == "none" )
		{
			t_pass->m_transparency_mode = renderer::ALPHAMODE_NONE;
		}
		else if ( subCommand == "alphatest" )
		{
			ss >> t_float;
			if ( t_float < FTYPE_PRECISION ) {
				t_float = 0.5f;
			}
			t_pass->m_transparency_mode = renderer::ALPHAMODE_ALPHATEST;
			t_pass->f_alphatest_value = t_float;
		}
		else if ( subCommand == "full" )
		{
			t_pass->m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
		}
	}
	else if ( command == "depthmask" )
	{
		ss >> subCommand;
		if ( subCommand == "default" || subCommand == "1" || subCommand == "true" ) {
			t_pass->b_depthmask = true;
		}
		else {
			t_pass->b_depthmask = false;
		}
	}
	else if ( command == "lighting" )
	{ // Set lighting
		ss >> subCommand;
		if ( subCommand == "disabled" ) {
			t_pass->m_lighting_mode = renderer::LI_NONE;
		}
		else if ( subCommand == "enabled" || subCommand == "default" ) {
			t_pass->m_lighting_mode = renderer::LI_NORMAL;
		}
		else if ( subCommand == "cel" || subCommand == "shaded" ) {
			t_pass->m_lighting_mode = renderer::LI_NORMAL;
		}
		else if ( subCommand == "skin" ) {
			t_pass->m_lighting_mode = renderer::LI_SKIN;
		}
		else {
			debug::Console->PrintWarning( "Invalid lighting matval.\n" );
		}
	}
	else if ( command == "shadows" )
	{ // Set shadows
		ss >> subCommand;
		//debug::Console->PrintWarning( "Shadow matvals are ALWAYS invalid, fool.\n" );
	}
	else if ( command == "blended_diffuse" )
	{ // Set blended diffuse
		ss >> subCommand;
		if ( subCommand == "disabled" || subCommand == "default" ) {
			//useBlendedDiffuse = false;
		}
		else if ( subCommand == "enabled"  ) {
			//useBlendedDiffuse = true;
		}
		else {
			debug::Console->PrintWarning( "Invalid blended_diffuse matval.\n" );
		}
		debug::Console->PrintWarning( "Matval 'blended_diffuse' is depreceated. Use sys_DiffuseColor in shaders instead.\n" );
	}
	else if (( command == "texture" )||( command == "texture0" ))
	{ // Set texture
		ss.getline( str, 256, '\n' );
		subCommand = core::utils::string::TrimLeft( str );
		if ( subCommand.length() > 1 ) {
			material->setTexture( TEX_DIFFUSE, new CTexture( subCommand, Texture2D, RGBA8, 1024,1024, Repeat, Repeat, mipmapMode ) );
		}
	}
	else if (( command == "glowmap" )||( command == "fur_colormask" )||( command == "texture1" )) { // Set texture
		ss.getline( str, 256, '\n' );
		subCommand = core::utils::string::TrimLeft( str );
		if ( subCommand.length() > 1 ) {
			material->setTexture( TEX_OVERLAY, new CTexture( subCommand, Texture2D, RGBA8, 1024,1024, Repeat, Repeat, mipmapMode ) );
		}
	}
	else if (( command == "ftexture" )||( command == "texture2" )) { // Set texture
		ss.getline( str, 256, '\n' );
		subCommand = core::utils::string::TrimLeft( str );
		if ( subCommand.length() > 1 ) {
			material->setTexture( TEX_SURFACE, new CTexture( subCommand, Texture2D, RGBA8, 1024,1024, Repeat, Repeat, mipmapMode ) );
		}
	}
	else if (( command == "tattoomap" )||( command == "texture3" )) { // Set texture
		ss.getline( str, 256, '\n' );
		subCommand = core::utils::string::TrimLeft( str );
		if ( subCommand.length() > 1 ) {
			material->setTexture( TEX_OVERLAY, new CTexture( subCommand, Texture2D, RGBA8, 1024,1024, Repeat, Repeat, mipmapMode ) );
		}
	}
	else if ( command == "diffuse" )
	{ // Set diffuse color
		ss >> t_float;	material->m_diffuse.red		= t_float;
		ss >> t_float;	material->m_diffuse.green	= t_float;
		ss >> t_float;	material->m_diffuse.blue	= t_float;
	}
	else if ( command == "emissive" )
	{ // Set emissive color
		ss >> t_float;	material->m_emissive.red	= t_float;
		ss >> t_float;	material->m_emissive.green	= t_float;
		ss >> t_float;	material->m_emissive.blue	= t_float;
	}
	else if ( command == "specular" )
	{ // Set specular color
		ss >> t_float;	material->m_specular.red	= t_float;
		ss >> t_float;	material->m_specular.green	= t_float;
		ss >> t_float;	material->m_specular.blue	= t_float;
	}
	//else if ( command == "specpow" )
	//{ // Set specular color
	//	ss >> t_float;	material->m_specularPower	= t_float;
	//}
	else if ( command == "facemode" )
	{
		ss >> subCommand;
		if ( subCommand == "twosided" ) {
			t_pass->m_face_mode = renderer::FM_FRONTANDBACK;
		}
		else if ( subCommand == "back" ) {
			t_pass->m_face_mode = renderer::FM_BACK;
		}
		else if ( subCommand == "front" ) {
			t_pass->m_face_mode = renderer::FM_FRONT;
		}
	}
	else
	{
		printf("Unrecognized material command \"%s\"\n", command.c_str());
	}
	return LOADING_PASS_PROPERTIES;
}


ELoadState loadEyePassProperties ( RrMaterial* material,
							   const string& command, std::stringstream& ss,
							   const renderer::rrShaderTag targetTag )
{
	//Real t_float;
	char str [256];
	string subCommand;
	RrPassForward* t_pass = &(material->passinfo.back());

	if ( command == "}" ) {
		return FINISHED_MAIN_PASS;
	}
	else if ( command == "begin_eyepass" )
	{
		t_pass->shader = new RrShader( "shaders/d/eye_shading.glsl", targetTag );
		material->m_specular = Color( 0,0,0 );
		//material->m_specularPower = 23;
	}
	else if ( command == "base" )
	{ // Set base texture
		ss.getline( str, 256, '\n' );
		subCommand = core::utils::string::TrimLeft( str );
		if ( subCommand.length() > 1 ) {
			material->setTexture( TEX_DIFFUSE, new CTexture ( subCommand ) );
		}
	}
	else if ( command == "iris" )
	{ // Set forground texture
		ss.getline( str, 256, '\n' );
		subCommand = core::utils::string::TrimLeft( str );
		if ( subCommand.length() > 1 ) {
			material->setTexture( TEX_SLOT1, new CTexture ( subCommand ) );
		}
	}
	else if ( command == "pupil" )
	{ // Set pupil texture
		ss.getline( str, 256, '\n' );
		subCommand = core::utils::string::TrimLeft( str );
		if ( subCommand.length() > 1 ) {
			material->setTexture( TEX_SLOT2, new CTexture ( subCommand ) );
		}
	}
	else if ( command == "specular" )
	{
		material->m_specular = Color( 1,1,1,1 );
		//material->m_specularPower = 23;
	}
	else
	{
		printf("Unrecognized material command \"%s\"\n", command.c_str());
	}

	return LOADING_EYE_PASS_PROPERTIES;
}

ELoadState loadDeferredPassProperties ( RrMaterial* material,
								const string& command, std::stringstream& ss )
{
	//Real t_float;
	//char str [256];
	string subCommand;
	RrPassDeferred* t_pass = &(material->deferredinfo.back());

	if ( command == "}" )
	{
		return FINISHED_MAIN_PASS;
	}
	/*else if ( command == "specpow" )
	{ // Set specular color
		ss >> t_float;	material->m_specularPower	= t_float;
	}*/
	else if ( command == "diffuse" )
	{
		/*ss >> subCommand;
		if ( subCommand == "default" || subCommand == "standard" ) {
			t_pass->m_diffuse_method = renderer::Deferred::DIFFUSE_DEFAULT;
		}
		if ( subCommand == "eyes" ) {
			t_pass->m_diffuse_method = renderer::Deferred::DIFFUSE_EYES;
			t_pass->m_rimlight_strength = 0.5f;
		}
		else if ( subCommand == "skin" ) {
			t_pass->m_diffuse_method = renderer::Deferred::DIFFUSE_SKIN;
			t_pass->m_rimlight_strength = 1.0f;
		}
		else if ( subCommand == "hair" ) {
			t_pass->m_diffuse_method = renderer::Deferred::DIFFUSE_HAIR;
			t_pass->m_rimlight_strength = 0.4f;
		}*/
		throw core::DeprecatedFeatureException();
	}
	else
	{
		printf("Unrecognized material command \"%s\"\n", command.c_str());
	}

	return LOADING_DEFERRED_PASS_PROPERTIES;
}