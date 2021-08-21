#include "core/system/io/FileUtils.h"
#include "core/utils/string.h"
#include "core/debug/console.h"
#include "core-ext/system/io/Resources.h"
#include "core-ext/resources/ResourceManager.h"

#include "renderer/state/RrRenderer.h"
#include "RrTexture3D.h"
#include "RrTextureMaster.h"

// TODO: Load3D.
// TODO: LoadAtlas. It needs a unique streamed behavior where it loads lower mip levels of all layers, then works its way up.
//                  The atlas is loaded as a 2D texture array in order to ensure proper mipmap behavior.

//	CreateUnitialized ( name ) : Creates an uninitialized texture object.
// Can be used for procedural textures, with Upload(...) later.
RrTexture3D*
RrTexture3D::CreateUnitialized ( const char* name )
{
	arstring256 resource_str_id (name);

	// We need to create a new texture:
	RrTexture3D* texture = new RrTexture3D(resource_str_id, NULL);

	// We don't add it to the resource system yet. For now, we just return it.

	return texture;
}

RrTexture3D::RrTexture3D (
	const char* s_resourceId,
	const char* s_resourcePath
	)
	: RrTexture(s_resourceId, s_resourcePath)
{
	// All handled by RrTexture.
}

//	Upload(...) : Upload data to the texture, initializing it.
// Use for procedural textures.
void
RrTexture3D::Upload (
	bool							streamed,
	void*							data,
	uint16_t						width,
	uint16_t						height,
	uint16_t						depth,
	core::gfx::tex::arColorFormat	format,
	core::gfx::tex::arWrappingType	repeatX,
	core::gfx::tex::arWrappingType	repeatY,
	core::gfx::tex::arWrappingType	repeatZ,
	core::gfx::tex::arMipmapGenerationStyle	mipmapGeneration,
	core::gfx::tex::arSamplingFilter defaultSamplerFilter
)
{
	auto resm = core::ArResourceManager::Active();

	if (!procedural)
	{
		debug::Console->PrintError("RrTexture3D.cpp : trying to upload on nonproc texture");
		return;
	}

	if (upload_request == NULL) {
		upload_request = new rrTextureUploadInfo();
	}
	upload_request->type = core::gfx::tex::kTextureType3D;
	upload_request->data = data;
	upload_request->width = width;
	upload_request->height = height;
	upload_request->depth = depth;
	upload_request->format = format;

	// Update the info with the input params
	info.repeatX = repeatX;
	info.repeatY = repeatY;
	info.repeatZ = repeatZ;
	info.mipmapStyle = mipmapGeneration;
	info.filter = defaultSamplerFilter;

	// Set streamed setting before adding self to resource system.
	this->streamed = streamed;
	// Add self to the resource system:
	if (resm->Contains(this) == false)
		resm->Add(this);
	else // Or force an update:
		Reload();
}

RrTexture3D::~RrTexture3D ( void )
{
	// All handled by RrTexture.
}
