#ifndef RENDERER_SAMPLER_STATE_H_
#define RENDERER_SAMPLER_STATE_H_

#include "core/gfx/textureFormats.h"
#include "core/math/Color.h"

struct rrSamplerState
{
	core::gfx::tex::arSamplingFilter	filter;
	core::gfx::tex::arWrappingType		wrapX;
	core::gfx::tex::arWrappingType		wrapY;
	core::gfx::tex::arWrappingType		wrapZ;
	float								lodBias;
	uint8_t								maxAnisotropy;
	//uint8_t								comparison;
	Color								borderColor;
};

#endif//RENDERER_SAMPLER_STATE_H_