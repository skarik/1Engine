
#include "CRenderable2D.h"

#include "core/utils/StringUtils.h"
#include "core-ext/system/io/Resources.h"

#include "renderer/material/RrShader.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/texture/TextureLoader.h"

#include "renderer/resource/CResourceManager.h"

#include "render2d/texture/TextureLoader.h"

#include "renderer/system/glMainSystem.h"

#include "render2d/preprocess/PaletteToLUT.h"
#include "render2d/preprocess/NormalMapGeneration.h"
#include "render2d/state/WorldPalette.h"

CRenderable2D::CRenderable2D ( void )
	: CRenderableObject()
{
	// Set up default parameters
	m_spriteGenerationInfo = spriteGenParams2D_t();
	m_spriteGenerationInfo.normal_default = Vector3d(0, 0, 1.0F);

	// Use a default 2D material
	m_material = new RrMaterial();
	m_material->setTexture( TEX_DIFFUSE, core::Orphan(new CTexture("null")) );
	m_material->setTexture( TEX_SURFACE, renderer::Resources::GetTexture(renderer::TextureBlack) );

	m_material->passinfo.push_back( RrPassForward() );
	m_material->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );
	m_material->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	m_material->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_ALPHATEST;
	m_material->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;

	m_material->deferredinfo.push_back( RrPassDeferred() );
	m_material->deferredinfo[0].m_transparency_mode = renderer::ALPHAMODE_ALPHATEST;

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
// Sets the sprite filename to load or convert. Uses resource manager to cache data.
void CRenderable2D::SetSpriteFile ( const char* n_sprite_filename )
{
	SetSpriteFileAnimated( n_sprite_filename, NULL, NULL );
}
//		SetSpriteFileAnimated ( c-string sprite filename )
// Sets the sprite filename to load and possibly convert, but provides returns for additional information.
void CRenderable2D::SetSpriteFileAnimated ( const char* n_sprite_filename, Textures::timgInfo* o_img_info, Real** o_img_frametimes )
{
	// Create the filename for the palette.
	std::string filename_palette = StringUtils::GetFileStemLeaf(n_sprite_filename) + "_pal";
	std::string filename_sprite	 = StringUtils::GetFileStemLeaf(n_sprite_filename);
	std::string filename_normals = StringUtils::GetFileStemLeaf(n_sprite_filename) + "_normal";
	std::string filename_surface = StringUtils::GetFileStemLeaf(n_sprite_filename) + "_surface";

	std::string resource_palette;
	std::string resource_sprite;
	std::string resource_normals;
	std::string resource_surface;

#if 1 // DEVELOPER_MODE
	// Check for the PNGs:
	{
		// Convert the resource files to engine's format (if the PNGs exist)
		if ( core::Resources::MakePathTo(filename_sprite + ".png", resource_sprite) )
		{
			Textures::ConvertFile(resource_sprite, StringUtils::GetFileStemLeaf(resource_sprite) + ".bpd");
		}
		if ( core::Resources::MakePathTo(filename_palette + ".png", resource_palette) )
		{
			Textures::ConvertFile(resource_palette, StringUtils::GetFileStemLeaf(resource_palette)  + ".bpd");
		}
		if ( core::Resources::MakePathTo(filename_normals + ".png", resource_normals) )
		{
			Textures::ConvertFile(resource_normals, StringUtils::GetFileStemLeaf(resource_normals)  + ".bpd");
		}
		if ( core::Resources::MakePathTo(filename_surface + ".png", resource_surface) )
		{
			Textures::ConvertFile(resource_surface, StringUtils::GetFileStemLeaf(resource_surface)  + ".bpd");
		}
	}
	// Check for the GALs:
	{
		// Convert the resource files to engine's format (if the GALs exist)
		if ( core::Resources::MakePathTo(filename_sprite + ".gal", resource_sprite) )
		{
			Textures::timgInfo image_info;
			pixel_t* image;
			if (o_img_info != NULL && o_img_frametimes != NULL)
				image = Textures::loadGAL_Animation(resource_sprite, image_info, NULL); 
			else
				image = Textures::loadGAL(resource_sprite, image_info); 
			Textures::ConvertData(image, &image_info, StringUtils::GetFileStemLeaf(resource_sprite) + ".bpd");
			delete [] image;

			image = Textures::loadGAL_Layer(resource_sprite, "normals", image_info);
			if (image != NULL)
			{
				Textures::ConvertData(image, &image_info, StringUtils::GetFileStemLeaf(resource_sprite) + "_normal.bpd");
				delete [] image;
			}

			image = Textures::loadGAL_Layer(resource_sprite, "surface", image_info);
			if (image != NULL)
			{
				Textures::ConvertData(image, &image_info, StringUtils::GetFileStemLeaf(resource_sprite) + "_surface.bpd");
				delete [] image;
			}
		}
		if ( core::Resources::MakePathTo(filename_palette + ".gal", resource_palette) )
		{
			Textures::timgInfo image_info;
			pixel_t* image = Textures::loadGAL(resource_palette, image_info); 
			Textures::ConvertData(image, &image_info, StringUtils::GetFileStemLeaf(resource_palette) + ".bpd");
			delete [] image;
		}
	}
#endif// DEVELOPER_MODE

	// Now create BPD paths:
	// Require the sprite
	if ( !core::Resources::MakePathTo(filename_sprite + ".bpd", resource_sprite) )
	{
		throw core::MissingFileException();
	}
	// If no palette, then load the object in forward mode.
	if ( !core::Resources::MakePathTo(filename_palette + ".bpd", resource_palette) )
	{
		// Remove deferred pass from the shader so it only renders in forward mode
		m_material->deferredinfo.clear();

		// Load up the texture
		CTexture* new_texture = RESOURCE_GET_TEXTURE(
			n_sprite_filename,
			new CTexture (
				n_sprite_filename, 
				Texture2D, RGBA8,
				1024,1024, Clamp,Clamp,
				MipmapNone,SamplingPoint
			)
		);

		// Set output texture info
		if (o_img_info) *o_img_info = new_texture->GetIOImgInfo();

		// Set sprite info
		m_spriteInfo.fullsize.x = new_texture->GetWidth();
		m_spriteInfo.fullsize.y = new_texture->GetHeight();

		m_spriteInfo.framesize.x = new_texture->GetWidth();
		m_spriteInfo.framesize.y = new_texture->GetHeight();

		// Set the material based on the input file.
		m_material->setTexture(TEX_DIFFUSE, new_texture);

		// No longer need the texture in this object
		new_texture->RemoveReference();

		// Not a palette'd sprite.
		return;
	}

	// Check for paletted textures:
	CTexture* loaded_palette	= renderer::Resources::GetTexture(filename_palette);
	CTexture* loaded_sprite		= renderer::Resources::GetTexture(filename_sprite);
	CTexture* loaded_normals	= renderer::Resources::GetTexture(filename_normals);
	CTexture* loaded_surface	= renderer::Resources::GetTexture(filename_surface);

	if ( loaded_palette == NULL )
	{
		// Load the raw data from the palette first
		Textures::timgInfo imginfo_palette;
		pixel_t* raw_palette = Textures::LoadRawImageData(resource_palette, imginfo_palette);
		if ( raw_palette == NULL ) throw core::MissingDataException();

		// Set all data in the palette to have 255 alpha (opaque)
		for (int i = 0; i < imginfo_palette.width * imginfo_palette.height; ++i)
			raw_palette[i].a = 255;

		// Add the palette to the world's palette
		Render2D::WorldPalette::Active()->AddPalette( raw_palette, imginfo_palette.height, imginfo_palette.width );
	
		// No longer need local palette
		delete [] raw_palette;

		// Save dummy value
		renderer::Resources::AddTexture(filename_palette);
	}

	if ( loaded_sprite == NULL )
	{
		// Load the raw data from the sprite
		Textures::timgInfo imginfo_sprite;
		pixel_t* raw_sprite = Textures::LoadRawImageData(resource_sprite, imginfo_sprite);
		if ( raw_sprite == NULL ) throw core::MissingDataException();

		// Set output texture info
		if (o_img_info) *o_img_info = imginfo_sprite;

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
		m_material->setTexture(TEX_DIFFUSE, new_texture);

		// No longer need the texture in this object
		new_texture->RemoveReference();

		// Add it to manager
		renderer::Resources::AddTexture(filename_sprite, new_texture);


		// Create normal map texture
		if ( core::Resources::MakePathTo(filename_normals + ".bpd", resource_normals) )
		{
			CTexture* new_texture = new CTexture (
				resource_normals, 
				Texture2D, RGBA8,
				1024,1024, Clamp,Clamp,
				MipmapNone,SamplingPoint
			);

			m_material->setTexture(TEX_NORMALS, new_texture);

			// No longer need the texture in this object
			new_texture->RemoveReference();

			// Add it to manager
			renderer::Resources::AddTexture(filename_normals, new_texture);
		}
		else
		{
			CTexture* new_texture = new CTexture("");

			// Generate a normal map based on input parameters
			pixel_t* raw_normalmap = new pixel_t [imginfo_sprite.width * imginfo_sprite.height];
			Render2D::Preprocess::GenerateNormalMap( raw_sprite, raw_normalmap, imginfo_sprite.width, imginfo_sprite.height, m_spriteGenerationInfo.normal_default );

			// Upload the data
			new_texture->Upload(
				raw_normalmap,
				imginfo_sprite.width, imginfo_sprite.height, 
				Clamp, Clamp,
				MipmapNone, SamplingPoint
			);

			// Set this new normal map
			m_material->setTexture(TEX_NORMALS, new_texture);

			// Clear off the data now that it's on the GPU
			delete [] raw_normalmap;

			// No longer need the texture in this object
			new_texture->RemoveReference();

			// Add it to manager
			renderer::Resources::AddTexture(filename_normals, new_texture);
		}


		// Clear off the data now that it's on the GPU and we're done using it for generation
		delete [] raw_sprite;


		// Create surface map texture
		if ( core::Resources::MakePathTo(filename_surface + ".bpd", resource_surface) )
		{
			CTexture* new_texture = new CTexture (
				resource_surface, 
				Texture2D, RGBA8,
				1024,1024, Clamp,Clamp,
				MipmapNone,SamplingPoint
			);

			// Set this new surface map
			m_material->setTexture(TEX_SURFACE, new_texture);

			// No longer need the texture in this object
			new_texture->RemoveReference();

			// Add it to manager
			renderer::Resources::AddTexture(filename_surface, new_texture);
		}

	}
	else
	{
		CTexture* new_texture = loaded_sprite;

		// Set sprite info
		m_spriteInfo.fullsize.x = new_texture->GetWidth();
		m_spriteInfo.fullsize.y = new_texture->GetHeight();

		m_spriteInfo.framesize.x = new_texture->GetWidth();
		m_spriteInfo.framesize.y = new_texture->GetHeight();

		// Set the palette texture as the sprite
		m_material->setTexture(TEX_DIFFUSE, new_texture);
		// Set the normal map up as well
		if (loaded_normals) m_material->setTexture(TEX_NORMALS, loaded_normals);
		// Set the surface map up as well
		if (loaded_surface) m_material->setTexture(TEX_SURFACE, loaded_surface);
	}
}

//		GetSpriteInfo ()
// Returns read-only reference to the current sprite information structure.
const spriteInfo_t& CRenderable2D::GetSpriteInfo ( void )
{
	return m_spriteInfo;
}

//		SpriteGenParams ()
// Returns read-write reference to the sprite generation parameters
spriteGenParams2D_t& CRenderable2D::SpriteGenParams ( void )
{
	return m_spriteGenerationInfo;
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
	GL.Transform( &(transform.world) );
	m_material->m_bufferSkeletonSize = 0;
	m_material->m_bufferMatricesSkinning = 0;
	m_material->bindPass(pass);
	//parent->SendShaderUniforms(this);
	BindVAO( pass, m_buffer_verts, m_buffer_tris );
	GL.DrawElements( GL_TRIANGLES, m_modeldata.triangleNum*3, GL_UNSIGNED_INT, 0 );

	//GL.endOrtho();
	// Success!
	return true;
}