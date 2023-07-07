//=onengine/core=================================================================================//
//
//		aruuid4.h
//
// Definitions for UUID4 generator class. Included as part of arguid.h.
// Adapted from https://github.com/gpakosz/uuid4
//
//===============================================================================================//
#ifndef CORE_CONTAINER_AR_UUID_4_H_
#define CORE_CONTAINER_AR_UUID_4_H_

#include <random>
#include <string>

#include "core/types/types.h"
#include "core/debug.h"
#include "core/utils/hex.h"

class aruuid4
{
public:
	explicit				aruuid4 ( void )
		: identifier{0, 0}
		{}

	explicit				aruuid4 ( const aruuid4&& other )
		: identifier{other.identifier[0], other.identifier[1]}
		{}

	explicit				aruuid4 ( const aruuid4& other)
		: identifier{other.identifier[0], other.identifier[1]}
		{}

	explicit				aruuid4 ( const char* stringValue )
	{
		setFromString(stringValue);
	}

	explicit				aruuid4 ( const std::string& stringValue )
		: aruuid4(stringValue.c_str())
		{}


	bool					operator== ( const aruuid4& other ) const
	{
		return identifier[0] == other.identifier[0] && identifier[1] == other.identifier[1];
	}
	bool					operator< ( const aruuid4& other ) const
	{
		return identifier[0] < other.identifier[0] || (identifier[0] == other.identifier[0] && identifier[1] < other.identifier[1]);
	}
	bool					operator> ( const aruuid4& other ) const
	{
		return identifier[0] > other.identifier[0] || (identifier[0] == other.identifier[0] && identifier[1] > other.identifier[1]);
	}
	bool					operator<= ( const aruuid4& other ) const
	{
		return identifier[0] < other.identifier[0] || (identifier[0] == other.identifier[0] && identifier[1] <= other.identifier[1]);
	}
	bool					operator>= ( const aruuid4& other ) const
	{
		return identifier[0] > other.identifier[0] || (identifier[0] == other.identifier[0] && identifier[1] <= other.identifier[1]);
	}
	bool					operator!= ( const aruuid4& other ) const
	{
		return identifier[0] != other.identifier[0] || identifier[1] != other.identifier[1];
	}

private:
	static constexpr int groups[] = {8, 4, 4, 4, 12};
	static constexpr size_t kStringBufferSize = sizeof("xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx") - 1; // y is either 8, 9, a or b

public:
	//	toString(output[kStringBufferSize]) : Ouputs the GUID string value into given buffer
	// Note: this is incredibly unsafe to use - be extremely careful when using it!
	void					toString ( char output_string[kStringBufferSize] )
	{
		int b = 0;
		char* output = &output_string[0];

		for (int group : groups)
		{
			// Write out a byte at a time
			for (int j = 0; j < group; j += 2)
			{
				uint8_t byte = bytes[b++];
				*output++ = core::utils::hex::NumToCharLower(byte >> 4);
				*output++ = core::utils::hex::NumToCharLower(byte & 0xf);
			}
			*output++ = '-';
		}

		*--output = 0;
	}

	//	toString() : Returns a string of the GUID value
	std::string				toString ( void )
	{
		char string_buffer [kStringBufferSize + 1] = {};
		toString(string_buffer);
		return std::string(string_buffer, kStringBufferSize);
	}

	//	setFromString(stringValue) : Sets from the given string value
	void					setFromString ( const char* stringValue )
	{
		ARCORE_ASSERT(strlen(stringValue) >= 8);

		identifier[0] = 0x0;
		identifier[1] = 0x0;

		int b = 0;
		int offset = 0;

		for (int group : groups)
		{
			// Read in a byte at a time:
			for (int j = 0; j < group; j += 2)
			{
				bytes[b++] =
					   core::utils::hex::CharLowerToNum(stringValue[offset + 0])
					| (core::utils::hex::CharLowerToNum(stringValue[offset + 1]) << 4);
				offset += 2;
			}
			ARCORE_ASSERT(stringValue[offset] == '-' || offset >= kStringBufferSize);
			offset += 1;
		}
	}

	//	setFromString(stringValue) : Sets from the given string value
	void					setFromString ( const std::string& stringValue )
	{
		ARCORE_ASSERT(stringValue.size() >= kStringBufferSize);
		setFromString(stringValue.c_str());
	}

private:
	static std::mt19937_64	generator;

public:
	//	generate() : Generates a new GUID value.
	void					generate ( void )
	{
		identifier[0] = generator();
		identifier[1] = generator();

		// Do the twiddles that mark this GUID as a GUID4 value
		bytes[6] = (bytes[6] & 0x0F) | 0x40;
		bytes[8] = (bytes[8] & 0x3F) | 0x80;
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
		return identifier[0] == 0 && identifier[1] == 0;
	}

public:
	union
	{
		uint64_t		identifier[2];
		uint8_t			bytes[16];
	};
};

#endif//CORE_CONTAINER_AR_UUID_4_H_