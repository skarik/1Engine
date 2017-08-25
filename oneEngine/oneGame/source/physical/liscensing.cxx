
#include "physical/mode.h"

//===============================================================================================//
// HAVOK LICENSING AND PLATFORM SETUP
//===============================================================================================//
#ifdef PHYSICS_USING_HAVOK

// Platform specific initialization
//#include <Common/Base/System/Init/PlatformInit.cxx>

// Include registration registration
#include <Common/Base/keycode.cxx>

// We're not using anything product specific yet. We undef these so we don't get the usual
// product initialization for the products.
#undef HK_FEATURE_PRODUCT_AI
//#undef HK_FEATURE_PRODUCT_ANIMATION		// TODO: comment out in future.
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_BEHAVIOR
//#undef HK_FEATURE_PRODUCT_PHYSICS

// Also we're not using any serialization/versioning so we don't need any of these.
#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
#define HK_EXCLUDE_FEATURE_RegisterVersionPatches
/*#ifndef _HAVOK_VISUAL_DEBUGGER_
#define HK_EXCLUDE_FEATURE_RegisterReflectedClasses //NEEDED FOR DEBUGGER
#endif*/
#define HK_EXCLUDE_FEATURE_MemoryTracker

// Serialization config
//#include <Common/Base/KeyCode.h>
//#define HK_CLASSES_FILE <Common/Serialize/ClassList/hkKeyCodeClasses.h>

// This include generates an initialization function based on the products
// and the excluded features.
#include <Common/Base/Config/hkProductFeatures.cxx>

#endif
//===============================================================================================//
// END HAVOK LICENSING AND PLATFORM SETUP
//===============================================================================================//
