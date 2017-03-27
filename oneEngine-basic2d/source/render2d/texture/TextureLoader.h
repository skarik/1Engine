
#ifndef _RENDER2D_TEXTURE_LOADER_H_
#define _RENDER2D_TEXTURE_LOADER_H_

#include "renderer/texture/TextureLoader.h"

namespace Textures
{
	// Actual conversions
	RENDER2D_API pixel_t* loadGAL ( const std::string& n_inputfile, timgInfo& o_info );
	// todo: load animation
	RENDER2D_API pixel_t* loadGAL_Layer ( const std::string& n_inputfile, const std::string& n_layername, timgInfo& o_info );

	// load animation
	RENDER2D_API pixel_t* loadGAL_Animation ( const std::string& n_inputfile, timgInfo& o_info, Real** o_frametimes );
}

#endif//_RENDER2D_TEXTURE_LOADER_H_