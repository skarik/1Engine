#include "CRenderable2D.h"

#include "core/utils/string.h"
#include "core-ext/system/io/Resources.h"
#include "core-ext/system/io/assets/TextureIO.h"

#include "gpuw/Device.h"
#include "renderer/texture/RrTexture.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/Material.h"

#include "render2d/preprocess/PaletteToLUT.h"
#include "render2d/preprocess/NormalMapGeneration.h"
#include "render2d/preprocess/Conversion.h"
#include "render2d/state/WorldPalette.h"

CRenderable2D::CRenderable2D ( void )
	: CRenderableObject()
{
	// Set up default parameters
	m_spriteGenerationInfo = rrSpriteGenParams();
	m_spriteGenerationInfo.normal_default = Vector3f(0, 0, 1.0F);

	// Use a default 2D material
	RrPass spritePass;
	spritePass.utilSetupAsDefault();
	spritePass.m_type = kPassTypeForward;
	spritePass.m_alphaMode = renderer::kAlphaModeAlphatest;
	spritePass.m_cullMode = gpu::kCullModeNone;
	spritePass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	spritePass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
	spritePass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
	spritePass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureBlack) );
	spritePass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );
	spritePass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/fullbright_vv.spv", "shaders/sys/fullbright_p.spv"}) );
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor};
	spritePass.setVertexSpecificationByCommonList(t_vspec, 3);
	spritePass.m_primitiveType = gpu::kPrimitiveTopologyTriangleStrip;
	PassInitWithInput(0, &spritePass);

	// Start off with empty model data
	memset(&m_modeldata, 0, sizeof(m_modeldata));
	m_modeldata.indexNum = 0;
	m_modeldata.vertexNum = 0;
}

CRenderable2D::~CRenderable2D ()
{
	m_meshBuffer.FreeMeshBuffers();

	// Material reference released automatically
}

//		SetSpriteFile ( c-string sprite filename )
// Sets the sprite filename to load or convert. Uses resource manager to cache data.
void CRenderable2D::SetSpriteFile ( const char* n_sprite_resname, rrSpriteSetResult* o_set_result )
{
	SetSpriteFileAnimated( n_sprite_resname, NULL, o_set_result );
}
//		SetSpriteFileAnimated ( c-string sprite filename )
// Sets the sprite filename to load and possibly convert, but provides returns for additional information.
void CRenderable2D::SetSpriteFileAnimated ( const char* n_sprite_resname, core::gfx::tex::arSpriteInfo* o_sprite_info, rrSpriteSetResult* o_set_result )
{
	//// Create the filename for the files.
	//std::string filename_palette = core::utils::string::GetFileStemLeaf(n_sprite_filename) + "_pal";
	//std::string filename_sprite	 = core::utils::string::GetFileStemLeaf(n_sprite_filename);
	//std::string filename_normals = core::utils::string::GetFileStemLeaf(n_sprite_filename) + "_normal";
	//std::string filename_surface = core::utils::string::GetFileStemLeaf(n_sprite_filename) + "_surface";

	//if (n_palette_filename != NULL)
	//{
	//	filename_palette = core::utils::string::GetFileStemLeaf(n_palette_filename);
	//}

	//std::string resource_palette;
	//std::string resource_sprite;
	//std::string resource_normals;
	//std::string resource_surface;

	arstring256 resource_sprite (n_sprite_resname);
	arstring256 resource_palette = resource_sprite + "_palette";
	arstring256 resource_normals = resource_sprite + "_normals";
	arstring256 resource_surface = resource_sprite + "_surface";
	arstring256 resource_illumin = resource_sprite + "_illumin";

	render2d::preprocess::rrConvertSpriteResults convert_results = {};
	bool conversionResult = render2d::preprocess::ConvertSprite(n_sprite_resname, &convert_results);
	ARCORE_ASSERT(conversionResult == true);

//#if 1 // DEVELOPER_MODE
//	// Check for a JPG:
//	{
//		// Convert the resource files to engine's format (if the JPGs exist)
//		if ( core::Resources::MakePathTo(filename_sprite + ".jpg", resource_sprite) )
//		{
//			Textures::ConvertFile(resource_sprite, core::utils::string::GetFileStemLeaf(resource_sprite) + ".bpd");
//		}
//	}
//	// Check for the PNGs:
//	{
//		// Convert the resource files to engine's format (if the PNGs exist)
//		if ( core::Resources::MakePathTo(filename_sprite + ".png", resource_sprite) )
//		{
//			Textures::ConvertFile(resource_sprite, core::utils::string::GetFileStemLeaf(resource_sprite) + ".bpd");
//		}
//		if ( core::Resources::MakePathTo(filename_palette + ".png", resource_palette) )
//		{
//			Textures::ConvertFile(resource_palette, core::utils::string::GetFileStemLeaf(resource_palette)  + ".bpd");
//		}
//		if ( core::Resources::MakePathTo(filename_normals + ".png", resource_normals) )
//		{
//			Textures::ConvertFile(resource_normals, core::utils::string::GetFileStemLeaf(resource_normals)  + ".bpd");
//		}
//		if ( core::Resources::MakePathTo(filename_surface + ".png", resource_surface) )
//		{
//			Textures::ConvertFile(resource_surface, core::utils::string::GetFileStemLeaf(resource_surface)  + ".bpd");
//		}
//	}
//	// Check for the GALs:
//	{
//		// Convert the resource files to engine's format (if the GALs exist)
//		if ( core::Resources::MakePathTo(filename_sprite + ".gal", resource_sprite) )
//		{
//			Textures::timgInfo image_info;
//			pixel_t* image;
//			if (o_img_info != NULL && o_img_frametimes != NULL)
//				image = Textures::loadGAL_Animation(resource_sprite, image_info, NULL); 
//			else
//				image = Textures::loadGAL(resource_sprite, image_info); 
//			Textures::ConvertData(image, &image_info, core::utils::string::GetFileStemLeaf(resource_sprite) + ".bpd");
//			delete [] image;
//
//			image = Textures::loadGAL_Layer(resource_sprite, "normals", image_info);
//			if (image != NULL)
//			{
//				Textures::ConvertData(image, &image_info, core::utils::string::GetFileStemLeaf(resource_sprite) + "_normal.bpd");
//				delete [] image;
//			}
//
//			image = Textures::loadGAL_Layer(resource_sprite, "surface", image_info);
//			if (image != NULL)
//			{
//				Textures::ConvertData(image, &image_info, core::utils::string::GetFileStemLeaf(resource_sprite) + "_surface.bpd");
//				delete [] image;
//			}
//		}
//		if ( core::Resources::MakePathTo(filename_palette + ".gal", resource_palette) )
//		{
//			Textures::timgInfo image_info;
//			pixel_t* image = Textures::loadGAL(resource_palette, image_info); 
//			Textures::ConvertData(image, &image_info, core::utils::string::GetFileStemLeaf(resource_palette) + ".bpd");
//			delete [] image;
//		}
//	}
//#endif// DEVELOPER_MODE

	//arstring256 name_sprite = resource_sprite;
	//core::utils::string::ToResourceName(name_sprite.data, 256);
	////name_sprite = arstring256("rr2d_") + name_sprite;
	////arstring256 name_palette = name_sprite + "_palette";
	////arstring256 name_normals = name_sprite + "_normals";
	////arstring256 name_surface = name_sprite + "_surface";
	////arstring256 name_illumin = name_sprite + "_illumin";

	//// Load the sprite procedurally for the albedo/palette:
	//m_textureAlbedo = RrTexture::Find(name_sprite);
	//if (m_textureAlbedo == NULL)
	//{
	//	m_textureAlbedo = RrTexture::CreateUnitialized(name_sprite);

	//	// load the bpd now
	//	
	//	// pull the info?

	//	m_textureAlbedo->Upload(false,
	//		NULL,
	//		1, 1,
	//		core::gfx::tex::kColorFormatRGBA8,
	//		core::gfx::tex::kWrappingRepeat, core::gfx::tex::kWrappingRepeat,
	//		core::gfx::tex::kMipmapGenerationNone,
	//		core::gfx::tex::kSamplingPoint);
	//}
	//else
	//{
	//	//o_sprite_info->
	//}
	//// Load up the paletted texture
	//if (m_texturePalette == NULL)
	//{
	//	m_texturePalette = RrTexture::Find(name_palette);
	//	// Load a new palette and add it to the world.
	//	if (m_texturePalette == NULL)
	//	{
	//		m_texturePalette = RrTexture::Load(resource_palette);

	//		// Some things need a palette, some things dont.

	//		// Push the palette to the world (possibly?? improve this???)
	//		/*if (m_texturePalette != NULL)
	//		{
	//			// Load the raw data from the palette first
	//			Textures::timgInfo imginfo_palette;
	//			pixel_t* raw_palette = Textures::LoadRawImageData(resource_palette, imginfo_palette);
	//			if ( raw_palette == NULL ) throw core::MissingDataException();

	//			// Set all data in the palette to have 255 alpha (opaque)
	//			for (int i = 0; i < imginfo_palette.width * imginfo_palette.height; ++i)
	//				raw_palette[i].a = 255;

	//			// Add the palette to the world's palette
	//			render2d::WorldPalette::Active()->AddPalette( raw_palette, imginfo_palette.height, imginfo_palette.width );

	//			// Save dummy value
	//			renderer::Resources::AddTexture(filename_palette);
	//		}*/
	//	}
	//}

	// Load the palette:
	if (core::Resources::Exists(resource_palette))
	{
		m_texturePalette = RrTexture::Load(resource_palette);
		m_textureAlbedo = RrTexture::Load(resource_sprite);
	}
	else
	{
		// The palette may be part of the bitmap:
		m_textureAlbedo = RrTexture::Load(resource_sprite);
		if (m_textureAlbedo)
		{
			//m_texturePalette = m_textureAlbedo->GetPalette();
			//TODO
		}
	}

	// Load the other textures using the normal sequence
	m_textureNormals = RrTexture::Load(resource_normals);
	m_textureSurface = RrTexture::Load(resource_surface);
	m_textureIllumin = RrTexture::Load(resource_illumin);

	// Output the sprite info
	if (o_sprite_info != NULL)
	{
		if (m_textureAlbedo->GetExtraInfo().spriteInfo != NULL)
		{
			*o_sprite_info = *m_textureAlbedo->GetExtraInfo().spriteInfo;
		}
	}

	// Set the palette texture as the sprite
	if (m_textureAlbedo)  PassAccess(0).setTexture(TEX_DIFFUSE, m_textureAlbedo);
	if (m_textureNormals) PassAccess(0).setTexture(TEX_NORMALS, m_textureNormals);
	if (m_textureSurface) PassAccess(0).setTexture(TEX_SURFACE, m_textureSurface);
	if (m_textureIllumin) PassAccess(0).setTexture(TEX_OVERLAY, m_textureIllumin);

	// Set output result
	if (o_set_result != NULL)
	{
		o_set_result->textureAlbedo = m_textureAlbedo;
		o_set_result->textureNormals = m_textureNormals;
		o_set_result->textureSurface = m_textureSurface;
		o_set_result->textureIllumin = m_textureIllumin;
		
		o_set_result->texturePalette = m_texturePalette;
	}

	/*m_material->setTexture(TEX_DIFFUSE, new_texture);
	// Set the normal map up as well
	if (loaded_normals) m_material->setTexture(TEX_NORMALS, loaded_normals);
	// Set the surface map up as well
	if (loaded_surface) m_material->setTexture(TEX_SURFACE, loaded_surface);*/

	// Now create BPD paths:
	// Require the sprite
	//if ( !core::Resources::MakePathTo(filename_sprite + ".bpd", resource_sprite) )
	//{
	//	throw core::MissingFileException();
	//}
	//// If no palette, then load the object in forward mode.
	//if ( !core::Resources::MakePathTo(filename_palette + ".bpd", resource_palette) )
	//{
	//	// Remove deferred pass from the shader so it only renders in forward mode
	//	m_material->deferredinfo.clear();

	//	// Load up the texture
	//	RrTexture* new_texture = RESOURCE_GET_TEXTURE(
	//		n_sprite_filename,
	//		new RrTexture (
	//			n_sprite_filename, 
	//			Texture2D, RGBA8,
	//			1024,1024, Clamp,Clamp,
	//			MipmapNone,SamplingPoint
	//		)
	//	);

	//	// Set output texture info
	//	if (o_img_info) *o_img_info = new_texture->GetIOImgInfo();

	//	// Set sprite info
	//	m_spriteInfo.fullsize.x = new_texture->GetWidth();
	//	m_spriteInfo.fullsize.y = new_texture->GetHeight();

	//	m_spriteInfo.framesize.x = new_texture->GetWidth();
	//	m_spriteInfo.framesize.y = new_texture->GetHeight();

	//	// Set the material based on the input file.
	//	m_material->setTexture(TEX_DIFFUSE, new_texture);

	//	// No longer need the texture in this object
	//	new_texture->RemoveReference();

	//	// Not a palette'd sprite.
	//	return;
	//}

	//// Check for paletted textures:
	//RrTexture* loaded_palette	= NULL;//renderer::Resources::GetTexture(filename_palette);
	//RrTexture* loaded_sprite	= RrTexture::Load(resource_sprite.c_str());
	//RrTexture* loaded_normals	= RrTexture::Load(resource_normals.c_str());
	//RrTexture* loaded_surface	= RrTexture::Load(resource_surface.c_str());
	////RrTexture* loaded_sprite	= renderer::Resources::GetTexture(filename_sprite);
	////RrTexture* loaded_normals	= renderer::Resources::GetTexture(filename_normals);
	////RrTexture* loaded_surface	= renderer::Resources::GetTexture(filename_surface);

	//if ( loaded_palette == NULL )
	//{
	//	// Load the raw data from the palette first
	//	Textures::timgInfo imginfo_palette;
	//	pixel_t* raw_palette = Textures::LoadRawImageData(resource_palette, imginfo_palette);
	//	if ( raw_palette == NULL ) throw core::MissingDataException();

	//	// Set all data in the palette to have 255 alpha (opaque)
	//	for (int i = 0; i < imginfo_palette.width * imginfo_palette.height; ++i)
	//		raw_palette[i].a = 255;

	//	// Add the palette to the world's palette
	//	Render2D::WorldPalette::Active()->AddPalette( raw_palette, imginfo_palette.height, imginfo_palette.width );
	//
	//	// No longer need local palette
	//	delete [] raw_palette;

	//	// Save dummy value
	//	renderer::Resources::AddTexture(filename_palette);
	//}

	////if ( loaded_sprite == NULL )
	////{
	////	// Load the raw data from the sprite
	////	Textures::timgInfo imginfo_sprite;
	////	pixel_t* raw_sprite = Textures::LoadRawImageData(resource_sprite, imginfo_sprite);
	////	if ( raw_sprite == NULL ) throw core::MissingDataException();

	////	// Set output texture info
	////	if (o_img_info) *o_img_info = imginfo_sprite;

	////	// Convert the colors to the internal world's palette
	////	render2d::preprocess::DataToLUT(
	////		raw_sprite, imginfo_sprite.width * imginfo_sprite.height,
	////		Render2D::WorldPalette::Active()->palette_data, Render2D::WorldPalette::MAX_HEIGHT, Render2D::WorldPalette::Active()->palette_width);

	////	// Create empty texture to upload data into
	////	RrTexture* new_texture = new RrTexture("");

	////	// Upload the data
	////	new_texture->Upload(
	////		raw_sprite,
	////		imginfo_sprite.width, imginfo_sprite.height, 
	////		Clamp, Clamp,
	////		MipmapNone, SamplingPoint
	////	);

	////	// Set sprite info
	////	m_spriteInfo.fullsize.x = new_texture->GetWidth();
	////	m_spriteInfo.fullsize.y = new_texture->GetHeight();

	////	m_spriteInfo.framesize.x = new_texture->GetWidth();
	////	m_spriteInfo.framesize.y = new_texture->GetHeight();

	////	// Set the palette texture as the sprite
	////	m_material->setTexture(TEX_DIFFUSE, new_texture);

	////	// No longer need the texture in this object
	////	new_texture->RemoveReference();

	////	// Add it to manager
	////	renderer::Resources::AddTexture(filename_sprite, new_texture);


	////	// Create normal map texture
	////	if ( core::Resources::MakePathTo(filename_normals + ".bpd", resource_normals) )
	////	{
	////		RrTexture* new_texture = new RrTexture (
	////			resource_normals, 
	////			Texture2D, RGBA8,
	////			1024,1024, Clamp,Clamp,
	////			MipmapNone,SamplingPoint
	////		);

	////		m_material->setTexture(TEX_NORMALS, new_texture);

	////		// No longer need the texture in this object
	////		new_texture->RemoveReference();

	////		// Add it to manager
	////		renderer::Resources::AddTexture(filename_normals, new_texture);
	////	}
	////	else
	////	{
	////		RrTexture* new_texture = new RrTexture("");

	////		// Generate a normal map based on input parameters
	////		pixel_t* raw_normalmap = new pixel_t [imginfo_sprite.width * imginfo_sprite.height];
	////		render2d::preprocess::GenerateNormalMap( raw_sprite, raw_normalmap, imginfo_sprite.width, imginfo_sprite.height, m_spriteGenerationInfo.normal_default );

	////		// Upload the data
	////		new_texture->Upload(
	////			raw_normalmap,
	////			imginfo_sprite.width, imginfo_sprite.height, 
	////			Clamp, Clamp,
	////			MipmapNone, SamplingPoint
	////		);

	////		// Set this new normal map
	////		m_material->setTexture(TEX_NORMALS, new_texture);

	////		// Clear off the data now that it's on the GPU
	////		delete [] raw_normalmap;

	////		// No longer need the texture in this object
	////		new_texture->RemoveReference();

	////		// Add it to manager
	////		renderer::Resources::AddTexture(filename_normals, new_texture);
	////	}


	////	// Clear off the data now that it's on the GPU and we're done using it for generation
	////	delete [] raw_sprite;


	////	// Create surface map texture
	////	if ( core::Resources::MakePathTo(filename_surface + ".bpd", resource_surface) )
	////	{
	////		RrTexture* new_texture = new RrTexture (
	////			resource_surface, 
	////			Texture2D, RGBA8,
	////			1024,1024, Clamp,Clamp,
	////			MipmapNone,SamplingPoint
	////		);

	////		// Set this new surface map
	////		m_material->setTexture(TEX_SURFACE, new_texture);

	////		// No longer need the texture in this object
	////		new_texture->RemoveReference();

	////		// Add it to manager
	////		renderer::Resources::AddTexture(filename_surface, new_texture);
	////	}

	////}
	////else
	//{
	//	RrTexture* new_texture = loaded_sprite;

	//	// Set sprite info
	//	m_spriteInfo.fullsize.x = new_texture->GetWidth();
	//	m_spriteInfo.fullsize.y = new_texture->GetHeight();

	//	m_spriteInfo.framesize.x = new_texture->GetWidth();
	//	m_spriteInfo.framesize.y = new_texture->GetHeight();

	//	// Set the palette texture as the sprite
	//	m_material->setTexture(TEX_DIFFUSE, new_texture);
	//	// Set the normal map up as well
	//	if (loaded_normals) m_material->setTexture(TEX_NORMALS, loaded_normals);
	//	// Set the surface map up as well
	//	if (loaded_surface) m_material->setTexture(TEX_SURFACE, loaded_surface);
	//}

	//// Remove forward pass to save memory
	//m_material->passinfo.clear();
}

//		GetSpriteInfo ()
// Returns read-only reference to the current sprite information structure.
const render2d::rrSpriteInfo& CRenderable2D::GetSpriteInfo ( void )
{
	return m_spriteInfo;
}

//		SpriteGenParams ()
// Returns read-write reference to the sprite generation parameters
rrSpriteGenParams& CRenderable2D::SpriteGenParams ( void )
{
	return m_spriteGenerationInfo;
}

//		PushModelData()
// Takes the information inside of m_modeldata and pushes it to the GPU so that it may be rendered.
void CRenderable2D::PushModeldata ( void )
{
	m_meshBuffer.InitMeshBuffers(&m_modeldata);
}

//		PreRender()
// Push the uniform properties
bool CRenderable2D::PreRender ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject(transform.world, cameraPass);
	return true;
}

//		Render()
// Render the model using the 2D engine's style
bool CRenderable2D::Render ( const rrRenderParams* params )
{
	//// Do not render if no buffer to render with
	//if ( m_buffer_verts == 0 || m_buffer_tris == 0 )
	//{
	//	return true;
	//}

	//// For now, we will render the same way as the 3d meshes render
	////GL.Transform( &(transform.world) );
	//m_material->m_bufferSkeletonSize = 0;
	//m_material->m_bufferMatricesSkinning = 0;
	//m_material->bindPass(pass);
	////parent->SendShaderUniforms(this);
	//BindVAO( pass, m_buffer_verts, m_buffer_tris );
	//GL.DrawElements( GL_TRIANGLES, m_modeldata.triangleNum*3, GL_UNSIGNED_INT, 0 );

	////GL.endOrtho();
	//// Success!

	// otherwise we will render the same way 3d meshes render
	{
		if ( !m_meshBuffer.m_mesh_uploaded )
			return true; // Only render when have a valid mesh and rendering enabled

		gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

		gpu::Pipeline* pipeline = GetPipeline( params->pass );
		gfx->setPipeline(pipeline);
		// Set up the material helper...
		renderer::Material(this, gfx, params->pass, pipeline)
			// set the depth & blend state registers
			.setDepthStencilState()
			.setRasterizerState()
			// bind the samplers & textures
			.setBlendState()
			.setTextures();
		// bind the vertex buffers
		for (int i = 0; i < renderer::shader::kVBufferSlotMaxCount; ++i)
			if (m_meshBuffer.m_bufferEnabled[i])
				gfx->setVertexBuffer(i, &m_meshBuffer.m_buffer[i], 0);
		// bind the index buffer
		gfx->setIndexBuffer(&m_meshBuffer.m_indexBuffer, gpu::kIndexFormatUnsigned16);
		// bind the cbuffers
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
		// draw now
		gfx->drawIndexed(m_modeldata.indexNum, 0, 0);
	}

	return true;
}