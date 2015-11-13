#ifndef _C_PLAYER_SKILL_TREE_GUI_H_
#define _C_PLAYER_SKILL_TREE_GUI_H_

#include "CPlayerInventoryGUI.h"

#include "renderer/object/CRenderableObject.h"
#include "after/states/inventory/CInventory.h"
#include "engine/behavior/CGameBehavior.h"
#include "renderer/texture/CBitmapFont.h"
#include "after/states/inventory/CCrafting.h"

struct skillNode;
class CSkillTree;
class CAfterPlayer;

class CSkillTreeGUI:  public CGameBehavior, public CRenderableObject
{
public:

	ClassName("CSkillTreeGUI");

	explicit CSkillTreeGUI ( CAfterPlayer* p_player, CSkillTree* p_SkillTree, CSkillTree* p_RaceTree, CPlayerInventoryGUI* p_inventoryGUI );
	~CSkillTreeGUI(void);

	bool Render(const char pass);
	void Click(void);

	void SetVisibility(bool visibility);
	void Update(void);

	// Syncs levels to items in inventory
	void SyncLevels ( void );
private:
	void doClickyEmpty (void);

	void doClickyFull (void);

	void drawHighlights (void);

	void drawSelected (void);


	void drawTooltips (void);

	void UpgradeClick (void);

	// Draw skill node
	void DrawSkillNode ( const Real radius, const Vector2d center, const int levels, const bool source );

	//Variables
	bool bDrawSkillGUI;
	bool bSetSkillPoint;
	int skillPoint;		// Points left to allocate

	// GUI tracking
	struct lerpTracker {
		ftype	prev;
		ftype	next;
		ftype	lerp;
	};

	skillNode*	m_mouseoverNode;
	int			m_mouseoverButton;
	Vector2d	m_viewposition;
	ftype		m_viewzoom;
	lerpTracker	m_viewzoom_state;
	ftype		m_lerp_viewzoom;
	bool		m_dragging;

	ftype		m_previous_viewzoom;
	lerpTracker	m_previous_viewzoom_state;
	Vector2d	m_previous_viewposition;

	bool		m_showing_race_tree;

	//Class Objects
	CAfterPlayer*	pPlayer;
	CSkillTree* pSkillTree;
	CSkillTree* pRaceTree;
	CPlayerInventoryGUI* pInventory;
	CWeaponItem* pSelected;
	
	//Rendering Material Variables
	glMaterial* testMat;
	glMaterial* testFnt;
	glMaterial* matDrawDebug;
	CBitmapFont*	fntSkilltitle;
	CBitmapFont*	fntSkillnames;
	CBitmapFont*	fntSkilldesc;
	CTexture*	texNull;
	CTexture*	texSkillblob;
	CTexture*	texSkillline;

};

#endif