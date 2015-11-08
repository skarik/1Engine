

#include "Color.h"
#include "core/math/random/Random.h"

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
	lightness = ( fmax + fmin ) / 2;
	chroma = fmax - fmin;

	// compute hue
	if ( chroma < FTYPE_PRECISION )
		hue = 0;
	else if ( fmax == red )
		hue = (green-blue)/chroma;
	else if ( fmax == green )
		hue = (blue-red)/chroma + 2.0f;
	else if ( fmax = blue )
		hue = (red-green)/chroma + 4.0f;
	else
		hue = 0;
	hue *= 60;

	// compute saturation
	if (( chroma > 0 )&&( lightness < 1 ))
		saturation = chroma/(1-fabs(2*lightness-1));
	else
		saturation = 0;

	outHSL.red	 = hue;
	outHSL.green = saturation;
	outHSL.blue	 = lightness;
	outHSL.alpha = chroma;
}
void Color::SetHSL ( const Color& inHSL )
{
	Real hue = inHSL.red;
	Real saturation = inHSL.green;
	Real lightness = inHSL.blue;

	Real chroma = ( 1.0f - fabs( 2*lightness - 1.0f )) * saturation;
	Real hue_t = hue / 60;
	Real cx_t = chroma * ( 1 - fabs( ( (hue_t*2 - int(hue_t/2))*2 ) - 1.0f ) );
	Color color_t;
	if ( hue_t < 1 )
		color_t = Color( chroma, cx_t, 0 );
	else if ( hue_t < 2 )
		color_t = Color( cx_t, chroma, 0 );
	else if ( hue_t < 3 )
		color_t = Color( 0, chroma, cx_t );
	else if ( hue_t < 4 )
		color_t = Color( 0, cx_t, chroma );
	else if ( hue_t < 5 )
		color_t = Color( cx_t, 0, chroma );
	else if ( hue_t < 6 )
		color_t = Color( chroma, 0, cx_t );
	else
		color_t = Color( 0,0,0 );
	Real cm_t = lightness - chroma/2;
	red		= color_t.red	+ cm_t;
	green	= color_t.green + cm_t;
	blue	= color_t.blue	+ cm_t;
}