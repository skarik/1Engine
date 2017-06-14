
#ifndef _C_PLAYER_LOGBOOK_H_
#define _C_PLAYER_LOGBOOK_H_

// == Includes ==
#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

#include "renderer/texture/CBitmapFont.h"

//#include "CRecipeLibrary.h"
class CRecipeLibrary;
struct tItemRecipe;

#include <string>
using std::string;

//#include <boost/filesystem.hpp>

class CWeaponItem;

class CInputControl;

// == Class Definition ==
class CPlayerLogbook : public CGameBehavior, public CRenderableObject
{
	ClassName( "CPlayerLogbook" );
public:

	explicit	CPlayerLogbook ( CRecipeLibrary* p_lib );
				~CPlayerLogbook ( void );

	void Update ( void );

	bool Render ( const char pass );

	// Set visiblity
	void SetVisibility ( bool& visibility, bool forceDirty=false );
	void SetTab ( const int tabNum );
private:
	bool	bVisibilityDirty;
	bool	bDragBook;
	bool	bIsBusyHolding;
	int i_mouseOver_tabSelect;
	int iLogbookMode;	// 0 is data, 1 is recipe, 2 is player log

	CInputControl*	mControl;

	// 0 is default
	// everything else varies state to state
	int i_logbookSubstate;
	
	// =====
private:	// Recipe Logbok
	void InitRecipeLogbook ( void );

	void UpdateRecipebook ( Vector2d vMousePos );
	void RenderRecipebook ( void );

	struct sRecipeSearch
	{
		bool bSearchDirty;
		string sCurrent;
	} currentRecipeSearch;

	struct sLogbookRecipe
	{
		tItemRecipe* recipe;
	};

	CRecipeLibrary*	p_recipe_Library;

	std::vector<sLogbookRecipe>	s_recipe_CurrentList;
	sLogbookRecipe	s_recipe_CurrentEntry;
	std::vector<CWeaponItem*>	p_recipe_CurrentEntryItems;
	string			s_recipe_CurrentSubject;
	bool			b_recipe_InSubjectView;

	int				i_recipe_MouseOverX;
	int				i_recipe_MouseOverY;

	int				i_recipe_ScrollSize;
	int				i_recipe_ScrollOffset;
	float			f_recipe_ScrollBarPos;

	Vector2d		v_recipe_DrawPos;

	string			s_recipe_SearchParams;

	void SearchRecipies ( string resultName, std::vector<sLogbookRecipe> & );
	void SearchIngredients ( string ingredientName, std::vector<sLogbookRecipe> & );

	// =====
private:	// Data Logbook
	// Loads the full logbook into memory
	void LoadFullLogbook ( void );

	// Update frame-by-frame code for logbook.
	void UpdateLogbook ( Vector2d vMousePos ); //old
	void UpdateLogbookView ( Vector2d vMousePos ); //new
	void RenderLogbook ( void );

	// Logbook storage types
	struct sLogbookEntry
	{
		string			name;
		//vector<string>	data;
		string			filename;
	};
	struct sLogbookGroup
	{
		string					name;
		std::vector<sLogbookGroup*>	subgroups;
		std::vector<sLogbookEntry*>	entries;
	};

	sLogbookGroup	root_logbook;

	void DebugPrintGroup ( const sLogbookGroup * currentGroup, int indent );
	string & FixEntryName ( string & );

	// Logbook current display datas.
	string		s_entry_title;
	string		s_entry_description;
	sLogbookEntry	lb_current_entry;
	std::vector<string>	s_subentries;

	void UpdatePageInfo ( void );

	// Logbook current position holders
	sLogbookEntry*	p_holder_CurrentEntry;
	sLogbookGroup*	p_holder_CurrentGroup;
	string			s_holder_CurrentSubject;
	bool			b_holder_InSubjectView;

	void UpdateHolderPosition ( const bool bClickedSubject );

	void UpdateDisplayedEntry ( const int entryIndex );

	// Logbook current mouseover position
	int	i_mouseOver_Offset;

private:
	//ftype	fLogbookOffset;
	Vector2d	position;
	Vector2d	anim_position;
	Vector2d	offset;

private:
	// Logbook drawing stuffs
	glMaterial*		matDrawer;

	CBitmapFont*	fntPageTitle;
	CBitmapFont*	fntPageText;
	CBitmapFont*	fntLogbookHelp;
	CTexture*		texNull;

	// Should logbook graphics be moved to DuskGUI?
};

#endif