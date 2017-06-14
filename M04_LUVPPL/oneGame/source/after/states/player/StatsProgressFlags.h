// StatsProgressFlags
// Stores information about current research, as well as miscelleneous stats


#ifndef _STATS_PROGRESS_FLAGS_H_
#define _STATS_PROGRESS_FLAGS_H_

#include "core/types/types.h"

class StatsProgressFlags ( void )
{
public:
	// Research flags
	char	faunaFlags	[2048];
	char	floraFlags	[2048];
	char	itemFlags	[2048];
	char	blockFlags	[1024];
	char	bookFlags	[2048];

	// Quest flags
	//char	questFlags	[2048];
	// Quest state:
	//	0 undiscovered
	//	1 discovered, not taken
	//	2 discovered, in progress
	//	3 completed
	//char	questState	[2048]; // man an fpga would be great for this class

	// Record info
	double		fTimePlayed;
	double		fFeetFallen;
	double		fFeetTravelled;
	uint32_t	blocksDestroyed;
};

#endif//_STATS_PROGRESS_FLAGS_H_