
#include "CSkillReference.h"

#include "after/lua/CLuaSkill.h"

const std::string CSkillReference<CLuaSkill,-3>::GetName ( void )
{
	return CLuaSkill::GetName(m_entityName.c_str());
}

const std::string CSkillReference<CLuaSkill,-3>::GetDescription ( void )
{
	return CLuaSkill::GetDescription(m_entityName.c_str());
}

const std::string CSkillReference<CLuaSkill,-3>::GetThisLevelInfo ( void )
{
	return CLuaSkill::GetThisLevelInfo(m_entityName.c_str(),m_level);
}
const std::string CSkillReference<CLuaSkill,-3>::GetNextLevelInfo ( void )
{
	return CLuaSkill::GetNextLevelInfo(m_entityName.c_str(),m_level);
}

void CSkillReference<CLuaSkill,-3>::GetThisLevelCost ( Real& o_health, Real& o_stamina, Real& o_mana )
{
	return CLuaSkill::GetThisLevelCost(m_entityName.c_str(),m_level,o_health,o_stamina,o_mana);
}
void CSkillReference<CLuaSkill,-3>::GetNextLevelCost ( Real& o_health, Real& o_stamina, Real& o_mana )
{
	return CLuaSkill::GetNextLevelCost(m_entityName.c_str(),m_level,o_health,o_stamina,o_mana);
}

const std::string CSkillReference<CLuaSkill,-3>::GetIconFilename ( void )
{
	return CLuaSkill::GetIconFilename(m_entityName.c_str());
}

bool CSkillReference<CLuaSkill,-3>::GetIsPassive ( void )
{
	return CLuaSkill::GetIsPassive(m_entityName.c_str());
}
void CSkillReference<CLuaSkill,-3>::DoPassiveSet ( CCharacter* character )
{
	CLuaSkill::DoPassiveSet( m_entityName.c_str(), character, m_level );
}

CWeaponItem* CSkillReference<CLuaSkill,-3>::Instantiate ( void )
{
	CLuaSkill* result = (CLuaSkill*)CWeaponItem::Instantiate(-3);
	result->SetLevel( m_level );
	result->SetEntityName( m_entityName.c_str() );
	return (CWeaponItem*)result;
}


/*
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

	const std::string GetName ( void ) override
	{
		return CLuaSkill::GetName(m_entityName.c_str());
	}
	const std::string GetDescription ( void ) override
	{
		return CLuaSkill::GetDescription(m_entityName.c_str());
	}
	const std::string GetThisLevelInfo ( void ) override
	{
		return CLuaSkill::GetThisLevelInfo(m_entityName.c_str(),m_level);
	}
	const std::string GetNextLevelInfo ( void ) override
	{
		return CLuaSkill::GetNextLevelInfo(m_entityName.c_str(),m_level);
	}

	void GetThisLevelCost ( Real& o_health, Real& o_stamina, Real& o_mana ) override
	{
		return CLuaSkill::GetThisLevelCost(m_entityName.c_str(),m_level,o_health,o_stamina,o_mana);
	}
	void GetNextLevelCost ( Real& o_health, Real& o_stamina, Real& o_mana ) override
	{
		return CLuaSkill::GetNextLevelCost(m_entityName.c_str(),m_level,o_health,o_stamina,o_mana);
	}

	const std::string GetIconFilename ( void ) override
	{
		return CLuaSkill::GetIconFilename(m_entityName.c_str());
	}

	bool GetIsPassive ( void ) override
	{
		return CLuaSkill::GetIsPassive(m_entityName.c_str());
	}
	void DoPassiveSet ( CCharacter* character ) override
	{
		CLuaSkill::DoPassiveSet( m_entityName.c_str(), character, m_level );
	}

	CWeaponItem* Instantiate ( void ) override
	{
		CLuaSkill* result = (CLuaSkill*)CWeaponItem::Instantiate(-3);
		result->SetLevel( m_level );
		result->SetEntityName( m_entityName.c_str() );
		return (CWeaponItem*)result;
	}
	short GetID ( void ) override
	{
		return -3;
	}

public:
	//
	std::string m_entityName;
};*/