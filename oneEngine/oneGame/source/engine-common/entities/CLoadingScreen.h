#ifndef ENGINE_COMMON_C_LOADING_SCREEN_H_
#define ENGINE_COMMON_C_LOADING_SCREEN_H_

#include "core/containers/arsingleton.h"
#include "engine/behavior/CGameBehavior.h"

class CLoadScreenInjector;

class CLoadingScreen : public CGameBehavior
{
private:
	ENGCOM_API static CLoadingScreen* m_Active;
public:
	//	Active() : Get the currently active loading screen in the scene.
	// Returns NULL if there is no loading screen active.
	static CLoadingScreen * Active ( void ) 
	{ 
		return m_Active; 
	} 

public:
	ENGCOM_API explicit			CLoadingScreen ( void );
	ENGCOM_API					~CLoadingScreen ( void );

	// 1Engine update override
	ENGCOM_API void				Update ( void ) override;

	//	loadStep() : Call for each step of the loading
	// Will update the loading screen whenever this is called.
	ENGCOM_API void				loadStep ( void );
	//	loadSetDone() : Sets the loading as done
	// The loading screen will fade out and delete itself.
	ENGCOM_API void				loadSetDone ( void );

private:
	// Screen injector - renders loading screen
	CLoadScreenInjector*	m_injector;
	// Current alpha (fades out when done loading)
	Real					m_alpha;
	// Is loading done?
	bool					m_loadingDone;
};

#endif//ENGINE_COMMON_C_LOADING_SCREEN_H_