//=onengine/core=================================================================================//
//
//		arguid.h
//
// Definitions for GUID generator classes. Has several utility functions for ensuring uniqueness
// within a container.
//
//===============================================================================================//
#ifndef CORE_CONTAINER_AR_GUID_H_
#define CORE_CONTAINER_AR_GUID_H_

#include <random>
#include <string>

#include "core/types/types.h"
#include "core/debug.h"
#include "core/utils/hex.h"

class arguid32
{
public:
	explicit				arguid32 ( void )
		: identifier(0)
		{}

	explicit				arguid32 ( const arguid32&& other )
		: identifier(other.identifier)
		{}

	explicit				arguid32 ( const arguid32& other)
		: identifier(other.identifier)
		{}

	explicit				arguid32 ( const char* stringValue )
	{
		setFromString(stringValue);
	}

	explicit				arguid32 ( const std::string& stringValue )
		: arguid32(stringValue.c_str())
		{}

	bool					operator== ( const arguid32& other ) const
	{
		return identifier == other.identifier;
	}
	bool					operator< ( const arguid32& other ) const
	{
		return identifier < other.identifier;
	}
	bool					operator> ( const arguid32& other ) const
	{
		return identifier > other.identifier;
	}
	bool					operator<= ( const arguid32& other ) const
	{
		return identifier <= other.identifier;
	}
	bool					operator>= ( const arguid32& other ) const
	{
		return identifier >= other.identifier;
	}
	bool					operator!= ( const arguid32& other ) const
	{
		return identifier != other.identifier;
	}

public:
	//	toString(output[8]) : Ouputs the GUID string value into given buffer
	// Note: this is incredibly unsafe to use - be extremely careful when using it!
	void					toString ( char output_string[8] )
	{
		for (int32_t offset = 0; offset < 8; ++offset)
		{
			output_string[offset] = core::utils::hex::NumToChar( (identifier >> (offset * 4)) & 0x0F );
		}
	}

	//	toString() : Returns a string of the GUID value
	std::string				toString ( void )
	{
		char string_buffer [8];
		toString(string_buffer);
		return std::string(string_buffer, 8);
	}

	//	setFromString(stringValue) : Sets from the given string value
	void					setFromString ( const char* stringValue )
	{
		ARCORE_ASSERT(strlen(stringValue) >= 8);

		identifier = 0x0;
		for (int32_t offset = 0; offset < 8; ++offset)
		{
			identifier |= core::utils::hex::CharToNum(stringValue[offset]) << (offset * 4);
		}
	}

	//	setFromString(stringValue) : Sets from the given string value
	void					setFromString ( const std::string& stringValue )
	{
		ARCORE_ASSERT(stringValue.size() >= 8);
		setFromString(stringValue.c_str());
	}

private:
	static std::mt19937	generator;

public:
	//	generate() : Generates a new GUID value.
	void					generate ( void )
	{
		identifier = generator();
	}

	//	generateDistinctTo(container) : Generates a new GUID value that is unique within the given container.
	template <class ContainerType>
	void					generateDistinctTo ( const ContainerType& container )
	{
		do
		{
			generate();
		}
		while (!isDistinctTo(container));
	}
	
	//	isDistinctTo(container) : Checks if the current GUID is unique to the given container.
	template <class ContainerType>
	bool					isDistinctTo ( const ContainerType& container ) const
	{
		for (const arguid32* guid : container)
		{
			if (*guid == *this)
			{
				return false;
			}
		}
		return true;
	}

	//	isNil() : Checks if this is a nil UID
	bool					isNil ( void ) const
	{
		return identifier == 0;
	}

public:
	uint32_t			identifier;
};

// Inject hash-table (std::unordered_map) functionality for arstring into the STD namespace
#include <functional>
namespace std
{
	template <>
	struct hash<arguid32>
	{
		size_t operator()(const arguid32& guid) const
		{	
			return std::hash<uint32_t>()(guid.identifier);
		}
	};
}

#endif//CORE_CONTAINER_AR_GUID_H_