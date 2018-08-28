#ifndef RR_SCREENSHOT_H_
#define RR_SCREENSHOT_H_

#include "core/containers/arstring.h"

// Helper class for taking screenshots.
class RrScreenshot
{
public:
	RENDER_API void SaveTimestampedToPNG ( void );
	RENDER_API void SaveToPNG ( const arstring<256>& );
};


#endif//RR_SCREENSHOT_H_