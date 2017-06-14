
#include "CSkyColorEditor.h"

#include "renderer/camera/CCamera.h"
#include "core/input/CInput.h"
#include "engine-common/dusk/CDuskGUI.h"
#include "core/system/Screen.h"
#include "after/entities/world/environment/DayAndNightCycle.h"

#include "renderer/debug/CDebugDrawer.h"
#include "core/system/System.h"

#include "after/states/CWorldState.h"

#include "renderer/texture/CBitmapFont.h"
#include "core/system/io/CBinaryFile.h"
//#include "EngineIO.h"

CSkyColorEditor::CSkyColorEditor ( void )
{
	m_daycycle	= NULL;

	m_camera	= new CCamera();
	m_gui		= new CDuskGUI();
	CreateGUI();
}

CSkyColorEditor::~CSkyColorEditor ( void )
{
	delete_safe( m_daycycle );
	delete_safe( m_camera );
	delete_safe( m_gui );
}

void CSkyColorEditor::CreateGUI ( void )
{
	// Create and config GUI
	Dusk::Handle prntHandle;
	Dusk::Handle tempHandle;

	m_gui->SetDefaultFont( new CBitmapFont( "YanoneKaffeesatz-R.otf", 13 ) );

	prntHandle = m_gui->CreateDraggablePanel();
	prntHandle.SetRect( Rect( 0.03f, 0.04f, 0.27f, 0.68f ) );
	prntHandle.SetText( "Skybox" );
	{
		m_guiparts.slider_time = m_gui->CreateSlider( prntHandle );
		m_guiparts.slider_time.SetRect( Rect( 0.05f, 0.10f, 0.23f, 0.04f ) );
		m_gui->SetSliderMinMax( m_guiparts.slider_time, 0, 86400 );

		tempHandle = m_gui->CreateText( prntHandle );
		tempHandle.SetRect( Rect( 0.05f, 0.15f, 0,0 ) );
		tempHandle.SetText( "Clear" );

		m_guiparts.col_clear_sunrise = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_clear_sunrise.SetRect( Rect( 0.05f, 0.20f, 0.04f, 0.04f ) );
		m_guiparts.col_clear_morning = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_clear_morning.SetRect( Rect( 0.05f, 0.25f, 0.04f, 0.04f ) );
		m_guiparts.col_clear_afternoon = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_clear_afternoon.SetRect( Rect( 0.05f, 0.30f, 0.04f, 0.04f ) );
		m_guiparts.col_clear_sunset = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_clear_sunset.SetRect( Rect( 0.05f, 0.35f, 0.04f, 0.04f ) );
		m_guiparts.col_clear_evening = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_clear_evening.SetRect( Rect( 0.05f, 0.40f, 0.04f, 0.04f ) );
		m_guiparts.col_clear_night = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_clear_night.SetRect( Rect( 0.05f, 0.45f, 0.04f, 0.04f ) );

		tempHandle = m_gui->CreateText( prntHandle );
		tempHandle.SetRect( Rect( 0.10f, 0.15f, 0,0 ) );
		tempHandle.SetText( "Light" );

		m_guiparts.col_light_sunrise = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_light_sunrise.SetRect( Rect( 0.10f, 0.20f, 0.04f, 0.04f ) );
		m_guiparts.col_light_morning = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_light_morning.SetRect( Rect( 0.10f, 0.25f, 0.04f, 0.04f ) );
		m_guiparts.col_light_afternoon = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_light_afternoon.SetRect( Rect( 0.10f, 0.30f, 0.04f, 0.04f ) );
		m_guiparts.col_light_sunset = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_light_sunset.SetRect( Rect( 0.10f, 0.35f, 0.04f, 0.04f ) );
		m_guiparts.col_light_evening = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_light_evening.SetRect( Rect( 0.10f, 0.40f, 0.04f, 0.04f ) );
		m_guiparts.col_light_night = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_light_night.SetRect( Rect( 0.10f, 0.45f, 0.04f, 0.04f ) );

		tempHandle = m_gui->CreateText( prntHandle );
		tempHandle.SetRect( Rect( 0.15f, 0.15f, 0,0 ) );
		tempHandle.SetText( "Sky" );

		m_guiparts.col_sky_sunrise = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_sky_sunrise.SetRect( Rect( 0.15f, 0.20f, 0.04f, 0.04f ) );
		m_guiparts.col_sky_morning = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_sky_morning.SetRect( Rect( 0.15f, 0.25f, 0.04f, 0.04f ) );
		m_guiparts.col_sky_afternoon = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_sky_afternoon.SetRect( Rect( 0.15f, 0.30f, 0.04f, 0.04f ) );
		m_guiparts.col_sky_sunset = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_sky_sunset.SetRect( Rect( 0.15f, 0.35f, 0.04f, 0.04f ) );
		m_guiparts.col_sky_evening = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_sky_evening.SetRect( Rect( 0.15f, 0.40f, 0.04f, 0.04f ) );
		m_guiparts.col_sky_night = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_sky_night.SetRect( Rect( 0.15f, 0.45f, 0.04f, 0.04f ) );

		tempHandle = m_gui->CreateText( prntHandle );
		tempHandle.SetRect( Rect( 0.20f, 0.15f, 0,0 ) );
		tempHandle.SetText( "Sun" );

		m_guiparts.col_sun_sunrise = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_sun_sunrise.SetRect( Rect( 0.20f, 0.20f, 0.04f, 0.04f ) );
		m_guiparts.col_sun_morning = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_sun_morning.SetRect( Rect( 0.20f, 0.25f, 0.04f, 0.04f ) );
		m_guiparts.col_sun_afternoon = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_sun_afternoon.SetRect( Rect( 0.20f, 0.30f, 0.04f, 0.04f ) );
		m_guiparts.col_sun_sunset = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_sun_sunset.SetRect( Rect( 0.20f, 0.35f, 0.04f, 0.04f ) );
		m_guiparts.col_sun_evening = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_sun_evening.SetRect( Rect( 0.20f, 0.40f, 0.04f, 0.04f ) );
		m_guiparts.col_sun_night = m_gui->CreateColorPicker( prntHandle );
		m_guiparts.col_sun_night.SetRect( Rect( 0.20f, 0.45f, 0.04f, 0.04f ) );

		m_guiparts.btn_save = m_gui->CreateButton( prntHandle );
		m_guiparts.btn_save.SetRect( Rect( 0.05f, 0.62f, 0.1f, 0.06f ) );
		m_guiparts.btn_save.SetText( "Save" );
	}

}

void CSkyColorEditor::Update ( void )
{
	// Set background
	if ( !m_daycycle ) { 
		m_daycycle = new Daycycle;
	}

	if ( m_gui->GetMouseInGUI() )
	{
		// GUI work
		//if ( m_guiparts.btn_load.GetButtonClicked() ) Load();
		if ( m_guiparts.btn_save.GetButtonClicked() )
		{
			CBinaryFile io;
			io.Open( ".res/terra/sun_terra0", io.IO_WRITE );
			io.WriteData( (char*)&m_daycycle->clearColors[0], sizeof(Color)*6 );
			io.WriteData( (char*)&m_daycycle->sunColors[0], sizeof(Color)*6 );
			io.WriteData( (char*)&m_daycycle->skyColors[0], sizeof(Color)*6 );
			io.WriteData( (char*)&m_daycycle->ambientColors[0], sizeof(Color)*6 );
			io.Close();
		}
		m_daycycle->SetTimeOfDay( m_gui->GetSliderValue( m_guiparts.slider_time ) );

		m_gui->UpdateColorPicker( m_guiparts.col_clear_sunrise, m_daycycle->clearColors[0] );
		m_gui->UpdateColorPicker( m_guiparts.col_clear_morning, m_daycycle->clearColors[1] );
		m_gui->UpdateColorPicker( m_guiparts.col_clear_afternoon, m_daycycle->clearColors[2] );
		m_gui->UpdateColorPicker( m_guiparts.col_clear_sunset, m_daycycle->clearColors[3] );
		m_gui->UpdateColorPicker( m_guiparts.col_clear_evening, m_daycycle->clearColors[4] );
		m_gui->UpdateColorPicker( m_guiparts.col_clear_night, m_daycycle->clearColors[4] );

		m_gui->UpdateColorPicker( m_guiparts.col_light_sunrise, m_daycycle->ambientColors[0] );
		m_gui->UpdateColorPicker( m_guiparts.col_light_morning, m_daycycle->ambientColors[1] );
		m_gui->UpdateColorPicker( m_guiparts.col_light_afternoon, m_daycycle->ambientColors[2] );
		m_gui->UpdateColorPicker( m_guiparts.col_light_sunset, m_daycycle->ambientColors[3] );
		m_gui->UpdateColorPicker( m_guiparts.col_light_evening, m_daycycle->ambientColors[4] );
		m_gui->UpdateColorPicker( m_guiparts.col_light_night, m_daycycle->ambientColors[5] );

		m_gui->UpdateColorPicker( m_guiparts.col_sky_sunrise, m_daycycle->skyColors[0] );
		m_gui->UpdateColorPicker( m_guiparts.col_sky_morning, m_daycycle->skyColors[1] );
		m_gui->UpdateColorPicker( m_guiparts.col_sky_afternoon, m_daycycle->skyColors[2] );
		m_gui->UpdateColorPicker( m_guiparts.col_sky_sunset, m_daycycle->skyColors[3] );
		m_gui->UpdateColorPicker( m_guiparts.col_sky_evening, m_daycycle->skyColors[4] );
		m_gui->UpdateColorPicker( m_guiparts.col_sky_night, m_daycycle->skyColors[5] );

		m_gui->UpdateColorPicker( m_guiparts.col_sun_sunrise, m_daycycle->sunColors[0] );
		m_gui->UpdateColorPicker( m_guiparts.col_sun_morning, m_daycycle->sunColors[1] );
		m_gui->UpdateColorPicker( m_guiparts.col_sun_afternoon, m_daycycle->sunColors[2] );
		m_gui->UpdateColorPicker( m_guiparts.col_sun_sunset, m_daycycle->sunColors[3] );
		m_gui->UpdateColorPicker( m_guiparts.col_sun_evening, m_daycycle->sunColors[4] );
		m_gui->UpdateColorPicker( m_guiparts.col_sun_night, m_daycycle->sunColors[5] );
	}
	else
	{
		Matrix4x4 rotMatx;
		rotMatx.setRotation( m_camera->transform.rotation );

		if ( (CInput::Mouse(CInput::MBLeft) && CInput::Mouse(CInput::MBRight))||(CInput::Mouse(CInput::MBMiddle))||(CInput::Mouse(CInput::MBRight)&&CInput::Key(Keys.Alt)) ) {
			m_camera->transform.position -= rotMatx * Vector3d( (ftype)CInput::DeltaMouseY(), (ftype)CInput::DeltaMouseX(), 0 ) * 0.2f;
		}
		else if ( CInput::Mouse( CInput::MBLeft ) ) {
			Vector3d targetAngles = m_camera->transform.rotation.getEulerAngles()-Vector3d( 0, (ftype)CInput::DeltaMouseY()*0.7f, -(ftype)CInput::DeltaMouseX()*0.7f );
			if ( targetAngles.y < -88.0f ) {
				targetAngles.y = -88.0f;
			}
			else if ( targetAngles.y > 88.0f ) {
				targetAngles.y = 88.0f;
			}
			m_camera->transform.rotation.Euler( targetAngles );
			
		}
		else if ( CInput::Mouse( CInput::MBRight ) ) {
			rotMatx.setRotation( m_camera->transform.rotation );
			m_camera->transform.position -= rotMatx * Vector3d( 0, (ftype)CInput::DeltaMouseX(), (ftype)CInput::DeltaMouseY() ) * 0.3f;
		}
	}

	// Draw grid
	for ( int i = -32; i <= 32; i += 2 ) {
		Debug::Drawer->DrawLine( Vector3d( 32,(ftype)i,0 ), Vector3d( -32,(ftype)i,0 ), Color(0.2f,0.2f,0.2f) );
		Debug::Drawer->DrawLine( Vector3d( (ftype)i,32,0 ), Vector3d( (ftype)i,-32,0 ), Color(0.2f,0.2f,0.2f) );
	}
}