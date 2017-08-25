#ifndef _CSkillTree_H_
#define _CSkillTree_H_

#include "core/math/Rect.h"
#include "after/entities/character/CAfterPlayer.h"
#include "after/entities/item/skill/CSkill.h"
#include <vector>

using std::vector;

struct skillNode
{
	CSkillReferenceBase*	currentSkill;
	CTexture*				icon;
	Rect		skillPos;
	std::vector<skillNode*>	parentSkill;
	std::vector<int>		parentSkillReq;
	std::vector<bool>		parentSkillProxy;
	std::vector<skillNode*>	childrenSkill;
};
struct skilltree_element
{
	uint		type;
	uint		subtype;
	std::string	content;
	Vector2d	position;
};

class CSkillTree
{
	ClassName ("PlayerSkillTree");

public:
	CSkillTree(void);
	~CSkillTree(void);

	//Node place holders for the Skill Tree
	//skillNode rootNode;
	//skillNode* currentNode;	
	vector<skillNode*>	nodes;

	arstring<256>	treeName;
	Color			treeBackground;

	vector<skilltree_element>	elements;

	void ClearTree ( void );
	void ReloadSkillTree ( void );
	void LoadSkillTree ( const char* skillTree );

	void ManagedLoad ( const char* skillTree );

	// Save the skill tree levels to the map in the file. Doesn't overwrite, only edits.
	void Save ( const arstring<256>& n_location );
	// Load the skill tree levels from a map in the file. Invalid skills are ignored.
	bool Load ( const arstring<256>& n_location );
private:
	arstring<256>	m_skilltreefile;

	skillNode* FindSkillnodeWithName ( const char* name );
};



		


#endif

