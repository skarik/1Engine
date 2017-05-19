//===============================================================================================//
//
//		Easing.h
//
// Provides a shitton of easing functions (see http://easings.net/ if you're a noob) within the
//	easing:: namespace.
//
//===============================================================================================//
#ifndef CORE_MATH_EASING_H_
#define CORE_MATH_EASING_H_

#include "core/types/types.h"
#include "core/types/Real.h"

#include <math.h>

//
// The following easing functions are open source under the BSD License.
//
//
// Copyright 2001 Robert Penner. All rights reserved.
//
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//
//	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//
//  - Neither the name of the author nor the names of contributors may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
// WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

// For a visual example of what each of these functions do, see http://easings.net/

namespace easing
{

	// Linear interpolation. No easing.
	inline
		FORCE_INLINE Real linear ( Real t )
	{
		return t;
	}


	// Quadratic interpolation (t^2). Eases in.
	inline
		FORCE_INLINE Real quadratic_in ( Real t )
	{
		return t * t;
	}
	// Quadratic interpolation (t^2). Eases out.
	inline
		FORCE_INLINE Real quadratic_out ( Real t )
	{
		return -t * (t - 2.0F);
	}
	// Quadratic interpolation (t^2). Eases in and out.
	inline
		FORCE_INLINE Real quadratic_inout ( Real t )
	{
		if ( t < 0.5F )
		{
			return 2.0F * t * t;
		}
		else
		{
			return (-2.0F * t * t) + (4.0F * t) - 1.0F;
		}
	}


	inline
		FORCE_INLINE Real cubic_in ( Real t )
	{
		return t * t * t;
	}
	inline
		FORCE_INLINE Real cubic_out ( Real t )
	{
		Real tr = t - 1.0F;
		return tr * tr * tr + 1.0F;
	}
	inline
		FORCE_INLINE Real cubic_inout ( Real t )
	{
		if ( t < 0.5F )
		{
			return 4.0F * cubic_in(t);
		}
		else
		{
			Real tr = (2.0F * t) - 2.0F;
			return 0.5F * cubic_in(tr) + 1.0F;
		}
	}


	inline
		FORCE_INLINE Real quartic_in ( Real t )
	{
		return t * t * t * t;
	}
	inline
		FORCE_INLINE Real quartic_out ( Real t )
	{
		Real tr = t - 1.0F;
		return t * t * t * (1.0F - t) + 1.0F;
	}
	inline
		FORCE_INLINE Real quartic_inout ( Real t )
	{
		if ( t < 0.5F )
		{
			return 8.0F * quartic_in(t);
		}
		else
		{
			Real tr = t - 1.0F;
			return -8.0F * quartic_in(tr) + 1.0F;
		}
	}


	inline
		FORCE_INLINE Real quintic_in ( Real t )
	{
		return t * t * t * t * t;
	}
	inline
		FORCE_INLINE Real quintic_out ( Real t )
	{
		Real tr = t - 1.0F;
		return tr * tr * tr * tr * tr + 1.0F;
	}
	inline
		FORCE_INLINE Real quintic_inout ( Real t )
	{
		if ( t < 0.5F )
		{
			return 16.0F * quintic_in(t);
		}
		else
		{
			Real tr = (2.0F * t) - 2.0F;
			return  0.5F * quintic_in(tr) + 1.0F;
		}
	}


	// Sine quarter-cycle. Eases in.
	inline
		FORCE_INLINE Real sine_in ( Real t )
	{
		return sinf( (t - 1.0F) * (Real)PI_2 ) + 1.0F;
	}
	// Sine quater-cycle. Eases out.
	inline
		FORCE_INLINE Real sine_out ( Real t )
	{
		return sinf( t * (Real)PI_2 );
	}
	// Sine half-cycle. Smooth in and smooth out.
	inline
		FORCE_INLINE Real sine_inout ( Real t )
	{
		return 0.5F * (1.0F - cosf(t * (Real)PI));
	}


	inline
		FORCE_INLINE Real circular_in ( Real t )
	{
		return 1.0F - sqrtf(1.0F - sqr(t));
	}
	inline
		FORCE_INLINE Real circular_out ( Real t )
	{
		return sqrtf((2.0F - t) * t);
	}
	inline
		FORCE_INLINE Real circular_inout ( Real t )
	{
		if ( t < 0.5F )
		{
			return 0.5F * (1.0F - sqrtf(1.0F - 4.0F * sqr(t)));
		}
		else
		{
			return 0.5F * (sqrtf(-((2.0F * t) - 3.0F) * ((2.0F * t) - 1.0F)) + 1.0F);
		}
	}


	inline
		FORCE_INLINE Real exponential_in ( Real t )
	{
		return (t == 0.0F) ? t : powf(2.0F, 10.0F * (t - 1.0F));
	}
	inline
		FORCE_INLINE Real exponential_out ( Real t )
	{
		return (t == 1.0F) ? t : (1.0F - powf(2.0F, -10.0F * t));
	}
	inline
		FORCE_INLINE Real exponential_inout ( Real t )
	{
		if ( t == 0.0F || t == 1.0F ) return t;

		if ( t < 0.5F )
		{
			return 0.5F * pow(2.0F, (20.0F * t) - 10.0F);
		}
		else
		{
			return -0.5F * pow(2.0F, (-20.0F * t) + 10.0F) + 1.0F;
		}
	}


	inline
		FORCE_INLINE Real elastic_in ( Real t )
	{
		return sinf(13.0F * (Real)PI_2 * t) * powf(2.0F, 10.0F * (t - 1.0F));
	}
	inline
		FORCE_INLINE Real elastic_out ( Real t )
	{
		return sinf(-13.0F * (Real)PI_2 * (t + 1.0F)) * powf(2.0F, -10.0F * t) + 1.0F;
	}
	inline
		FORCE_INLINE Real elastic_inout ( Real t )
	{
		if ( t < 0.5F )
		{
			return 0.5F * sinf(13.0F * (Real)PI_2 * (2.0F * t)) * powf(2.0F, 10.0F * ((2.0F * t) - 1.0F));
		}
		else
		{
			// TODO: Check this line
			return 0.5F * (sinf(-13 * (Real)PI_2 * ((2.0F * t - 1.0F) + 1.0F)) * powf(2.0F, -10.0F * (2.0F * t - 1.0F)) + 2.0F);
		}
	}


	inline
		FORCE_INLINE Real back_in ( Real t )
	{
		return t * t * t - t * sinf(t * (Real)PI);
	}
	inline
		FORCE_INLINE Real back_out ( Real t )
	{
		Real tr = 1.0F - t;
		return 1.0F - back_in(tr);
	}
	inline
		FORCE_INLINE Real back_inout ( Real t )
	{
		if ( t < 0.5F )
		{
			Real tr = 2.0F * t;
			return 0.5F * back_in(tr);
		}
		else
		{
			Real tr = 1.0F - (2.0F * t - 1.0F);
			return 0.5F * (1.0F - back_in(tr)) + 0.5F;
		}
	}


	inline
		FORCE_INLINE Real bounce_out ( Real t )
	{
		if ( t < 4/11.0F )
		{
			return (121 * sqr(t)) / 16.0F;
		}
		else if ( t < 8/11.0F )
		{
			return (363/40.0F * sqr(t)) - (99/10.0F * t) + 17/5.0F;
		}
		else if ( t < 9/10.0F )
		{
			return (4356/361.0F * sqr(t)) - (35442/1805.0F * t) + 16061/1805.0F;
		}
		else
		{
			return (54/5.0F * sqr(t)) - (513/25.0F * t) + 268/25.0F;
		}
	}
	inline
		FORCE_INLINE Real bounce_in ( Real t )
	{
		return 1.0F - bounce_out( 1.0F - t );
	}
	inline
		FORCE_INLINE Real bounce_inout ( Real t )
	{
		if ( t < 0.5F )
		{
			return 0.5F * bounce_in( t * 2 );
		}
		else
		{
			return 0.5F * bounce_out( t * 2 - 1 ) + 0.5F;
		}
	}


}

#endif//CORE_MATH_EASING_H_