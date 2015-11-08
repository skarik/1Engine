
#include "after-editor/standalone/CLipsyncEditor.h"
#include "after/states/model/CLipsyncSequence.h"

#include "renderer/camera/CCamera.h"
#include "after/entities/character/npc/CNpcBase.h"
#include "after/entities/character/CMccCharacterModel.h"

#include "core/input/CInput.h"

#include "engine/audio/CAudioInterface.h"
#include "audio/CAudioSource.h"

#include "renderer/texture/CBitmapFont.h"
#include "core/system/Screen.h"

#include "core/math/Math.h"
#include "core/system/io/FileUtils.h"
#include "core/system/System.h"

#include "renderer/material/glMaterial.h"

#include "engine/state/CGameState.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CLipsyncEditor::CLipsyncEditor ( void )
	: CGameBehavior(), CRenderableObject()
{
	this->renderType = Renderer::V2D;

	m_camera = new CCamera;
	m_camera->fov = 45;
	m_npc = NULL;

	matui = new glMaterial();
	matui->setTexture( 0, new CTexture("null") );
	matui->passinfo.push_back( glPass() );
	matui->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	matui->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matui->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	SetMaterial( matui );

	CBitmapFont* fntMenu	= new CBitmapFont ( "monofonto.ttf", 16, FW_BOLD );
	matfnt = new glMaterial;
	matfnt->m_diffuse = Color( 1.0f,1,1 );
	matfnt->setTexture( 0, fntMenu );
	matfnt->passinfo.push_back( glPass() );
	matfnt->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matfnt->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	matfnt->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	m_clickmode = CLICK_NONE;
	
	m_loaded = false;
	m_target_sound = ".res/sounds/voice/MF_WellIsntThatInteresting.mp3";
	m_syncsound = Audio.PlayWaveFile( m_target_sound );
	m_syncsound->Stop();
	m_target_sound = "-nothing loaded-";

	m_startposition = 0;
	m_position = 0;

	m_selectedkeyframe = -1;

	m_editmode = 0;

	memset( m_waveform, 0, sizeof(Real)*1000 );
}

CLipsyncEditor::~CLipsyncEditor ( void )
{
	delete m_syncsound;
}

void CLipsyncEditor::Update ( void )
{
	if ( m_npc == NULL ) {
		m_npc = (NPC::CNpcBase*)(CGameState::Active()->FindFirstObjectWithTypename( "CNpcBase" ));
		m_lipsync = ((CMccCharacterModel*)(m_npc->GetCharModel()))->GetLipSyncer();
		if ( m_npc == NULL ) {
			return;
		}
	}

	// Set the camera position
	{
		XTransform rotTransform;
		m_npc->GetCharModel()->GetEyecamTransform( rotTransform );
		Ray npcEyeRay = m_npc->GetEyeRay();
		Vector3d targetPosition = npcEyeRay.pos + (Rotator(rotTransform.rotation*Quaternion( Vector3d( 90,0,90 ) )) * Vector3d(3,0,0)); // + npcEyeRay.dir * 3.2f;
		///m_camera->transform.rotation.RotationTo( Vector3d::forward, -npcEyeRay.dir );
		Rotator targetRotation = Rotator(rotTransform.rotation*Quaternion( Vector3d( 90,0,90 ) )) * Rotator( 0,0,180 );// * Rotator( 0,0,180 );

		m_camera->transform.position = m_camera->transform.position.lerp( targetPosition, 0.1f );
		m_camera->transform.rotation = m_camera->transform.rotation.LerpTo( targetRotation,0.1f );
	}

	// Force NPC to look ahead
	{
		m_npc->GetAI()->RequestLookatOverride( m_npc->transform.position + Vector3d( 10,0,5 ) );
	}

	// Look for load input
	{
		if ( Input::Key( Keys.Control ) && Input::Keydown( 'O' ) )
		{
			char str_result [1024];
			System::sFileDialogueEntry entries [3];
			strcpy( entries[0].extension, "*.MP3;*.OGG;*.WAV;*.MPH" );
			strcpy( entries[0].filetype, "All valid files" );
			strcpy( entries[1].extension, "*.MP3;*.OGG;*.WAV" );
			strcpy( entries[1].filetype, "Sound Files" );
			strcpy( entries[2].extension, "*.MPH" );
			strcpy( entries[2].filetype, "Morph Files" );
			if ( System::GetOpenFilename( str_result, entries, 3, ".res/sounds/", "Load Sound or Morph File" ) )
			{
				//LoadSystem( str_result );
				// clear the list and recreate it
				//myGUI->ClearListview( edtComponentList.list );
				string result = str_result;
				result = result.substr( 0, result.find_last_of('.') );
				result += ".mph";
				m_lipsync->Load( result );
				
				delete m_syncsound;
				m_target_sound = str_result;
				m_syncsound = Audio.PlayWaveFile( m_target_sound );
				m_syncsound->Stop();

				m_loaded = true;
			}
		}
	}

	// Don't do anything else if not loaded
	if ( !m_loaded ) {
		return;
	}

	// Do mouse click
	Vector2d mousePosition ( Input::MouseX() / Screen::Info.width, Input::MouseY() / Screen::Info.height );
	{
		//0.05f,0.70f, 0.9f,0.25f is the rect
		if ( Input::MouseDown(Input::MBRight) ) {
			if ( m_editmode == 0 ) {
				m_editmode = 1;
			}
			else {
				m_editmode = 0;
			}
		}

		if ( Input::MouseDown(Input::MBLeft) ) {
			if ( Input::Key( Keys.Shift ) ) {
				m_selectedkeyframe = -1;
				if ( Rect(0.05f,0.70f, 0.9f,0.25f).Contains( mousePosition )  ) {
					CLipsyncSequence::keyframe_t newkey;
					newkey.time = Math.Clamp( (mousePosition.x-0.05f)/0.90f, 0,1 ) * m_sound_length;
					newkey.phonetic[0] = 0;
					newkey.strength = 1;
					m_lipsync->m_keyframes.push_back( newkey ); // Add new frame at the position
					std::sort( m_lipsync->m_keyframes.begin(), m_lipsync->m_keyframes.end() ); // Sort the keyframes again
					m_selectedkeyframe = -1;
				}
			}
			else {
				// Check for the nearest keyframe
				m_selectedkeyframe = -1;
				Real mousePosTime = Math.Clamp( (mousePosition.x-0.05f)/0.90f, 0,1 ) * m_sound_length;
				Real distance, closestDistance;
				closestDistance = m_sound_length;
				for ( uint i = 0; i < m_lipsync->m_keyframes.size(); ++i )
				{
					distance = fabs(m_lipsync->m_keyframes[i].time - mousePosTime);
					if ( m_selectedkeyframe == -1 || distance < closestDistance ) {
						m_selectedkeyframe = i;
						closestDistance = distance;
					}
				}
				// If it's too far, then drag
				if ( closestDistance > 0.01f * m_sound_length ) {
					m_selectedkeyframe = -1;
				}
				// Set start time anyways
				m_startposition = mousePosTime;
			}
		}

		// Mouse up
		if ( Input::MouseUp(Input::MBLeft) )
		{
			if ( m_clickmode == CLICK_KEYDRAG && m_mousedownTime > 0.25f )
			{
				m_selectedkeyframe = -1;
				std::sort( m_lipsync->m_keyframes.begin(), m_lipsync->m_keyframes.end() );
			}
		}

		// Mouse held down
		if ( Input::Mouse(Input::MBLeft) )
		{
			m_mousedownTime += Time::deltaTime;
			if ( m_clickmode == CLICK_KEYDRAG || m_selectedkeyframe != -1 )
			{
				if ( m_clickmode == CLICK_NONE ) {
					m_clickmode = CLICK_KEYDRAG;
				}
				if ( m_clickmode == CLICK_KEYDRAG )
				{
					if ( m_mousedownTime > 0.25f ) {
						m_lipsync->m_keyframes[m_selectedkeyframe].time = Math.Clamp( (mousePosition.x-0.05f)/0.90f, 0,1 ) * m_sound_length;
					}
				}
			}
			if ( m_clickmode == CLICK_MAINDRAG || Rect(0.05f,0.70f, 0.9f,0.25f).Contains( mousePosition ) )
			{
				if ( m_clickmode == CLICK_NONE ) {
					m_clickmode = CLICK_MAINDRAG;
				}
				if ( m_clickmode == CLICK_MAINDRAG )
				{
					m_startposition = Math.Clamp( (mousePosition.x-0.05f)/0.90f, 0,1 ) * m_sound_length;
				}
			}
		}
		else
		{
			m_mousedownTime = 0;
			m_clickmode = CLICK_NONE;
		}
		
	}
	// Play or pause when press space
	if ( Input::Keydown( Keys.Space ) )
	{
		if ( m_syncsound->IsPlaying() ) {
			m_syncsound->Stop();
		}
		else {
			m_syncsound->SetPlaybackTime( m_startposition );
			m_syncsound->Play();
			if ( !m_syncsound->IsPlaying() ) { // error or sound freed itself
				delete m_syncsound;
				m_syncsound = Audio.PlayWaveFile( m_target_sound );
				m_syncsound->SetPlaybackTime( m_startposition );
				m_syncsound->Play();
			}
		}
	}
	// If have a key selected, be able to type or delete it
	if ( m_selectedkeyframe != -1 )
	{
		//if ( m_keyframes[m_selectedkeyframe].phonetic
		char addChar = Input::GetTypeChar();
		if ( isalpha(addChar) ) {
			int length = strlen(m_lipsync->m_keyframes[m_selectedkeyframe].phonetic);
			if ( length < 14 ) {
				m_lipsync->m_keyframes[m_selectedkeyframe].phonetic[length] = tolower(addChar);
				m_lipsync->m_keyframes[m_selectedkeyframe].phonetic[length+1] = 0;
			}
		}
		if ( Input::Keydown( Keys.Backspace ) ) {
			int length = strlen(m_lipsync->m_keyframes[m_selectedkeyframe].phonetic);
			if ( length > 0 ) {
				m_lipsync->m_keyframes[m_selectedkeyframe].phonetic[length-1] = 0;
			}
		}

		// Mouse wheel for the strength
		if ( Input::DeltaMouseW() > 0 ) {
			m_lipsync->m_keyframes[m_selectedkeyframe].strength += 0.05f;
		}
		if ( Input::DeltaMouseW() < 0 ) {
			m_lipsync->m_keyframes[m_selectedkeyframe].strength -= 0.05f;
		}
		m_lipsync->m_keyframes[m_selectedkeyframe].strength = Math.Clamp( m_lipsync->m_keyframes[m_selectedkeyframe].strength, 0, 1 );

		if ( Input::Key( Keys.Delete ) ) {
			m_lipsync->m_keyframes.erase( m_lipsync->m_keyframes.begin() + m_selectedkeyframe );
			m_selectedkeyframe = -1;
		}
	}

	// Look for save input
	{
		if ( Input::Key( Keys.Control ) && Input::Keydown( 'S' ) )
		{
			//m_loaded = true;
			string result = m_target_sound;
			result = result.substr( 0, result.find_last_of('.') );
			result += ".mph";

			m_lipsync->Save( result );
		}
	}

	// Play the sound and get playback time
	{
		m_sound_length = (Real) m_syncsound->GetSoundLength();
		if ( m_syncsound->IsPlaying() ) {
			m_position = (Real) m_syncsound->GetPlaybackTime();
			// Get the goddamn waveform
			m_waveform[int(999*m_position/m_sound_length)] = (Real) m_syncsound->GetCurrentMagnitude();
		}
		else {
			m_position = m_startposition;
		}
	}

	// Now, update the morphs
	m_lipsync->m_sequence_length = m_sound_length;
	m_lipsync->m_position = m_position;
	m_lipsync->UpdateMorphs();
}



bool CLipsyncEditor::Render ( const char pass )
{
	if ( pass != 0 || m_npc == NULL ) {
		return true;
	}

	GL_ACCESS GLd_ACCESS

	GL.beginOrtho();

	// Render the BG
	matui->m_diffuse = Color( 0.2,0.2,0.2,0.5 );
	matui->bindPass(0);
	GLd.DrawRectangleA( 0.05f,0.70f, 0.9f,0.25f );

	// Render the waveform
	matui->m_diffuse = Color( 0,0,0,0.3 );
	matui->bindPass(0);
	for ( uint i = 0; i < 1000; ++i )
	{
		GLd.DrawLineA( 0.05f+0.9f*(i/1000.0f), 0.825f-m_waveform[i]*0.1f, 0.05f+0.9f*(i/1000.0f),0.825f+m_waveform[i]*0.1f );
	}

	// Render the sound position
	matui->m_diffuse = Color( 0,0,0,1 );
	matui->bindPass(0);
	GLd.DrawLineA( 0.05f+0.9f*(m_startposition/m_sound_length), 0.70f, 0.05f+0.9f*(m_startposition/m_sound_length),0.95f );
	GLd.DrawLineA( 0.05f+0.9f*(m_position/m_sound_length), 0.70f, 0.05f+0.9f*(m_position/m_sound_length),0.95f );

	// Render the keyframes
	std::vector<CLipsyncSequence::keyframe_t>& m_keyframes = m_lipsync->m_keyframes;
	bool offset = false;
	for ( uint i = 0; i < m_keyframes.size(); ++i )
	{
		if ( i != 0 ) {
			if ( (m_keyframes[i].time-m_keyframes[i-1].time)/m_sound_length < 0.03 ) {
				offset = !offset;
			}
			else {
				offset = false;
			}
		}
		if ( i == m_selectedkeyframe ) {
			matui->m_diffuse = Color( 1,1,1,0.3 );
			matui->bindPass(0);
			GLd.DrawRectangleA( 0.05f+0.9f*(m_keyframes[i].time/m_sound_length) - 0.02f*0.5f, 0.70f, 0.02f,0.25f );
		}
		matui->m_diffuse = Color( 0.4,0.4,0.4,1 );
		matui->bindPass(0);
		GLd.DrawLineA( 0.05f+0.9f*(m_keyframes[i].time/m_sound_length), 0.70f, 0.05f+0.9f*(m_keyframes[i].time/m_sound_length),0.95f );
		matui->m_diffuse = Color( 0.7,0.7,0.7,1 );
		matui->bindPass(0);
		GLd.DrawLineA( 0.05f+0.9f*(m_keyframes[i].time/m_sound_length), 0.70f+(1-m_keyframes[i].strength)*0.25f, 0.05f+0.9f*(m_keyframes[i].time/m_sound_length),0.95f );

		Real textWidth = GLd.GetAutoTextWidth( "%s", m_keyframes[i].phonetic ) / Screen::Info.width;
		matui->m_diffuse = Color( 0.1,0.1,0.1,0.4 );
		matui->bindPass(0);
		GLd.DrawRectangleA( 0.05f+0.9f*(m_keyframes[i].time/m_sound_length) - (textWidth+0.01f)*0.5f, 0.75f-0.027f + (offset?0.031f:0.0f), (textWidth+0.01f),0.03f );

		matfnt->m_diffuse = Color( 1.0,1.0,1.0,1.0 );
		matfnt->bindPass(0);
		GLd.DrawAutoTextCentered( 0.05f+0.9f*(m_keyframes[i].time/m_sound_length),0.75f + (offset?0.031f:0.0f), "%s", m_keyframes[i].phonetic );
		if ( i == m_selectedkeyframe ) {
			GLd.DrawAutoTextCentered( 0.05f+0.9f*(m_keyframes[i].time/m_sound_length),0.83f, "%.2lf", m_keyframes[i].strength );
		}
	}

	// Render the sound length
	matfnt->m_diffuse = Color( 0.5,0.5,0.5,1.0 );
	matfnt->bindPass(0);
	GLd.DrawAutoTextCentered( 0.05f,0.95f, "%.2lf", 0.0 );
	GLd.DrawAutoTextCentered( 0.95f,0.95f, "%.2lf", m_sound_length );

	// Draw the file name while we're here
	GLd.DrawAutoText( 0.05f,0.70f, m_target_sound.c_str() );

	// Render the controls
	matfnt->m_diffuse = Color( 0.9,0.9,0.9, 0.5 );
	matfnt->bindPass(0);
	GLd.DrawAutoText( 0.02f,0.47f, "LMB" );
	GLd.DrawAutoText( 0.08f,0.47f, "Select/Drag" );

	GLd.DrawAutoText( 0.02f,0.5f, "Shift+LMB" );
	GLd.DrawAutoText( 0.08f,0.5f, "New Key" );

	GLd.DrawAutoText( 0.02f,0.53f, "M Wheel" );
	GLd.DrawAutoText( 0.08f,0.53f, "Strength" );

	GLd.DrawAutoText( 0.02f,0.56f, "Space" );
	GLd.DrawAutoText( 0.08f,0.56f, "Play/pause" );

	GLd.DrawAutoText( 0.02f,0.59f, "Ctrl+O" );
	GLd.DrawAutoText( 0.08f,0.59f, "Open" );

	GLd.DrawAutoText( 0.02f,0.62f, "Ctrl+S" );
	GLd.DrawAutoText( 0.08f,0.62f, "Save" );

	GL.endOrtho();

	return true;
}
