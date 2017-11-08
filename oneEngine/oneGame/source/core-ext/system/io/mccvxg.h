// =============================================================
//					  MCC VXG File Interface
// Interface for reading and writing mcc.VXG files
// Implementation (C) 2013 by EpicHouse Studios and Joshua Boren
// =============================================================
#ifndef CORE_VXG_IO_H_
#define CORE_VXG_IO_H_

#include "core/types/types.h"
#include "stdio.h"
#include "stdlib.h"

// Definitions
#define MCCVXG_WIDTH	0
#define MCCVXG_XSIZE	0
#define MCCVXG_HEIGHT	2
#define MCCVXG_YSIZE	1
#define MCCVXG_ZSIZE	2
#define MCCVXG_DEPTH	1

// MCC VXG Structs
typedef struct mccVXG_file_info
{
	FILE*	file;
	int		xsize;
	int		ysize;
	int		zsize;
	int		bitdepth;
	int		locdata;
	int		locimagesz;
	void*	buffer;
	int		vermajor;
	int		verminor;
	int		datasize;
} mccVXG_file_info_t;
typedef mccVXG_file_info_t* mccVXG_file_infop;

// IO general
mccVXG_file_infop	mccvxg_begin_io ( FILE* in_file );
int		mccvxg_end_io ( mccVXG_file_infop file_info );

// Input functions
int		mccvxg_read_info ( mccVXG_file_infop file_info );
int		mccvxg_get_bit_depth ( mccVXG_file_infop file_info );
int		mccvxg_get_image_size ( mccVXG_file_infop file_info, int dimtype );
int		mccvxg_read_image ( mccVXG_file_infop file_info, void* outimagedata );

// Output functions
void	mccvxg_write_header ( mccVXG_file_infop file_info );
void	mccvxg_write_image_size ( mccVXG_file_infop file_info, int xsize, int ysize, int zsize );
void	mccvxg_write_bit_depth ( mccVXG_file_infop file_info, int bitdepth );
void	mccvxg_write_info ( mccVXG_file_infop file_info );
void	mccvxg_write_image ( mccVXG_file_infop file_info, void* inimagedata );

#endif//CORE_VXG_IO_H_