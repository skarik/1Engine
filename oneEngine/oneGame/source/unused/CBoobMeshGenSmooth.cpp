//
/*


#include "CBoob.h"
#include "CRenderState.h"
#include "CVoxelTerrain.h"

// Define lookup tables
const int edgeTable[256]={
	0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
	0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
	0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
	0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
	0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
	0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
	0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
	0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
	0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
	0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
	0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
	0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
	0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
	0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
	0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
	0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
	0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
	0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
	0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
	0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
	0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
	0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
	0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
	0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
	0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
	0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
	0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
	0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
	0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
	0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
	0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
	0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };
const int triTable[256][16] =
	{{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
	{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
	{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
	{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
	{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
	{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
	{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
	{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
	{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
	{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
	{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
	{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
	{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
	{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
	{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
	{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
	{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
	{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
	{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
	{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
	{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
	{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
	{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
	{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
	{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
	{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
	{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
	{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
	{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
	{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
	{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
	{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
	{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
	{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
	{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
	{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
	{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
	{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
	{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
	{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
	{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
	{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
	{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
	{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
	{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
	{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
	{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
	{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
	{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
	{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
	{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
	{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
	{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
	{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
	{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
	{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
	{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
	{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
	{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
	{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
	{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
	{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
	{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
	{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
	{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
	{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
	{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
	{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
	{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
	{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
	{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
	{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
	{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
	{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
	{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
	{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
	{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
	{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
	{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
	{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
	{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
	{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
	{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
	{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
	{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
	{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
	{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
	{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
	{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
	{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
	{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
	{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
	{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
	{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
	{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
	{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
	{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

void CBoobMesh::GenerateInVoxel ( CBoob * pBoob, char const index16, char const index8, int const indexd, Vector3d const& pos, char const res )
{
	static const int bsize = 2;
	char grid[8];
	unsigned char light[8];
	char majorGridVal = EB_NONE;

	// Get the eight values we need
	for ( char i = 0; i < 8; ++i )
	{
		if ( !GetBlockValue( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data,indexd,8, i, res, grid[i], light[i] ) )
			return;
	}
	
	// Create the cube index
	static int cubeindex;
	cubeindex = 0;
	if ( IsSolid(grid[0]) ) cubeindex |= 1;
	if ( IsSolid(grid[1]) ) cubeindex |= 2;
	if ( IsSolid(grid[2]) ) cubeindex |= 4;
	if ( IsSolid(grid[3]) ) cubeindex |= 8;
	if ( IsSolid(grid[4]) ) cubeindex |= 16;
	if ( IsSolid(grid[5]) ) cubeindex |= 32;
	if ( IsSolid(grid[6]) ) cubeindex |= 64;
	if ( IsSolid(grid[7]) ) cubeindex |= 128;

	// Cube is entirely in/out of the surface
	if ( edgeTable[cubeindex] == 0 )
		return;

	// Get the major cube type (todo: build lookup table for this)
	//for ( char i = 0; i < 8; ++i )
	//{
	//	if ( grid[i] != EB_NONE )
	//	{
	//		majorGridVal = grid[i];
	//		if (( majorGridVal != EB_GRASS )||( i >= 4 ))
	//		{
	//			break;
	//		}
	//	}
	//}

	// Create the cube point list
	Vector3d gridvList[8];
	for ( char i = 0; i < 4; ++i )
		gridvList[i] = pos+Vector3d( bsize*0.5f*res,bsize*0.5f*res,bsize*0.5f*res );
	gridvList[1].x += bsize*res;
	gridvList[2].x += bsize*res;
	gridvList[2].y += bsize*res;
	gridvList[3].y += bsize*res;
	for ( char i = 4; i < 8; ++i )
	{
		gridvList[i] = gridvList[i-4];
		gridvList[i].z += bsize*res;
	}

	// Create the cube texture coordinate list (todo: move this to a constant)
	Vector3d gridtList[8];
	gridtList[0] = Vector3d( 0,0,0 );
	gridtList[1] = Vector3d( res,0,0 );
	gridtList[2] = Vector3d( res,res,0 );
	gridtList[3] = Vector3d( 0,res,0 );
	gridtList[4] = Vector3d( 0,0,res );
	gridtList[5] = Vector3d( res,0,res );
	gridtList[6] = Vector3d( res,res,res );
	gridtList[7] = Vector3d( 0,res,res );
	//for ( char i = 0; i < 8; ++i )
	//	gridtList[i] -= Vector3d( res/2.0f,res/2.0f,res/2.0f );

	// Create the cube normal coordinate list
	Vector3d normaltVal;
	if ( !IsSolid(grid[0]) ) normaltVal += Vector3d( -1,-1,-1 );
	if ( !IsSolid(grid[1]) ) normaltVal += Vector3d(  1,-1,-1 );
	if ( !IsSolid(grid[2]) ) normaltVal += Vector3d(  1, 1,-1 );
	if ( !IsSolid(grid[3]) ) normaltVal += Vector3d( -1, 1,-1 );
	if ( !IsSolid(grid[4]) ) normaltVal += Vector3d( -1,-1, 1 );
	if ( !IsSolid(grid[5]) ) normaltVal += Vector3d(  1,-1, 1 );
	if ( !IsSolid(grid[6]) ) normaltVal += Vector3d(  1, 1, 1 );
	if ( !IsSolid(grid[7]) ) normaltVal += Vector3d( -1, 1, 1 );
	normaltVal = normaltVal.normal();

	// Create the intersection list
	Vector3d vertlist[12];
	Vector3d texclist[12]; // todo: move this to a constant

	if (edgeTable[cubeindex] & 1)
		vertlist[0] = (gridvList[0]+gridvList[1])*0.5f;
	if (edgeTable[cubeindex] & 2)
		vertlist[1] = (gridvList[1]+gridvList[2])*0.5f;
	if (edgeTable[cubeindex] & 4)
		vertlist[2] = (gridvList[2]+gridvList[3])*0.5f;
	if (edgeTable[cubeindex] & 8)
		vertlist[3] = (gridvList[3]+gridvList[0])*0.5f;
	if (edgeTable[cubeindex] & 16)
		vertlist[4] = (gridvList[4]+gridvList[5])*0.5f;
	if (edgeTable[cubeindex] & 32)
		vertlist[5] = (gridvList[5]+gridvList[6])*0.5f;
	if (edgeTable[cubeindex] & 64)
		vertlist[6] = (gridvList[6]+gridvList[7])*0.5f;
	if (edgeTable[cubeindex] & 128)
		vertlist[7] = (gridvList[7]+gridvList[4])*0.5f;
	if (edgeTable[cubeindex] & 256)
		vertlist[8] = (gridvList[0]+gridvList[4])*0.5f;
	if (edgeTable[cubeindex] & 512)
		vertlist[9] = (gridvList[1]+gridvList[5])*0.5f;
	if (edgeTable[cubeindex] & 1024)
		vertlist[10] = (gridvList[2]+gridvList[6])*0.5f;
	if (edgeTable[cubeindex] & 2048)
		vertlist[11] = (gridvList[3]+gridvList[7])*0.5f;
	//if (edgeTable[cubeindex] & 1)
	//	vertlist[0] = VertexInterp( 0.5f, gridvList[0], gridvList[1], gridvals[0], gridvals[1] );
	//if (edgeTable[cubeindex] & 2)
	//	vertlist[1] = VertexInterp( 0.5f, gridvList[1], gridvList[2], gridvals[1], gridvals[2] );
	//if (edgeTable[cubeindex] & 4)
	//	vertlist[2] = VertexInterp( 0.5f, gridvList[2], gridvList[3], gridvals[2], gridvals[3] );
	//if (edgeTable[cubeindex] & 8)
	//	vertlist[3] = VertexInterp( 0.5f, gridvList[3], gridvList[0], gridvals[3], gridvals[0] );
	//if (edgeTable[cubeindex] & 16)
	//	vertlist[4] = VertexInterp( 0.5f, gridvList[4], gridvList[5], gridvals[4], gridvals[5] );
	//if (edgeTable[cubeindex] & 32)
	//	vertlist[5] = VertexInterp( 0.5f, gridvList[5], gridvList[6], gridvals[5], gridvals[6] );
	//if (edgeTable[cubeindex] & 64)
	//	vertlist[6] = VertexInterp( 0.5f, gridvList[6], gridvList[7], gridvals[6], gridvals[7] );
	//if (edgeTable[cubeindex] & 128)
	//	vertlist[7] = VertexInterp( 0.5f, gridvList[7], gridvList[4], gridvals[7], gridvals[4] );
	//if (edgeTable[cubeindex] & 256)
	//	vertlist[8] = VertexInterp( 0.5f, gridvList[0], gridvList[4], gridvals[0], gridvals[4] );
	//if (edgeTable[cubeindex] & 512)
	//	vertlist[9] = VertexInterp( 0.5f, gridvList[1], gridvList[5], gridvals[1], gridvals[5] );
	//if (edgeTable[cubeindex] & 1024)
	//	vertlist[10] = VertexInterp( 0.5f, gridvList[2], gridvList[6], gridvals[2], gridvals[6] );
	//if (edgeTable[cubeindex] & 2048)
	//	vertlist[11] = VertexInterp( 0.5f, gridvList[3], gridvList[7], gridvals[3], gridvals[7] );

	texclist[0] = (gridtList[0]+gridtList[1])*0.5f;
	texclist[1] = (gridtList[1]+gridtList[2])*0.5f;
	texclist[2] = (gridtList[2]+gridtList[3])*0.5f;
	texclist[3] = (gridtList[3]+gridtList[0])*0.5f;
	texclist[4] = (gridtList[4]+gridtList[5])*0.5f;
	texclist[5] = (gridtList[5]+gridtList[6])*0.5f;
	texclist[6] = (gridtList[6]+gridtList[7])*0.5f;
	texclist[7] = (gridtList[7]+gridtList[4])*0.5f;
	texclist[8] = (gridtList[0]+gridtList[4])*0.5f;
	texclist[9] = (gridtList[1]+gridtList[5])*0.5f;
	texclist[10] = (gridtList[2]+gridtList[6])*0.5f;
	texclist[11] = (gridtList[3]+gridtList[7])*0.5f;

	// Create the triangles
	for ( int i = 0; triTable[cubeindex][i] != -1; i += 3 )
	{
		// Based on the coords, add weights
		char weightTable [8] = {0,0,0,0,0,0,0,0};
		//==============
		for ( int j = 0; j < 3; ++j )
		{
			if ( triTable[cubeindex][i+j] == 0 )
			{
				++(weightTable[0]);
				++(weightTable[1]);
			}
			else if ( triTable[cubeindex][i+j] == 1 )
			{
				++(weightTable[1]);
				++(weightTable[2]);
			}
			else if ( triTable[cubeindex][i+j] == 2 )
			{
				++(weightTable[2]);
				++(weightTable[3]);
			}
			else if ( triTable[cubeindex][i+j] == 3 )
			{
				++(weightTable[3]);
				++(weightTable[0]);
			}
			else if ( triTable[cubeindex][i+j] == 4 )
			{
				++(weightTable[4]);
				++(weightTable[5]);
			}
			else if ( triTable[cubeindex][i+j] == 5 )
			{
				++(weightTable[5]);
				++(weightTable[6]);
			}
			else if ( triTable[cubeindex][i+j] == 6 )
			{
				++(weightTable[6]);
				++(weightTable[7]);
			}
			else if ( triTable[cubeindex][i+j] == 7 )
			{
				++(weightTable[7]);
				++(weightTable[0]);
			}
			else if ( triTable[cubeindex][i+j] == 8 )
			{
				++(weightTable[0]);
				++(weightTable[4]);
			}
			else if ( triTable[cubeindex][i+j] == 9 )
			{
				++(weightTable[1]);
				++(weightTable[5]);
			}
			else if ( triTable[cubeindex][i+j] == 10 )
			{
				++(weightTable[2]);
				++(weightTable[6]);
			}
			else if ( triTable[cubeindex][i+j] == 11 )
			{
				++(weightTable[3]);
				++(weightTable[7]);
			}
		}
		// Find max weight
		int maxWeightIndex = 0;
		for ( int j = 0; j < 8; ++j )
		{
			if ( weightTable[j] > weightTable[maxWeightIndex] )
			{
				//if ( IsSolid( grid[j] ) )
					maxWeightIndex = j;
			}
		}
		if ( !IsSolid( grid[maxWeightIndex] ) )
		{
			// find min weight
			for ( int j = 0; j < 8; ++j )
			{
				if ( weightTable[j] < weightTable[maxWeightIndex] )
				{
					if ( IsSolid( grid[j] ) )
						maxWeightIndex = j;
				}
			}
		}
		if ( !IsSolid( grid[maxWeightIndex] ) )
		{
			// grab whatever we can
			for ( int j = 0; j < 8; ++j )
			{
				if ( IsSolid( grid[j] ) )
				{
					maxWeightIndex = j;
				}
			}
		}

		// Find min weight
		int minWeightIndex = 0;
		for ( int j = 0; j < 8; ++j )
		{
			if ( weightTable[j] < weightTable[minWeightIndex] )
			{
				//if ( !IsSolid( grid[j] ) )
					minWeightIndex = j;
			}
		}
		if ( IsSolid( grid[minWeightIndex] ) )
		{
			// find max weight
			for ( int j = 0; j < 8; ++j )
			{
				if ( weightTable[j] > weightTable[minWeightIndex] )
				{
					if ( !IsSolid( grid[j] ) )
						minWeightIndex = j;
				}
			}
		}
		if ( IsSolid( grid[minWeightIndex] ) )
		{
			// grab whatever we can
			for ( int j = 0; j < 8; ++j )
			{
				if ( !IsSolid( grid[j] ) )
				{
					minWeightIndex = j;
				}
			}
		}
		// and you know what? screw it, let's just get the darkest lighting
		for ( int j = 0; j < 8; ++j )
		{
			if ( light[minWeightIndex] > light[j] )
			{
				minWeightIndex = j;
			}
		}
		//==============

		// Set block
		majorGridVal = grid[maxWeightIndex];

		// Skip if the block is skip type
		//bool skip = false;
		//for ( int j = 0; j < 8; ++j )
		//{
		//	if ( weightTable[j] > 0 )
		//	{
		//		if ( grid[j] == EB_STONEBRICK )
		//		{
		//			skip = true;
		//		}
		//	}
		//}
		//if ( skip )
		//	continue;
		bool skip = true;
		for ( int j = 0; j < 8; ++j )
		{
			if ( weightTable[j] > 0 )
			{
				if (( IsSolid( grid[j] ) )&&( grid[j] != EB_STONEBRICK )&&( grid[j] != EB_WOOD ))
				{
					skip = false;
				}
			}
		}
		if ( skip )
			continue;

		// Vertex position
		vertices[vertexCount  ].x = vertlist[triTable[cubeindex][i  ]].x;
		vertices[vertexCount  ].y = vertlist[triTable[cubeindex][i  ]].y;
		vertices[vertexCount  ].z = vertlist[triTable[cubeindex][i  ]].z;

		vertices[vertexCount+1].x = vertlist[triTable[cubeindex][i+1]].x;
		vertices[vertexCount+1].y = vertlist[triTable[cubeindex][i+1]].y;
		vertices[vertexCount+1].z = vertlist[triTable[cubeindex][i+1]].z;

		vertices[vertexCount+2].x = vertlist[triTable[cubeindex][i+2]].x;
		vertices[vertexCount+2].y = vertlist[triTable[cubeindex][i+2]].y;
		vertices[vertexCount+2].z = vertlist[triTable[cubeindex][i+2]].z;

		// Hard Normals (v1)
		//{
		//	vertices[vertexCount  ].nx = normaltVal.x;
		//	vertices[vertexCount  ].ny = normaltVal.y;
		//	vertices[vertexCount  ].nz = normaltVal.z;

		//	vertices[vertexCount+1].nx = normaltVal.x;
		//	vertices[vertexCount+1].ny = normaltVal.y;
		//	vertices[vertexCount+1].nz = normaltVal.z;

		//	vertices[vertexCount+2].nx = normaltVal.x;
		//	vertices[vertexCount+2].ny = normaltVal.y;
		//	vertices[vertexCount+2].nz = normaltVal.z;
		//}

		// Hard Normals (v2)
		{
			normaltVal = 
				((Vector3d( vertices[vertexCount+2].x,vertices[vertexCount+2].y,vertices[vertexCount+2].z )
				-Vector3d( vertices[vertexCount+0].x,vertices[vertexCount+0].y,vertices[vertexCount+0].z )).cross(
				Vector3d( vertices[vertexCount+1].x,vertices[vertexCount+1].y,vertices[vertexCount+1].z )
				-Vector3d( vertices[vertexCount+0].x,vertices[vertexCount+0].y,vertices[vertexCount+0].z ) )).normal();

			vertices[vertexCount  ].nx = normaltVal.x;
			vertices[vertexCount  ].ny = normaltVal.y;
			vertices[vertexCount  ].nz = normaltVal.z;

			vertices[vertexCount+1].nx = normaltVal.x;
			vertices[vertexCount+1].ny = normaltVal.y;
			vertices[vertexCount+1].nz = normaltVal.z;

			vertices[vertexCount+2].nx = normaltVal.x;
			vertices[vertexCount+2].ny = normaltVal.y;
			vertices[vertexCount+2].nz = normaltVal.z;
		}

		// Texture Coordinates (dependant on normals)
		vertices[vertexCount  ].u = texclist[triTable[cubeindex][i  ]].x;
		vertices[vertexCount  ].v = texclist[triTable[cubeindex][i  ]].y;
		vertices[vertexCount  ].w = texclist[triTable[cubeindex][i  ]].z;

		vertices[vertexCount+1].u = texclist[triTable[cubeindex][i+1]].x;
		vertices[vertexCount+1].v = texclist[triTable[cubeindex][i+1]].y;
		vertices[vertexCount+1].w = texclist[triTable[cubeindex][i+1]].z;

		vertices[vertexCount+2].u = texclist[triTable[cubeindex][i+2]].x;
		vertices[vertexCount+2].v = texclist[triTable[cubeindex][i+2]].y;
		vertices[vertexCount+2].w = texclist[triTable[cubeindex][i+2]].z;

		SetSmoothFaceUVs( vertices, normaltVal, majorGridVal, pos*0.5f );

		quads[faceCount].vert[0] = vertexCount+2;
		quads[faceCount].vert[1] = vertexCount+1;
		quads[faceCount].vert[2] = vertexCount+0;
		quads[faceCount].vert[3] = vertexCount+0;

		{
			//float lightLevel = pBoob->data[index16].data[index8].light[indexd]/256.0f;
			float lightLevel = (( unsigned char )( light[minWeightIndex] ))/256.0f;
			vertices[vertexCount  ].r = lightLevel;
			vertices[vertexCount  ].g = lightLevel;
			vertices[vertexCount  ].b = lightLevel;
			vertices[vertexCount  ].a = 1;

			vertices[vertexCount+1].r = lightLevel;
			vertices[vertexCount+1].g = lightLevel;
			vertices[vertexCount+1].b = lightLevel;
			vertices[vertexCount+1].a = 1;

			vertices[vertexCount+2].r = lightLevel;
			vertices[vertexCount+2].g = lightLevel;
			vertices[vertexCount+2].b = lightLevel;
			vertices[vertexCount+2].a = 1;
		}

		vertexCount += 3;
		faceCount += 1;

		if ( vertexCount < 3 ) throw xcp_vertex_overflow;
		if ( vertexCount > maxVertexCount ) throw xcp_vertex_overflow;
	}
}

inline Vector3d CBoobMesh::VertexInterp( ftype isolevel, const Vector3d & p1, const Vector3d & p2, ftype valp1, ftype valp2 )
{
   ftype mu;
   Vector3d p;

   if ( fabs(isolevel-valp1) < 0.00001f )
      return(p1);
   if ( fabs(isolevel-valp2) < 0.00001f )
      return(p2);
   if ( fabs(valp1-valp2) < 0.00001f )
      return(p1);

   mu = (isolevel - valp1) / (valp2 - valp1);
   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);

   return(p);
}

inline bool CBoobMesh::IsSolid ( char blokVal )
{
	return (( blokVal != EB_NONE )&&( blokVal != EB_WATER ));
}

inline void CBoobMesh::SetSmoothFaceUVs ( CTerrainVertex* vertices, const Vector3d & inNormal, unsigned char data, const Vector3d & inOffset )
{
	EFaceDir dir = TOP;
	if ( inNormal.z < 0 )
		dir = BOTTOM;

	// Do positional coordinates
	//for ( unsigned char i = 0; i < 3; ++i )
	//{
	//	vertices[vertexCount+i].u = modf(vertices[vertexCount+0].u,&dummy);
	//	vertices[vertexCount+i].v = modf(vertices[vertexCount+0].v,&dummy);
	//}
	Vector3d finalOffset = inOffset;

	// Swap w coordinate with u or v, depending on facing
	if ( fabs(inNormal.z) < 0.8f )
	{
		if ( fabs(inNormal.x) < fabs(inNormal.y) )
		{
			vertices[vertexCount+0].v = vertices[vertexCount+0].w;
			vertices[vertexCount+1].v = vertices[vertexCount+1].w;
			vertices[vertexCount+2].v = vertices[vertexCount+2].w;

			finalOffset.y = finalOffset.z;

			dir = FRONT;
		}
		else
		{
			vertices[vertexCount+0].u = vertices[vertexCount+0].v;
			vertices[vertexCount+1].u = vertices[vertexCount+1].v;
			vertices[vertexCount+2].u = vertices[vertexCount+2].v;
			vertices[vertexCount+0].v = vertices[vertexCount+0].w;
			vertices[vertexCount+1].v = vertices[vertexCount+1].w;
			vertices[vertexCount+2].v = vertices[vertexCount+2].w;

			finalOffset.x = finalOffset.y;
			finalOffset.y = finalOffset.z;

			dir = LEFT;
		}
	}

	// Texture scale
	float texScale = 1.0f;
	if (( data == EB_DIRT )||( data == EB_GRASS )||( data == EB_SAND )||( data == EB_CLAY )||( data == EB_CRYSTAL )||( data == EB_XPLO_CRYSTAL ))
	{
		texScale = 1/4.0f;
	}
	else if (( data == EB_STONE )||( data == EB_STONEBRICK ))
	{
		texScale = 1/2.0f;
	}
	else if ( data == EB_WOOD )
	{
		//texScale = 1/1.5f;
		texScale = 1/2.0f;
	}
	if (( data == EB_GRASS )&&( dir != TOP )&&( dir != BOTTOM ))
	{
		texScale = 0.5f;
	}

	// Make texture smaller
	for ( unsigned char i = 0; i < 3; ++i )
	{
		vertices[vertexCount+i].u *= texScale;
		vertices[vertexCount+i].v *= texScale;
	}

	// Do limiting on coordinates
	float dummy;
	
	finalOffset.x = modf( finalOffset.x*texScale, &dummy );
	finalOffset.y = modf( finalOffset.y*texScale, &dummy );
	if ( finalOffset.x < 0 )
		finalOffset.x += 1;
	if ( finalOffset.y < 0 )
		finalOffset.y += 1;

	for ( unsigned char i = 0; i < 3; ++i )
	{
		vertices[vertexCount+i].u += finalOffset.x;
		vertices[vertexCount+i].v += finalOffset.y;
	}
	
	if (( data == EB_GRASS )&&( dir != TOP )&&( dir != BOTTOM ))
	{
		//finalOffset.y = modf( finalOffset.y*2.0f, &dummy ) * 0.5f;
		//finalOffset.y += 0.25f;
		//finalOffset.y = modf( (finalOffset.y-0.5f)*2.0f, &dummy ) * 0.5f;
		//finalOffset.y += 0.25f;
		bool shift;
		
		shift = false;
		for ( unsigned char i = 0; i < 3; ++i )
			if ( vertices[vertexCount+i].v > 0.751f )
				shift = true;
		if ( shift )
			for ( unsigned char i = 0; i < 3; ++i )
				vertices[vertexCount+i].v -= 0.5f;

		shift = false;
		for ( unsigned char i = 0; i < 3; ++i )
			if ( vertices[vertexCount+i].v < 0.249f )
				shift = true;
		if ( shift )
			for ( unsigned char i = 0; i < 3; ++i )
				vertices[vertexCount+i].v += 0.5f;
	}

	//atlas is 4x4
	float border = 1/1024.0f;
	for ( unsigned int i = 0; i < 3; i += 1 )
	{
		vertices[vertexCount+i].u *= 0.25f-(border*2);
		vertices[vertexCount+i].v *= 0.25f-(border*2);

		switch ( data )
		{
		case EB_GRASS:
			if ( dir == TOP )
			{
				vertices[vertexCount+i].u += 0.0;
				//vertices[vertexCount+i].v += 0.0;
			}
			else if ( dir == BOTTOM )
			{
				vertices[vertexCount+i].u += 0.50; //Dirt on bottom
				//vertices[vertexCount+i].v += 0.0;
			}
			else
			{
				vertices[vertexCount+i].u += 0.25;
				vertices[vertexCount+i].v += 0.25;
			}
			break;
		case EB_DIRT:
			vertices[vertexCount+i].u += 0.50;
			//vertices[vertexCount+i].v += 0.0;
			break;
		case EB_STONE:
			vertices[vertexCount+i].u += 0.25;
			//vertices[vertexCount+i].v += 0.0;
			break;
		case EB_STONEBRICK:
			vertices[vertexCount+i].u += 0.50;
			vertices[vertexCount+i].v += 0.25;
			break;
		case EB_WOOD:
			vertices[vertexCount+i].u += 0.25;
			vertices[vertexCount+i].v += 0.50;
			break;
		case EB_SAND:
			vertices[vertexCount+i].u += 0.75;
			break;
		case EB_CLAY:
			//vertices[vertexCount+i].u += 0.0;
			vertices[vertexCount+i].v += 0.25;
			break;
		case EB_WATER: // DEBUGGGGG TODO
			vertices[vertexCount+i].u += 0.75;
			vertices[vertexCount+i].v += 0.75;
			break;
		case EB_CRYSTAL:
		case EB_XPLO_CRYSTAL:
			vertices[vertexCount+i].v += 0.50;
			break;
		}

		vertices[vertexCount+i].u += border;
		vertices[vertexCount+i].v += border;
	}
}

// the generation starts at bottom back left.
// goes towards right, forward, then up
bool CBoobMesh::GetBlockValue ( CBoob * pBoob, char index16, char index8, char * data, short i, int const width, char const grabIndex, unsigned char ofs, char& return_block_type, unsigned char& return_light_value )
{
	static unsigned char checkdata = EB_DIRT;


	if ( grabIndex >= 4 )
	{
		if ( ((i/width/width)%width) != width-ofs )
		{
			//checkdata = data[i+(width*width*ofs)];
			i += (width*width*ofs);
		}
		else
		{
			if ( ((index8/4)%2) != 1 )
			{
				//checkdata = pBoob->data[index16].data[index8+4].data[i-(width*width*(width-ofs))];
				index8 += 4;
				i -= (width*width*(width-ofs));

				data = (char*)pBoob->data[index16].data[index8].data;
			}
			else if ( ((index16/4)%2) != 1 )
			{
				//checkdata = pBoob->data[index16+4].data[index8-4].data[i-(width*width*(width-ofs))];
				index16 += 4;
				index8 -= 4;
				i -= (width*width*(width-ofs));

				data = (char*)pBoob->data[index16].data[index8].data;
			}
			else if ( pBoob->top != NULL )
			{
				//checkdata = pBoob->top->data[index16-4].data[index8-4].data[i-(width*width*(width-ofs))];
				pBoob = pBoob->top;
				index16 -= 4;
				index8 -= 4;
				i -= (width*width*(width-ofs));

				data = (char*)pBoob->data[index16].data[index8].data;
			}
			else
			{
				return false;
			}
		}
	}
	switch ( grabIndex%4 )
	{
	case 0: //current
		{
			checkdata = data[i];
			return_light_value = pBoob->data[index16].data[index8].light[i];
		}
		break;
	case 1: //front corner
		{
			if ( i%width < width-ofs )
			{
				checkdata = data[i+ofs];

				return_light_value = pBoob->data[index16].data[index8].light[i+ofs];
			}
			else
			{
				if ( index8%2 != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+1].data[i-(width-ofs)];

					return_light_value = pBoob->data[index16].data[index8+1].light[i-(width-ofs)];
				}
				else if ( index16%2 != 1 )
				{
					checkdata = pBoob->data[index16+1].data[index8-1].data[i-(width-ofs)];

					return_light_value = pBoob->data[index16+1].data[index8-1].light[i-(width-ofs)];
				}
				else if ( pBoob->front != NULL )
				{
					checkdata = pBoob->front->data[index16-1].data[index8-1].data[i-(width-ofs)];

					return_light_value = pBoob->front->data[index16-1].data[index8-1].light[i-(width-ofs)];
				}
				else
				{
					return false;
				}
			}
		}
		break;
	case 2: // far corner
		{
			if ( i%width < width-ofs )
			{
				i += ofs;
			}
			else
			{
				if ( index8%2 != 1 )
				{
					index8 += 1;
					i -= (width-ofs);

					data = (char*)pBoob->data[index16].data[index8].data;
				}
				else if ( index16%2 != 1 )
				{
					index16 += 1;
					index8 -= 1;
					i -= (width-ofs);

					data = (char*)pBoob->data[index16].data[index8].data;
				}
				else if ( pBoob->front != NULL )
				{
					pBoob = pBoob->front;
					index16 -= 1;
					index8 -= 1;
					i -= (width-ofs);

					data = (char*)pBoob->data[index16].data[index8].data;
				}
				else
				{
					return false;
				}
			}
		}
	case 3: // right corner
		{
			if ( ((i/width)%width) < width-ofs )
			{
				checkdata = data[i+(width*ofs)];

				return_light_value = pBoob->data[index16].data[index8].light[i+(width*ofs)];
			}
			else
			{
				if ( ((index8/2)%2) != 1 )
				{
					checkdata = pBoob->data[index16].data[index8+2].data[i-(width*(width-ofs))];

					return_light_value = pBoob->data[index16].data[index8+2].light[i-(width*(width-ofs))];
				}
				else if ( ((index16/2)%2) != 1 )
				{
					checkdata = pBoob->data[index16+2].data[index8-2].data[i-(width*(width-ofs))];

					return_light_value = pBoob->data[index16+2].data[index8-2].light[i-(width*(width-ofs))];
				}
				else if ( pBoob->left != NULL )
				{
					checkdata = pBoob->left->data[index16-2].data[index8-2].data[i-(width*(width-ofs))];

					return_light_value = pBoob->left->data[index16-2].data[index8-2].light[i-(width*(width-ofs))];
				}
				else
				{
					return false;
				}
			}
		}
		break;
	}
	//return (checkdata==0);
	//return_block_type = checkdata;
	//return ((checkdata!=EB_NONE)&&(checkdata!=EB_WATER));

	return_block_type = checkdata;
	return true;
}


*/
//