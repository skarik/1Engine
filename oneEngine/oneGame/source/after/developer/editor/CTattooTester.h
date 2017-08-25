
#ifndef _C_TATTOO_TESTER_H_
#define _C_TATTOO_TESTER_H_

#include "engine/behavior/CGameBehavior.h"
#include "engine-common/dusk/CDuskGUI.h"
#include "after/states/player/CPlayerStats.h"

class CTexture;
class CRenderTexture;

class CSkinnedModel;
class CMccCharacterModel;


class CTattooTester : public CGameBehavior
{

public:
				CTattooTester ( void );
				~CTattooTester ( void );

	void		Update ( void );

private:
	// ==Models==
	/*CSkinnedModel*	mdlSrc;
	CSkinnedModel*	mdlBody;
	CSkinnedModel*	mdlHead;*/
	CMccCharacterModel* model;

	void LoadModels ( void );
	void UpdateModels ( void );

	CTexture*		texTattoo;
	CRenderTexture*	texTattooTarget;
	CPlayerStats*	pl_stats;

	void UpdateTarget ( void );

	void DrawToTarget ( void );
	struct tattoo_t;
	void DrawTattooToTarget ( tattoo_t& );

	void RaycastModel ( void );

	static const int TATT_CLANMARK = 0;
	static const int TATT_TATTOO = 1;
	static const int TATT_SCAR = 2;
	static const int TATT_FLUXXGLOW = 3;
	struct tattoo_t {
		uchar			type;		// type (if it glows, if it's a scar, yadayada)
		arstring<32>	pattern;	// name of pattern
		Color			color;		// color of marking
		bool			mirror;		// should it be mirrored? (symmetrical designs)
		Vector3d		projection_pos;		// projection start position
		Vector3d		projection_dir;		// projection direction
		Vector3d		projection_scale;	// projection scaling
		ftype			projection_angle;	// the angle to that the projection area should curve to
		// Tattoos are rendered once to a side-buffer. The side-buffer is rendered on 
		// the character model with a shader. Rendering a tattoo to the side-buffer
		// involves a model-space to UV-space projection. A semi-high density quad
		// plane is used to render the tattoo to the side-buffer. The curvature of the
		// projection can be configured to give better results on curved areas, such
		// as arms or legs. A limitation is the project needs one of the first rays to
		// hit, or the projection will be ignored. The algorithm needs a reference
		// point to work with.
		//bool			valid;
		//Rotator			projection_rot;
	};
	tattoo_t tattoo0;

	//vector<tattoo_t> tattoos;

	Vector2d	uvCursor;

	// ==GUI==
	void CreateGUI ( void );
	void DoGUIWork ( void );

	typedef Dusk::Handle GUIHandle;

	CDuskGUI*	gui;

	struct editorComponentList_t
	{
		GUIHandle btnLoad;
	} edtComponentList;

	struct sTattooElements_t
	{
		typedef GUIHandle Handle;
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
};

#endif//_C_TATTOO_TESTER_H_