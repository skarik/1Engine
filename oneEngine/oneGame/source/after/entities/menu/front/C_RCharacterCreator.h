
//
// -- C_RCharacterCreator --
// Class for the character creation/edit menu
// This  lass requires that a character be set for editing by SetCharacter first, however.
// For visual editing, a CCharacterModel must be passed in.
// This class does not control the camera nor position of the model, only what is shown on the model.
//

#ifndef _C_R_CHARACTER_CREATOR_H_
#define _C_R_CHARACTER_CREATOR_H_

#include "core/types/types.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine-common/dusk/CDuskGUI.h"

#include "after/entities/character/CMccCharacterModel.h"
#include "after/states/player/CPlayerStats.h"

class CCamera;
namespace NPC {
	class CNpcBase;
}

class C_RCharacterCreator : public CGameBehavior
{
	ClassName( "C_RCharacterCreator" );
public:
				C_RCharacterCreator ( void );
				~C_RCharacterCreator ( void );

				// Set the character to edit/create
	void		SetSaveFile ( string const& );
				// Set visual model for editing. This is optional.
	void		SetVisualModel ( CMccCharacterModel* );
	void		SetVisualModel ( NPC::CNpcBase* );

				// Returns if the creation has finished and this dialogue should be deleted
	bool		IsDoneEditing ( void );
				// Return if can cancel this at the moment
	bool		GetCanCancel ( void );

				// Returns if the creation should move to overview camera mode
	bool		DoCameraOverview ( void );
				// Modifies camera transform to go into overview mode
	void		GetCameraOverview ( CCamera* );

				// Update (performs GUI update)
	void		Update ( void );


	CPlayerStats*	GetPlayerStats ( void ) {
		return pl_stats;
	}
private:
	CPlayerStats*		pl_stats;
	//CMccCharacterModel*	pl_model;
	NPC::CNpcBase*	m_playercharacter;

	Rotator				rModelRotation;
	Vector3d			vCameraRotation;

	bool				b_done_editing;

	// Magic effects
	CParticleSystem*	particlesLeftHand;
	CParticleSystem*	particlesRightHand;


	CGameObject*		stageFloor;

public:
	// Now, GUI elements
	typedef CDuskGUI::Handle Handle;
	// Pointer to GUI object
	CDuskGUI*	gui;

private:
	// Menu states
	enum eMenuState
	{
		M_main,
		M_colors,
		M_stats,
		M_style,
		M_friend,
		M_tattoos,
		M_visual
	} iMenuState;

	void stateMain ( void );
	void stateColors ( void );
	void stateStats ( void );
	void stateStyle ( void );
	void stateCompanion ( void );
	void stateTattoos ( void );
	void stateVisual ( void );

	struct sMainElements_t
	{
		Handle	container;
		Handle	btnback;
		Handle	btncontinue;

		Handle	lbl_soulname;
		Handle	fld_soulname;
		string	str_soulname;

		Handle	lbl_name;
		Handle	fld_name;
		string	str_name;

		Handle	lbl_race;
		Handle	ddl_race_choice;

		Handle	lbl_gender;
		Handle	ddl_gender_choice;

		Handle	lbl_gametype;
		Handle	ddl_gametype_choice;

		Handle	lbl_cloudmode;
		Handle	ddl_cloudmode_choice;

		Handle	btn_colors;
		Handle	btn_stats;
		Handle	btn_style;
		Handle	btn_companion;
		Handle	btn_markings;
		Handle	btn_appearance;
		Handle	btn_randomize;
		Handle	btn_reroll;
	} tMainAttr;
	struct sColorElements_t
	{
		Handle	container;
		Handle	btndone;

		Handle	lbl_color;

		Handle	lbl_eyecolor;
		Handle	cs_eyecolor;
		Handle	lbl_focuscolor;
		Handle	cs_focuscolor;

		Handle	lbl_haircolor;
		Handle	cs_haircolor;
		Handle	lbl_skincolor;
		Handle	cs_skincolor;
	} tCharColors;
	struct sStatElements_t
	{
		Handle	container;
		Handle	btndone;

		Handle	lbl_race;
		Handle	ddl_race_choice;

		Handle	lbl_stats;

		Handle	lbl_txt_str;
		Handle	lbl_txt_agi;
		Handle	lbl_txt_int;

		Handle	lbl_txt_hp;
		Handle	lbl_txt_sp;
		Handle	lbl_txt_mp;

		Handle	pgh_race_desc;
		Handle	pgh_race_buff;
	} tCharStats;
	struct sStyleElements_t
	{
		Handle	container;
		Handle	btndone;

		Handle	lbl_hair;
		Handle	ddl_hair_choice;

		Handle	lbl_movement;
		Handle	ddl_movement_choice;

		Handle	lbl_speech;
		Handle	ddl_speech_choice;
	} tCharStyle;
	struct sCompanionElements_t
	{
		Handle	container;
		Handle	btndone;

		Handle	lbl_gender;
		Handle	ddl_gender_choice;

		Handle	lbl_race;
		Handle	ddl_race_choice;

		Handle	lbl_love;
		Handle	ddl_love_choice;

		Handle	pgh_comp_desc;
	} tCompAttr;

	struct sTattooElements_t
	{
		Handle	container;
		Handle	btndone;

		Handle	btn_delete_tattoo;
		Handle	btn_add_tattoo;
		Handle	lbl_current_tattoo;
		Handle	btn_next_tattoo;
		Handle	btn_prev_tattoo;
		bool	need_list_refresh;

		Handle	lbl_tattoo_type;
		Handle	ddl_tattoo_type;
		Handle	lbl_tattoo_index;
		Handle	ddl_tattoo_index;
		int		cur_tattoo_type;

		Handle	lbl_position;
		Handle	fld_position;
		Handle	lbl_tattoo_facing;
		Handle	ddl_tattoo_facing; //x axis, y axis, z axis, face character
		Handle	lbl_mirrored;
		Handle	chk_mirrored;

		Handle	lbl_scale;
		Handle	sdr_scale;
		Handle	lbl_flip;
		Handle	chk_flip;
		Handle	lbl_curvature;
		Handle	sdr_curvature;
		bool	cur_flipped;

		Handle	lbl_blend;
		Handle	clr_blend;

		bool	need_redraw_tattoos;
		int		current_loaded_tattoo;

		Vector3d	vTattooPosition;
		ftype		fTattooScaling;
		std::vector<string>	vTattooNames;

		bool	reinitialize;
	} tCharMarks;
	void TattooGUIRefreshList ( void );
	void TattooGUIGenerateDropdown ( int );
	void TattooGUILoadTattoo ( int );
	void TattooGUILimitTattoo ( CRacialStats::tattoo_t& );
	void TattooGUIRandomizeTattoos ( void );

	struct sVisualsElements_t
	{
		Handle	container;
		Handle	btndone;
	} tCharLooks;

	// Function to handle all these elements
	void CreateGUIElements ( void );

	// Function to check player info to make sure valid
	void CheckColors ( void );
};


#endif//_C_R_CHARACTER_CREATOR_H_