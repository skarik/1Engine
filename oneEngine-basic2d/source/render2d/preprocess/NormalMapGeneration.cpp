#include "NormalMapGeneration.h"

#include "core/math/Math.h"
#include "core/math/vect2d_template.h"

//		GenerateNormalMap
// Given an image, default normal, and output buffer, generates a default normal map.
// The normals are pillow-shaded based on distance to the transparent edges.
void Render2D::Preprocess::GenerateNormalMap (
	const pixel_t* i_pixel_data, pixel_t* o_normal_map, 
	const uint n_image_width, const uint n_image_height,
	const Vector3d& n_default_normal
)
{
	const int filter_radius = 3;

	pixel_t pixel;
	int x, y;
	int xoffset, yoffset;
	int xoffsetmax;
	int xt, yt;
	const int max_x = (int)n_image_width;
	const int max_y = (int)n_image_height;
	const Vector3d n_normal_up		= Vector3d(n_default_normal.x, n_default_normal.y - 1.0F, n_default_normal.z * 0.5F).normal();
	const Vector3d n_normal_down	= Vector3d(n_default_normal.x, n_default_normal.y + 1.0F, n_default_normal.z * 0.5F).normal();
	const Vector3d n_normal_left	= Vector3d(n_default_normal.x - 1.0F, n_default_normal.y, n_default_normal.z * 0.5F).normal();
	const Vector3d n_normal_right	= Vector3d(n_default_normal.x + 1.0F, n_default_normal.y, n_default_normal.z * 0.5F).normal();
	Vector3d current_normal;
	Vector3d target_normal;
	Vector2i current_aggregate;

	for ( y = 0; y < max_y; ++y )
	{
		for ( x = 0; x < max_x; ++x )
		{
			// Reset normal
			current_normal = n_default_normal;
			current_aggregate = Vector2i(0,0);

			// Sample nearby pixels in a radius for an empty pixel
			for ( yoffset = -filter_radius; yoffset <= filter_radius; ++yoffset )
			{
				yt = y + yoffset;
				if ( yt < 0 || yt >= n_image_height ) continue; // Skip out of range values

				xoffsetmax = filter_radius - abs(yoffset);
				for ( xoffset = -xoffsetmax; xoffset <= xoffsetmax; ++xoffset )
				{
					xt = x + xoffset;
					if ( xt < 0 || xt >= n_image_width ) continue; // Skip out of range values

					// Look up pixel position
					pixel = i_pixel_data[xt + yt * n_image_width];

					// Is the pixel transparent?
					if ( pixel.a < 128 )
					{
						// Add in the current XY position
						current_aggregate.x += xoffset;
						current_aggregate.y += yoffset;
					}
				}
			}

			// Now we have an aggregate for the normal. Thus, we use that to generate our x-y strengths
			int32_t dist_x = Math::round( sqrtf( std::max<float>(0, abs(current_aggregate.x) - 1.0F) ) - 0.5F );
			int32_t dist_y = Math::round( sqrtf( std::max<float>(0, abs(current_aggregate.y) - 1.0F) ) - 0.5F );
			float strength_x = Math::saturate( dist_x / (float)filter_radius ) * Math::sgn( current_aggregate.x );
			float strength_y = Math::saturate( dist_y / (float)filter_radius ) * Math::sgn( current_aggregate.y );

			// Create the normal
			target_normal = Vector3d();
			
			if ( strength_x >= 0 )
				target_normal += n_normal_right * strength_x;
			else
				target_normal += n_normal_left * -strength_x;

			if ( strength_y >= 0 )
				target_normal += n_normal_down * strength_y;
			else
				target_normal += n_normal_up * -strength_y;

			current_normal = n_default_normal.lerp( target_normal, sqrtf(Math::square(strength_x) + Math::square(strength_y)) );

			// Normalize result
			current_normal.normalize();

			// Save current normal as pixel color
			pixel.r = (uint8_t)( (current_normal.x * 0.5F + 0.5F) * 255.0F + 0.5F );
			pixel.g = (uint8_t)( (current_normal.y * 0.5F + 0.5F) * 255.0F + 0.5F );
			pixel.b = (uint8_t)( (current_normal.z * 0.5F + 0.5F) * 255.0F + 0.5F );
			pixel.a = 255;
			o_normal_map[x + y * n_image_width] = pixel;
		}
	}

	// And now we got a normal map. How swaggy!
}