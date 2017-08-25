
#ifndef _C_SKILL_REFERENCE_H_
#define _C_SKILL_REFERENCE_H_

#include <string>
#include "core/types/float.h"

class CCharacter;
class CLuaSkill;
class CWeaponItem;

class CSkillReferenceBase 
{
public:
	virtual void	SetEntityName ( const std::string& )=0;
	virtual const std::string	GetEntityName ( void )=0;

	virtual const std::string	GetName ( void )=0;
	virtual const std::string	GetDescription ( void )=0;
	virtual const std::string	GetThisLevelInfo ( void )=0;
	virtual const std::string	GetNextLevelInfo ( void )=0;

	virtual void	GetThisLevelCost ( Real& o_health, Real& o_stamina, Real& o_mana )=0;
	virtual void	GetNextLevelCost ( Real& o_health, Real& o_stamina, Real& o_mana )=0;

	virtual const std::string	GetIconFilename ( void )=0;

	virtual bool	GetIsPassive ( void )=0;
	virtual void	DoPassiveSet ( CCharacter* character )=0;

	virtual CWeaponItem*	Instantiate ( void )=0;
	virtual short			GetID ( void )=0;

public:
	short	m_level;
};

template <typename Skill, short ID>
class CSkillReference : public CSkillReferenceBase
{
public:
	CSkillReference ( void )
	{
		m_level = 0;
	}

	void SetEntityName ( const std::string& ) { }
	const std::string GetEntityName ( void ) {
		return "";
	}

	const std::string GetName ( void ) override
	{
		return Skill::GetName();
	}
	const std::string GetDescription ( void ) override
	{
		return Skill::GetDescription();
	}
	const std::string GetThisLevelInfo ( void ) override
	{
		return Skill::GetThisLevelInfo(m_level);
	}
	const std::string GetNextLevelInfo ( void ) override
	{
		return Skill::GetNextLevelInfo(m_level);
	}

	void GetThisLevelCost ( Real& o_health, Real& o_stamina, Real& o_mana ) override
	{
		return Skill::GetThisLevelCost(m_level,o_health,o_stamina,o_mana);
	}
	void GetNextLevelCost ( Real& o_health, Real& o_stamina, Real& o_mana ) override
	{
		return Skill::GetNextLevelCost(m_level,o_health,o_stamina,o_mana);
	}

	const std::string GetIconFilename ( void ) override
	{
		return Skill::GetIconFilename();
	}

	bool GetIsPassive ( void ) override
	{
		return Skill::GetIsPassive();
	}
	void DoPassiveSet ( CCharacter* character ) override
	{
		Skill::DoPassiveSet( character, m_level );
	}

	CWeaponItem* Instantiate ( void ) override
	{
		Skill* result = (Skill*)CWeaponItem::Instantiate(ID);
		result->SetLevel( m_level );
		return (CWeaponItem*)result;
	}
	short GetID ( void ) override
	{
		return ID;
	}

public:
	//
};

//#include "CLuaSkill.h"

template <>
class CSkillReference<CLuaSkill,-3> : public CSkillReferenceBase
{
public:
	CSkillReference ( void )
	{
		m_level = 0;
		m_entityName = "";
	}

	void SetEntityName ( const std::string& nname )
	{
		m_entityName = nname;
	}
	const std::string GetEntityName ( void )
	{
		return m_entityName;
	}

	const std::string GetName ( void ) override;
	const std::string GetDescription ( void ) override;
	const std::string GetThisLevelInfo ( void ) override;
	const std::string GetNextLevelInfo ( void ) override;
	void GetThisLevelCost ( Real& o_health, Real& o_stamina, Real& o_mana ) override;
	void GetNextLevelCost ( Real& o_health, Real& o_stamina, Real& o_mana ) override;

	const std::string GetIconFilename ( void ) override;

	bool GetIsPassive ( void ) override;
	void DoPassiveSet ( CCharacter* character ) override;
	CWeaponItem* Instantiate ( void ) override;

	short GetID ( void ) override
	{
		return -3;
	}

public:
	//
	std::string m_entityName;
};

#endif//_C_SKILL_REFERENCE_H_