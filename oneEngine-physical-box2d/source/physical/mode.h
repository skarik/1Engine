
#ifndef _PHYSICS_MODE_
#define _PHYSICS_MODE_

#define bPHYSICS_BOX2D 0	
#undef PHYSICS_USING_BOX2D	// Disabled 2016 legacy
#define bPHYSICS_HAVOK 0	
#undef PHYSICS_USING_HAVOK	// Disabled 2014 legacy

#define PHYSICS_USING_BULLET
#define bPHYSICS_BULLET 1

#define PHYSICS_SINGLETHREADED
#undef PHYSICS_MULTITHREADED	// Don't use multithreading

#endif//_PHYSICS_MODE_
