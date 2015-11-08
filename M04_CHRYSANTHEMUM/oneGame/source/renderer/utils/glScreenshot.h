// Helper class for taking screenshots.

#ifndef _GL_SCREENSHOT_H_
#define _GL_SCREENSHOT_H_

#include "core/containers/arstring.h"

class glScreenshot
{
public:
	RENDER_API void SaveTimestampedToPNG ( void );
	RENDER_API void SaveToPNG ( const arstring<256>& );
};


#endif//_GL_SCREENSHOT_H_