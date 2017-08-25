
//
// -- C_RMainMenu --
// Class for the main menu
// Manages camera angles, time of day in the background
// CMainMenu is outclassed by this class. Not that CMainMenu ever worked in the first place.
//

#ifndef _C_R_MAIN_MENU_H_
#define _C_R_MAIN_MENU_H_

#include "core/types/types.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine-common/dusk/CDuskGUI.h"

#include "C_RCharacterCreator.h"
#include "C_RMainMenuTitle.h"

#include "after/interfaces/world/CWorldCartographer.h"

class CCamera;
class Daycycle;
class Mooncycle;
class CloudSphere;

class CPlayerStats;

class CRenderTarget;
class CSprite;

class COctreePauseloader;
class CGameType;
namespace NPC {
	class CNpcBase;
}

class C_RMainMenu : public CGameBehavior
{
	ClassName( "C_RMainMenu" );
public:

	C_RMainMenu ( CGameType* );
	~C_RMainMenu ( void );

	void Update ( void );

private:
	// Menu Object for bg (todo, move these to environment effects. Maybe. Or just pass them to the environment.)
	CCamera*	pCamera;
	Daycycle*	pDaycycle;
	Mooncycle*	pMooncycle;
	CloudSphere* pCloudcycle;

	// Menu object for external stops
	COctreePauseloader*	pPauseLoader;

	// Menu states
	enum eMenuState
	{
		M_load,
		M_intro,
		M_mainmenu,
		M_options,
		M_realmselect,
		M_blendtocharselect,
		M_charselect,
		M_blendtogameplay,
		M_blendtocharcreation,
		M_charcreation,
		M_blendtogameplaystart
	} iMenuState;

	// Menu lerp values
	ftype	fTimer;
	Real	fLerpValue;
	FORCE_INLINE void BlendWithSpeed ( const Real speed );

	bool	bSubstateBlend;
	/*ftype	fTimeOfDay;
	ftype	fSpaceEffect;

	Vector3d	vCamPosPrev;
	Quaternion	vCamRotPrev;
	Vector3d	vCamPosNext;
	Quaternion	vCamRotNext;
	ftype		fCamFovPrev;
	ftype		fCamFovNext;
	ftype	fCameraLerp;

	ftype	fTimeOfDayPrev;
	ftype	fTimeOfDayNext;

	ftype	fSpaceEffectPrev;
	ftype	fSpaceEffectNext;*/
	struct MenuEnvState {
		Real	timeOfDay;
		Real	spaceEffect;

		Vector3d	cameraPos;
		Quaternion	cameraRot;
		Real		cameraRoll;
		Real		cameraFoV;
	};
	MenuEnvState stateCurrent;
	MenuEnvState stateTarget;
	MenuEnvState statePrevious;

private:
	eMenuState	stateLoad ( void );
	eMenuState	stateIntro ( void );
	eMenuState	stateMainMenu ( void );
	eMenuState	stateOptions ( void );
	eMenuState	stateRealmSelect ( void );
	eMenuState	stateCharSelect ( void );
	eMenuState	stateCharCreation ( void );

	eMenuState	stateBlendToCharSelect ( void );
	eMenuState	stateBlendToGameplay ( void );
	eMenuState	stateBlendToCharCreation ( void );
	eMenuState	stateBlendToGameplayNewChar ( void );

private:
	// Now, GUI elements
	typedef CDuskGUI::Handle Handle;
	// Pointer to GUI object
	CDuskGUI*	gui;

	struct sMainMenuElements_t
	{
		C_RMainMenuTitle*	title;
		Handle	container;
		Handle	btnrealm;
		Handle	btnoptions;
		Handle	btnquit;
	} tMainMenu;
	struct sOptionsMenuElements_t
	{
		Handle	container;
		Handle	btnback;

		// dropdown menu for the fullscreen resolution
		Handle	lbl_fullscreen_choice;
		Handle	ddl_fullscreen_choice;
		// label saying fullscreen enabled
		Handle	lbl_fullscreen_info;

		// toggle for shadows
		Handle	chk_shadow_enabled;
		// shadow resolution dropdown
		Handle	ddl_shadow_resolution;
	} tOptions;
	struct sRealmSelectElements_t
	{
		static const int S_SELECT_REALM = 0;
		static const int S_CREATE_REALM = 1;
		int state;

		Handle	container;

		Handle	cont_selection;
		Handle	lblheader;
		Handle	btnback;
		Handle	btnnewrealm;
		Handle	btnselectrealm;
		Handle	btndeleterealm;
		Handle	btn_refresh;
		Handle	slsListview;
		Handle	lblinfo;

		Handle	dlgdeleteconfirm;

		
		Handle  cont_creation;
		Handle	lbl_creation_header;
		Handle	lbl_realmname;
		Handle	fld_realmname;
		string	cur_realmname;
		Handle	lbl_seed;
		Handle	fld_seed;
		string	cur_seed;
		Handle	btn_confirmnew;


		CRenderTexture*	terra_rt;
		CSprite*	terra_sprite;
		CWorldCartographer*	cartographer;
		bool	renderMap;
		CWorldCartographer::sRenderProgressState	mapRenderState;
		Terrain::CWorldGenerator*	generator;
		CModel*		main_planetoid;
		
		std::vector<string> list_realms;
		int		selection;

		void	CreateRealmList ( void );

		CDuskGUI*	gui;	// Needs the GUI
	} tRealmSelect;
	struct sCharacterSelectElements_t
	{
		Handle	container;
		Handle	btnback;

		Handle	lblheader;

		Handle	btnselectchar;
		Handle	btnnewchar;
		Handle	btndeletechar;

		Handle	pghinfo;

		Handle	dlgdeleteconfirm;

		//Handle*	list_chars;
		//string*	list_strs;
		Handle	slsCharacterList;
		std::vector<string>	list_chars;

		uint	char_num;
		int		selection;
		void	CreateCharList ( void );
		void	FreeCharList ( void );

		arstring<256>	last_selected;

		CDuskGUI*	gui;	// Needs the GUI
	} tCharSelect;
	struct sNewCharacterElements_t
	{
		int		state;

		Handle	container;
		Handle	btnback;

		//Handle	dlg_soulname;
		Handle	pgh_soulname;
		Handle	fld_soulname;
		Handle	btn_soulnamecontinue;

		string	cur_soulname;
		/*Handle	btncontinue;

		Handle	fld_character_name;

		Handle	lbl_race;
		Handle	lbl_race_choice;
		Handle	btn_race_left;
		Handle	btn_race_right;
		eCharacterRace	race;

		Handle	lbl_gender;
		Handle	lbl_gender_choice;
		Handle	btn_gender_left;
		Handle	btn_gender_right;

		Handle	lbl_gametype;
		Handle	lbl_gametype_choice;
		Handle	btn_gametype_left;
		Handle	btn_gametype_right;*/
	} tCharCreation;

	CPlayerStats*	pl_stats;

	C_RCharacterCreator*	p_char_creator;
	CMccCharacterModel*		p_model;

	struct sCharacterModelState_t
	{
		ftype		glanceShiftTime;
		Vector3d	glanceAngle;
		Vector3d	glanceFinalAngle;
		
		Vector3d	lookatCameraOffset;
		Vector3d	lookatCameraFinalOffset;

	} tCharModel;

	CGameType*		m_gametype;
	NPC::CNpcBase*	m_playercharacter;

	arstring<128> m_environment;
	arstring<128> m_behavior;

	Vector3d_d	m_next_spawnpoint;

	// Function to handle all these elements (this class is so massive T_T)
	void CreateGUIElements ( void );

	// Sets the terrain placement position based on character stats
	void SetTerrainPlacementFromStats ( void );

};

#endif