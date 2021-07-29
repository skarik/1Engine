#ifndef RENDERER_TYPES_VIEWPORT_H_
#define RENDERER_TYPES_VIEWPORT_H_

#include "core/types/types.h"
#include "core/math/vect2d_template.h"
#include "renderer/types/types.h"

struct rrViewport
{
	Vector2i			corner = Vector2i(0, 0);
	Vector2i			size = Vector2i(0, 0);
	Vector2i			pixel_density = Vector2i(1, 1);
};

#endif//RENDERER_TYPES_VIEWPORT_H_