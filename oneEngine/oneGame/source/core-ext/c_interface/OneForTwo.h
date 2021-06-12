//=oneengine/ext===============================================================================//
//
//	OneForTwo.h
//
// Straightforward C interface, targeting Game Maker Studio 2.
//
//===============================================================================================//
#ifndef CORE_EXT_C_INTERFACE_ONE_FOR_TWO_H_
#define CORE_EXT_C_INTERFACE_ONE_FOR_TWO_H_

#include "config.h"
#include "core/types.h"

#if ONEFORTWO_ENABLED

#ifndef AR_CEXPORT
#define AR_CEXPORT extern "C" AR_EXPORT
#endif

//
// Files management (Files.h)
//

AR_CEXPORT double AR_CALL FileGetSize ( const char* filename );
AR_CEXPORT double AR_CALL FileGetLastWriteTime ( const char* filename );

#endif//ONEFORTWO_ENABLED

#endif//CORE_EXT_C_INTERFACE_ONE_FOR_TWO_H_