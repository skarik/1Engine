
#include "CRenderable2D.h"

#include "core/utils/StringUtils.h"
#include "core-ext/system/io/Resources.h"

#include "renderer/material/glShader.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/texture/TextureLoader.h"
#include "render2d/texture/TextureLoader.h"

#include "renderer/system/glMainSystem.h"

#include "render2d/preprocess/PaletteToLUT.h"
#include "render2d/state/WorldPalette.h"

CRenderable2D::CRenderable2D ( void )
	: CRenderableObject()
{
	// Use a default 2D material
	m_material = new glMaterial();
	m_material->setTexture( 0, Core::Orphan(new CTexture("null")) );

	m_material->passinfo.push_back( glPass() );
	m_material->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
	m_material->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	m_material->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;
	m_material->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;

	m_material->deferredinfo.push_back( glPass_Deferred() );
	m_material->deferredinfo[0].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;
	m_material->deferredinfo[0].m_lighting_mode = Renderer::LI_NONE;
	m_material->deferredinfo[0].m_diffuse_method = Renderer::Deferred::DIFFUSE_DEFAULT;

	// Start with empty buffers
	m_buffer_verts = NIL;
	m_buffer_tris = NIL;

	// Start off with empty model data
	m_modeldata.triangleNum = 0;
	m_modeldata.triangles = NULL;
	m_modeldata.vertexNum = 0;
	m_modeldata.vertices = NULL;
}

CRenderable2D::~CRenderable2D ()
{ GL_ACCESS
	// Material reference released automatically

	// Still have to release buffers
	if ( m_buffer_verts != NIL ) {
		GL.FreeBuffer( &m_buffer_verts );
		m_buffer_verts = NIL;
	}
	if ( m_buffer_tris != NIL ) {
		GL.FreeBuffer( &m_buffer_tris );
		m_buffer_tris = NIL;
	}

}

//		SetSpriteFile ( c-string sprite filename )
// Sets the sprite filename to load or convert
void CRenderable2D::SetSpriteFile ( const char* n_sprite_filename )
{
	// Create the filename for the palette.
	std::string filename_palette = StringUtils::GetFileStemLeaf(n_sprite_filename) + "_pal";
	std::string filename_sprite	 = StringUtils::GetFileStemLeaf(n_sprite_filename);
	std::string filename_normals = StringUtils::GetFileStemLeaf(n_sprite_filename) + "_normal";

	std::string resource_palette;
	std::string resource_sprite;
	std::string resource_normals;

#if 1 // DEVELOPER_MODE
	// Check for the PNGs:
	{
		// Convert the resource files to engine's format (if the PNGs exist)
		if ( Core::Resources::MakePathTo(filename_sprite + ".png", resource_sprite) )
		{
			Textures::ConvertFile(resource_sprite, StringUtils::GetFileStemLeaf(resource_sprite) + ".bpd");
		}
		if ( Core::Resources::MakePathTo(filename_palette + ".png", resource_palette) )
		{
			Textures::ConvertFile(resource_palette, StringUtils::GetFileStemLeaf(resource_palette)  + ".bpd");
		}
		if ( Core::Resources::MakePathTo(filename_normals + ".png", resource_normals) )
		{
			Textures::ConvertFile(resource_normals, StringUtils::GetFileStemLeaf(resource_normals)  + ".bpd");
		}
	}
	// Check for the GALs:
	{
		// Convert the resource files to engine's format (if the GALs exist)
		if ( Core::Resources::MakePathTo(filename_sprite + ".gal", resource_sprite) )
		{
			Textures::timgInfo image_info;
			pixel_t* image = Textures::loadGAL(resource_sprite, image_info); 
			Textures::ConvertData(image, &image_info, StringUtils::GetFileStemLeaf(resource_sprite) + ".bpd");
			delete [] image;

			image = Textures::loadGAL_Layer(resource_sprite, "normals", image_info);
			if (image != NULL)
			{
				Textures::ConvertData(image, &image_info, StringUtils::GetFileStemLeaf(resource_sprite) + "_normal.bpd");
				delete [] image;
			}
		}
		if ( Core::Resources::MakePathTo(filename_palette + ".gal", resource_palette) )
		{
			Textures::timgInfo image_info;
			pixel_t* image = Textures::loadGAL(resource_palette, image_info); 
			Textures::ConvertData(image, &image_info, StringUtils::GetFileStemLeaf(resource_palette) + ".bpd");
			delete [] image;
		}
	}
#endif// DEVELOPER_MODE

	// Now create BPD paths
	if ( !Core::Resources::MakePathTo(filename_sprite + ".bpd", resource_sprite) )
	{
		throw Core::MissingFileException();
	}
	if ( !Core::Resources::MakePathTo(filename_palette + ".bpd", resource_palette) )
	{
		CTexture* new_texture = new CTexture (
			n_sprite_filename, 
			Texture2D, RGBA8,
			1024,1024, Clamp,Clamp,
			MipmapNone,SamplingPoint
		);

		// Set sprite info
		m_spriteInfo.fullsize.x = new_texture->GetWidth();
		m_spriteInfo.fullsize.y = new_texture->GetHeight();

		m_spriteInfo.framesize.x = new_texture->GetWidth();
		m_spriteInfo.framesize.y = new_texture->GetHeight();

		// Set the material based on the input file.
		m_material->setTexture(0, new_texture);

		// No longer need the texture in this object
		new_texture->RemoveReference();

		// TODO: Remove deferred pass from the shader so it only renders in forward mode
		//m_material->deferredinfo.clear();

		// Not a palette'd sprite.
		return;
	}

	// Load the raw data from the palette first
	Textures::timgInfo imginfo_palette;
	pixel_t* raw_palette = Textures::LoadRawImageData(resource_palette, imginfo_palette);
	if ( raw_palette == NULL ) throw Core::MissingDataException();

	// Set all data in the palette to have 255 alpha (opaque)
	for (int i = 0; i < imginfo_palette.width * imginfo_palette.height; ++i)
		raw_palette[i].a = 255;

	// Add the palette to the world's palette
	Render2D::WorldPalette::Active()->AddPalette( raw_palette, imginfo_palette.height, imginfo_palette.width );
	
	// No longer need local palette
	delete [] raw_palette;

	// Load the raw data from the sprite
	Textures::timgInfo imginfo_sprite;
	pixel_t* raw_sprite = Textures::LoadRawImageData(resource_sprite, imginfo_sprite);
	if ( raw_sprite == NULL ) throw Core::MissingDataException();

	// Convert the colors to the internal world's palette
	Render2D::Preprocess::DataToLUT(
		raw_sprite, imginfo_sprite.width * imginfo_sprite.height,
		Render2D::WorldPalette::Active()->palette_data, Render2D::WorldPalette::MAX_HEIGHT, Render2D::WorldPalette::Active()->palette_width);

	// Create empty texture to upload data into
	CTexture* new_texture = new CTexture("");

	// Upload the data
	new_texture->Upload(
		raw_sprite,
		imginfo_sprite.width, imginfo_sprite.height, 
		Clamp, Clamp,
		MipmapNone, SamplingPoint
	);

	// Set sprite info
	m_spriteInfo.fullsize.x = new_texture->GetWidth();
	m_spriteInfo.fullsize.y = new_texture->GetHeight();

	m_spriteInfo.framesize.x = new_texture->GetWidth();
	m_spriteInfo.framesize.y = new_texture->GetHeight();

	// Set the palette texture as the sprite
	m_material->setTexture(0, new_texture);

	// Clear off the data now that it's on the GPU
	delete [] raw_sprite;

	// No longer need the texture in this object
	new_texture->RemoveReference();

	// Create normal map texture
	if ( Core::Resources::MakePathTo(filename_normals + ".bpd", resource_normals) )
	{
		CTexture* new_texture = new CTexture (
			resource_normals, 
			Texture2D, RGBA8,
			1024,1024, Clamp,Clamp,
			MipmapNone,SamplingPoint
		);

		m_material->setTexture(1, new_texture);

		// No longer need the texture in this object
		new_texture->RemoveReference();
	}
	else
	{
		CTexture* new_texture = new CTexture( "textures/default_normals.jpg" );
		m_material->setTexture(1, new_texture);

		// No longer need the texture in this object
		new_texture->RemoveReference();
	}
}

//		GetSpriteInfo ()
// Returns read-only reference to the current sprite information structure.
const spriteInfo_t& CRenderable2D::GetSpriteInfo ( void )
{
	return m_spriteInfo;
}

//		PushModelData()
// Takes the information inside of m_modeldata and pushes it to the GPU so that it may be rendered.
void CRenderable2D::PushModeldata ( void )
{ GL_ACCESS
	GL.BindVertexArray( 0 );

	// Create new buffers
	if ( m_buffer_verts == NIL )
		GL.CreateBuffer( &m_buffer_verts );
	if ( m_buffer_tris == NIL )
		GL.CreateBuffer( &m_buffer_tris );
	//bShaderSetup = false; // With making new buffers, shader is now not ready

	// Bind to some buffer objects
	GL.BindBuffer( GL_ARRAY_BUFFER,			m_buffer_verts ); // for vertex coordinates
	GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER,	m_buffer_tris ); // for face vertex indexes

	// Copy data to the buffer
	GL.UploadBuffer( GL_ARRAY_BUFFER,
		sizeof(CModelVertex) * (m_modeldata.vertexNum),
		m_modeldata.vertices,
		GL_STATIC_DRAW );
	GL.UploadBuffer( GL_ELEMENT_ARRAY_BUFFER,
		sizeof(CModelTriangle) * (m_modeldata.triangleNum),
		m_modeldata.triangles,
		GL_STATIC_DRAW );

	// bind with 0, so, switch back to normal pointer operation
	GL.UnbindBuffer( GL_ARRAY_BUFFER );
	GL.UnbindBuffer( GL_ELEMENT_ARRAY_BUFFER );
}

//		Render()
// Render the model using the 2D engine's style
bool CRenderable2D::Render ( const char pass )
{ GL_ACCESS
	// Do not render if no buffer to render with
	if ( m_buffer_verts == 0 || m_buffer_tris == 0 )
	{
		return true;
	}

	// For now, we will render the same way as the 3d meshes render
	GL.Transform( &(transform) );
	m_material->m_bufferSkeletonSize = 0;
	m_material->m_bufferMatricesSkinning = 0;
		GL.CheckError();
	m_material->bindPass(pass);
		GL.CheckError();
	//parent->SendShaderUniforms(this);
	//	GL.CheckError();
	BindVAO( pass, m_buffer_verts, m_buffer_tris );
		GL.CheckError();
	GL.DrawElements( GL_TRIANGLES, m_modeldata.triangleNum*3, GL_UNSIGNED_INT, 0 );
		GL.CheckError();

	//GL.endOrtho();
	// Success!
	return true;
}