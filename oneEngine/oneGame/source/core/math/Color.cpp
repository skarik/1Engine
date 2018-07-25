#include "Vector2d.h"
#include "Vector3d.h"
#include "Vector4d.h"
#include "Quaternion.h"
#include "core/math/random/Random.h"

#include "Color.h"

// Implicit cast to Vector4.
Color::operator Vector4d()
{
	return Vector4d(x, y, z, w);
}

//Randomize the color
void Color::randomize (void)
{
	red = (Real)(Random.Next() % 256) / 255.0f;
	green = (Real)(Random.Next() % 256) / 255.0f;
	blue = (Real)(Random.Next() % 256) / 255.0f;
	alpha = (Real)(Random.Next() % 256) / 255.0f;
}


//===============================================================================================//
// HSL + INT Conversion
//===============================================================================================//
void Color::GetHSLC ( Color& outHSL ) const
{
	Real hue, saturation, lightness;
	Real chroma;

	// compute lightness
	Real fmax, fmin;
	fmax = std::max<Real>( std::max<Real>( red, green ), blue );
	fmin = std::min<Real>( std::min<Real>( red, green ), blue );
	lightness	= (fmax + fmin) / 2.0F;
	chroma		= fmax - fmin;

	if ( fmax == fmin )
	{
		hue = 0.0F;
		saturation = 0.0F;
	}
	else
	{
		// compute hue
			 if ( fmax == red )
			hue = (green - blue) / chroma + (green < blue ? 6.0F : 0.0F);
		else if ( fmax == green )
			hue = (blue - red)   / chroma + 2.0f;
		else if ( fmax == blue )
			hue = (red - green)  / chroma + 4.0f;
		else
			hue = 0;
		hue /= 6.0F;

		// compute saturation
		if ( chroma > 0.0F && lightness < 1.0F )
			saturation = chroma / (1.0F - fabs(2.0F * lightness - 1.0F));
		else
			saturation = 0.0F;
	}

	outHSL.red	 = hue;
	outHSL.green = saturation;
	outHSL.blue	 = lightness;
	outHSL.alpha = chroma;
}

FORCE_INLINE Real HueToRgb ( Real p, Real q, Real t )
{
	if (t < 0.0F)
		t += 1.0F;
	if (t > 1.0F)
		t -= 1.0F;
	if (t < 1.0F / 6.0F)
		return p + (q - p) * 6.0F * t;
	if (t < 1.0F / 2.0F)
		return q;
	if (t < 2.0F / 3.0F)
		return p + (q - p) * (2.0F / 3.0F - t) * 6.0F;
	return p;
}
void Color::SetHSL ( const Color& inHSL )
{
	Real hue = inHSL.red;
	Real saturation = inHSL.green;
	Real lightness = inHSL.blue;

	if ( saturation > 0.0F )
	{
		Real chroma = lightness < 0.5f ? (lightness * (1 + saturation)) : lightness + saturation - lightness * saturation;
		//Real chroma = ( 1.0f - fabs( 2*lightness - 1.0f )) * saturation;

		float p = 2 * lightness - chroma;
		red		= HueToRgb(p, chroma, hue + 1.0F / 3.0F);
		green	= HueToRgb(p, chroma, hue);
		blue	= HueToRgb(p, chroma, hue - 1.0F / 3.0F);
	}
	else
	{
		red		= lightness;
		green	= lightness;
		blue	= lightness;
	}
}