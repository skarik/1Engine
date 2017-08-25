
#ifndef _THE_WON_ENGINE_CALLBACKS_
#define _THE_WON_ENGINE_CALLBACKS_

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

#endif//_THE_WON_ENGINE_CALLBACKS_