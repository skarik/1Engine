
#ifndef _C_BOOB_BITS_HPP_
#define _C_BOOB_BITS_HPP_

#include "standard_types.h"

inline unsigned char GetWireBit ( uint32_t data )
{
	return (unsigned char)( data & 0x1 );
}
inline void SetWireBit ( uint32_t &data, unsigned char newBit )
{
	data = ((data & ~0x1) | (newBit & 0x1));
}
inline void SetWireBitChar ( unsigned char &data, unsigned char newBit )
{
	data = ((data & ~0x1) | (newBit & 0x1));
}

inline char GetPowerBit ( uint32_t data )
{
	return (unsigned char)( data & 0x2 );
}
inline void SetPowerBit ( uint32_t &data, unsigned char newBit )
{
	data = ((data & ~0x2) | ((newBit<<1) & 0x2));
}
inline void SetPowerBitChar ( unsigned char &data, unsigned char newBit )
{
	data = ((data & ~0x2) | ((newBit<<1) & 0x2));
}

inline unsigned char GetSource ( uint32_t data )
{
	return ( (data & 0x3) == 0x2 );
}


#endif