//===============================================================================================//
//
//		core/callback_basics.h
//
// Forgot where this was going
//
//===============================================================================================//
#ifndef CORE_ENGINE_CALLBACKS_
#define CORE_ENGINE_CALLBACKS_

// == CALLBACK DEFINITION ==
typedef int (*arCallback_Void)( void );
typedef int (*arCallback_Ptr)( void* );

typedef int (CGameBehavior::*arCallback_Class_Void)( void );
typedef int (CGameBehavior::*arCallback_Class_Ptr)( void* );

// Callback observer info
class CCallbackObserver
{

};
typedef int (CCallbackObserver::*arCallback_Clb_Void)( void );
typedef int (CCallbackObserver::*arCallback_Clb_Ptr)( void* );

#endif//CORE_ENGINE_CALLBACKS_