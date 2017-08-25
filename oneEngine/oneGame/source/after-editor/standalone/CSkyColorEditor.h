
#ifndef _C_SKY_COLOR_EDITOR_H_
#define _C_SKY_COLOR_EDITOR_H_

#include "engine/behavior/CGameBehavior.h"
#include "engine-common/dusk/CDuskGUIHandle.h"

class CCamera;
class CDuskGUI;
class Daycycle;

class CSkyColorEditor : public CGameBehavior
{

public:
						CSkyColorEditor ( void );
						~CSkyColorEditor ( void );
						// Update routine. Handles input and GUI work
	void				Update ( void );

private:
	void				CreateGUI ( void );

private:
	Daycycle*		m_daycycle;

	CCamera*		m_camera;

	CDuskGUI*		m_gui;
	struct sGUIParts
	{
		//Dusk::Handle	col_day;
		Dusk::Handle	slider_time;
		Dusk::Handle	btn_load;
		Dusk::Handle	btn_save;

		Dusk::Handle	col_clear_sunrise;
		Dusk::Handle	col_clear_morning;
		Dusk::Handle	col_clear_afternoon;
		Dusk::Handle	col_clear_sunset;
		Dusk::Handle	col_clear_evening;
		Dusk::Handle	col_clear_night;

		Dusk::Handle	col_light_sunrise;
		Dusk::Handle	col_light_morning;
		Dusk::Handle	col_light_afternoon;
		Dusk::Handle	col_light_sunset;
		Dusk::Handle	col_light_evening;
		Dusk::Handle	col_light_night;

		Dusk::Handle	col_sky_sunrise;
		Dusk::Handle	col_sky_morning;
		Dusk::Handle	col_sky_afternoon;
		Dusk::Handle	col_sky_sunset;
		Dusk::Handle	col_sky_evening;
		Dusk::Handle	col_sky_night;

		Dusk::Handle	col_sun_sunrise;
		Dusk::Handle	col_sun_morning;
		Dusk::Handle	col_sun_afternoon;
		Dusk::Handle	col_sun_sunset;
		Dusk::Handle	col_sun_evening;
		Dusk::Handle	col_sun_night;
	} m_guiparts;
};

#endif//_C_SKY_COLOR_EDITOR_H_