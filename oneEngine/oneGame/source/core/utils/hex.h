#ifndef CORE_UTILS_HEX_H_
#define CORE_UTILS_HEX_H_

namespace core {
namespace utils {
namespace hex {

	static constexpr char* kDigits = "0123456789ABCDEF";

	//	NumToChar() : Converts given 4-bits to a character
	static constexpr char NumToChar ( const int num )
	{
		return kDigits[num];
	}

	//	CharToNum() : Converts given character to a 4-bit value
	static constexpr char CharToNum ( const char ch )
	{
		if (ch >= 'A')
			return ch - ('A' - 0xA);
		return ch - ('0' - 0x0);
	}

}}}

#endif//CORE_UTILS_HEX_H_