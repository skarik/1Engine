
#ifndef _DIRECTION_FLAGS_H_
#define _DIRECTION_FLAGS_H_

namespace Terrain
{
	// Direction enumeration
	enum EFaceDir
	{
		FRONT	=1,
		BACK	=2,
		RIGHT	=4,
		LEFT	=8,
		TOP		=16,
		BOTTOM	=32,

		X_POS	=1,
		X_NEG	=2,
		Y_POS	=4,
		Y_NEG	=8,
		Z_POS	=16,
		Z_NEG	=32

		/*
		TOP = 1,
		BOTTOM,
		FRONT,
		BACK,
		LEFT,
		RIGHT
		*/
	};
}

#endif//_DIRECTION_FLAGS_H_