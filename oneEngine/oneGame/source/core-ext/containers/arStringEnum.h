//=onengine/core=================================================================================//
//
//		arStringEnum.h
//
// Definitions for an externally-loaded enum class.
//
//===============================================================================================//
#ifndef CORE_CONTAINER_STRING_ENUM_H_
#define CORE_CONTAINER_STRING_ENUM_H_

#include "core/types.h"
#include "core/exceptions.h"

#include <vector>
#include <string>

class arStringEnumValue;

class arStringEnumDefinition
{
public:
	// Definition of a "StringEnum."
	struct NameValue
	{
		std::string		name;
		int32_t			value;
	};
private:
	std::vector<NameValue>
						m_enumInfo;

public:
	//	CreateNew(enumInfo) : Creates a new persistent string enum definition.
	// String enums are freed at the end of the lifetime, or when explicitly freed.
	CORE_API static arStringEnumDefinition*
							CreateNew ( std::vector<NameValue>& enumInfo );

	//	Free(definition) : Frees specific string enum definition.
	CORE_API void			Free ( arStringEnumDefinition* definition );

private:
	CORE_API explicit		arStringEnumDefinition (std::vector<NameValue>& enumInfo); // Hidden constructor.

public:
	//	CreateValue() : Creates a value that matches the given enum name
	CORE_API arStringEnumValue
							CreateValue ( const char* enumName );

	//	CreateValue() : Creates a value that matches the given enum value
	CORE_API arStringEnumValue
							CreateValue ( const int32_t enumValue );

	//	GetSize() : Number of elements in the enum
	size_t					GetSize ( void )
	{
		return m_enumInfo.size();
	}

	//	GetEnumName() : The name of the Nth enum.
	const char*				GetEnumName ( const int32_t enumIndex )
	{
		if (enumIndex == -1)
		{
			return nullptr;
		}
		else if (enumIndex < 0 || enumIndex > m_enumInfo.size())
		{
			throw core::CorruptedDataException();
		}
		return m_enumInfo[enumIndex].name.c_str();
	}

	//	GetEnumValue() : The value of the Nth enum.
	int32_t					GetEnumValue ( const int32_t enumIndex )
	{
		if (enumIndex < 0 || enumIndex > m_enumInfo.size())
		{
			throw core::CorruptedDataException();
		}
		return m_enumInfo[enumIndex].value;
	}

private:
	int32_t				FindEnumValue ( const char* enumName )
	{
		for (int32_t index = 0; index < m_enumInfo.size(); ++index)
		{
			if (m_enumInfo[index].name == enumName)
			{
				return index;
			}
		}
		return -1;
	}
	int32_t				FindEnumValue ( const int32_t enumValue )
	{
		for (int32_t index = 0; index < m_enumInfo.size(); ++index)
		{
			if (m_enumInfo[index].value == enumValue)
			{
				return index;
			}
		}
		return -1;
	}
};

class arStringEnumValue
{
private:
	friend arStringEnumDefinition;

	arStringEnumValue(arStringEnumDefinition* definition, int32_t index)
		: m_definition(definition)
		, m_enumIndex(index)
	{}

public:

	//	IsValid() : Return if enum is valid
	bool					IsValid ( void )
	{
		return m_definition != nullptr && m_enumIndex >= 0 && m_enumIndex < m_definition->GetSize();
	}

	//	GetName() : Return name of the enum
	const char*				GetName ( void )
	{
		return m_definition->GetEnumName(m_enumIndex);
	}

	//	GetValue() : Return underlying value of the enum
	const int32_t			GetValue ( void )
	{
		return m_definition->GetEnumValue(m_enumIndex);
	}

	arStringEnumValue&		operator=(const arStringEnumValue& other)
	{
		if (other.m_definition != m_definition)
		{
			throw core::InvalidArgumentException();
		}
		m_enumIndex = other.m_enumIndex;
	}

protected:
	arStringEnumDefinition*
						m_definition;
	int32_t				m_enumIndex;
};

#endif//CORE_CONTAINER_STRING_ENUM_H_