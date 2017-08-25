
#ifndef _C_LIPSYNC_EDITOR_H_
#define _C_LIPSYNC_EDITOR_H_

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

class CCamera;
class CAudioSource;
class CLipsyncSequence;
namespace NPC {
	class CNpcBase;
}

class CLipsyncEditor : public CGameBehavior, public CRenderableObject
{

public:
	CLipsyncEditor ( void );
	~CLipsyncEditor( void );


	void Update ( void ) override;

	bool Render ( const char pass ) override;

private:
	/*void UpdateMorphs ( void );
	void CreatePhonetic ( const char*, Matrix4x4& );

	void Load ( const string& );
	void Save ( const string& );*/

private:
	glMaterial*		matui;
	glMaterial*		matfnt;

	CCamera*		m_camera;
	NPC::CNpcBase*	m_npc;
	CLipsyncSequence*	m_lipsync;

	enum eClickModes {
		CLICK_NONE,
		CLICK_MAINDRAG,
		CLICK_KEYDRAG
	};
	eClickModes		m_clickmode;
	Real			m_mousedownTime;	

	CAudioSource*	m_syncsound;
	Real			m_startposition;
	Real			m_position;
	Real			m_sound_length;

	int				m_editmode;

	bool			m_loaded;
	string			m_target_sound;

	/*struct keyframe_t {
		Real	time;
		Real	strength;
		char	phonetic [16];

		bool operator < (const keyframe_t& right) {
			return time < right.time;
		}
	};
	vector<keyframe_t>	m_keyframes;*/
	int			m_selectedkeyframe;

	Real			m_waveform [1000];
};

#endif//_C_LIPSYNC_EDITOR_H_