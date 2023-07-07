#ifndef CORE_UTILS_HEX_H_
#define CORE_UTILS_HEX_H_

namespace core {
namespace utils {
namespace hex {

	static constexpr char* kDigits = "0123456789ABCDEF";
	static constexpr char* kDigitsLower = "0123456789abcdef";

	//	NumToChar() : Converts given 4-bits to a character
	static constexpr char NumToChar ( const int num )
	{
		return kDigits[num];
	}

	//	NumToCharLower() : Converts given 4-bits to a character
	static constexpr char NumToCharLower ( const int num )
	{
		return kDigitsLower[num];
	}

	//	CharToNum() : Converts given character to a 4-bit value
	static constexpr char CharToNum ( const char ch )
	{
		if (ch >= 'A')
			return ch - ('A' - 0xA);
		return ch - ('0' - 0x0);
	}

	//	CharLowerToNum() : Converts given character to a 4-bit value
	static constexpr char CharLowerToNum ( const char ch )
	{
		if (ch >= 'a')
			return ch - ('a' - 0xA);
		return ch - ('0' - 0x0);
	}

}}}

#endif//CORE_UTILS_HEX_H_