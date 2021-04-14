#include "arStringEnum.h"

#include "core-ext/core-ext.h"
#include <vector>

// Listing of all string enums
static std::vector<arStringEnumDefinition*> m_stringEnums;

// Clear up all string enums when the application ends
static void CleanupAllStringEnumInstances ( void )
{
	for (auto& stringEnum : m_stringEnums)
	{
		delete stringEnum;
	}
	m_stringEnums.clear();
}
REGISTER_ON_APPLICATION_END(CleanupAllStringEnumInstances);

arStringEnumDefinition* arStringEnumDefinition::CreateNew(std::vector<NameValue>& enumInfo)
{
	return new arStringEnumDefinition(enumInfo);
}

arStringEnumDefinition::arStringEnumDefinition (std::vector<NameValue>& enumInfo)
	: m_enumInfo(enumInfo)
{
	m_stringEnums.push_back(this);
}

void arStringEnumDefinition::Free ( arStringEnumDefinition* definition )
{
	auto enumEntry = std::find(m_stringEnums.begin(), m_stringEnums.end(), definition);
	if (enumEntry == m_stringEnums.end())
	{
		throw core::InvalidArgumentException();
	}
	m_stringEnums.erase(enumEntry);
	delete definition;
}

arStringEnumValue arStringEnumDefinition::CreateValue ( const char* enumName )
{
	int32_t enumIndex = FindEnumValue(enumName);
	return arStringEnumValue(this, enumIndex);
}

arStringEnumValue arStringEnumDefinition::CreateValue ( const int32_t enumValue )
{
	int32_t enumIndex = FindEnumValue(enumValue);
	return arStringEnumValue(this, enumIndex);
}

//	CreateValueFromIndex() : Creates a value with the given enum index
arStringEnumValue arStringEnumDefinition::CreateValueFromIndex ( const int32_t enumIndex )
{
	if (enumIndex < GetSize() && enumIndex >= 0)
	{
		return arStringEnumValue(this, enumIndex);
	}
	else
	{
		return arStringEnumValue(this, kInvalidValue);
	}
}