#ifndef UI_EVENTIDE_ELEMENT_STYLER_H_
#define UI_EVENTIDE_ELEMENT_STYLER_H_

#include "../Common.h"
#include "core/math/Color.h"
#include "core/math/Easing.h"

namespace ui {
namespace eventide {

	class Styler
	{
	public:
		struct StylerBoxInfo
		{
			Color			defaultColor	= Color(0.2, 0.2, 0.2, 1.0);
			Color			hoverColor		= Color(0.4, 0.4, 0.4, 1.0);
			Color			activeColor		= Color(0.6, 0.6, 0.6, 1.0);

			float			menuPadding		= 3.0F;
		};
		StylerBoxInfo		box;

		struct StylerTextInfo
		{
			const char*		font			= "YanoneKaffeesatz-B.otf";

			Color			headingColor	= Color(1.0, 1.0, 1.0, 1.0);
			float			headingSize		= 25.0F;

			Color			buttonColor		= Color(1.0, 1.0, 1.0, 1.0);
			Color			buttonDisabledColor	= Color(0.5, 0.5, 0.5, 1.0);
			float			buttonSize		= 15.0F;
		};
		StylerTextInfo		text;

		struct StylerTimingInfo
		{
			float			hoverFade		= 0.1F;
			float			clickPulse		= 0.4F;
		};
		StylerTimingInfo	timing;

	public:
		EVENTIDE_API static float
								PulseFade ( float t )
		{
			return easing::quadratic_out(t) * easing::exponential_out(math::saturate(4.0F - 4.0F * t));
		}
	};

	EVENTIDE_API extern Styler
						DefaultStyler;

}}

#endif//UI_EVENTIDE_ELEMENT_STYLER_H_
