#include "renderer/object/util/CLoadScreenInjector.h"
#include "CLoadingScreen.h"

ARSINGLETON_CPP_DEF(CLoadingScreen);

CLoadingScreen::CLoadingScreen ( void )
	: CGameBehavior()
{
	m_injector = new CLoadScreenInjector();
	m_alpha = 1.0F;
	m_loadingDone = false;

	m_Active = this;
}

CLoadingScreen::~CLoadingScreen ( void )
{
	delete_safe(m_injector);

	if (m_Active == this)
	{
		m_Active = NULL;
	}
}
void CLoadingScreen::Update ( void )
{
	if (m_loadingDone)
	{
		m_alpha -= Time::deltaTime / 0.33F;
		m_injector->setAlpha(m_alpha);

		// Delete when 100% invis
		if (m_alpha <= -1.0F)
		{
			DeleteObject(this);
		}
	}
}

void CLoadingScreen::loadStep ( void )
{
	m_injector->StepScreen();
}

void CLoadingScreen::loadSetDone ( void )
{
	m_injector->StepScreen();
	m_loadingDone = true;
}