// Interface for town generation
//
// Different town types to generation must implement the Generate() function.
// All variables visible to child classes may and should use the variables as needed for generation.
//

#ifndef _INTERFACE_PATTERN_GENERATION_H_
#define _INTERFACE_PATTERN_GENERATION_H_

#include "core/math/vect3d_template.h"
#include "after/types/world/Towns.h"
#include "after/types/world/Regions.h"
#include "CPatternController.h"

class CVoxelTerrain;

namespace Terrain
{
	class SidebufferVolumeEditor;
	//struct regionproperties_t;

	class IPatternGeneration
	{
	public:
		explicit IPatternGeneration ( void ) 
			: mseed(0), genTown(NULL), genTownEntry(NULL), genRegion(NULL)
		{
			;
		}
	public:
		// System grabber
		CVoxelTerrain*			terrain;		// Pointer to associated terrain
		SidebufferVolumeEditor*	generator;		// Pointer to associated mesh generation (used for regenerating towns)

		// Area information
		uint8_t				areaBiome;
		uint8_t				areaTerra;
		Real_32				areaElevation;
		RangeVector			areaPosition;
		
		// Town entry
		World::patternData_t*		genTown;
		World::patternQuickInfo_t*	genTownEntry;
		World::regionproperties_t*	genRegion;

		//CTownManager*		floorplans;
		//CFloorplanManager*	floorplans;

		// Actual interface that requires implementation
		virtual void		Generate ( void )=0;

	protected:
		Vector3d_d	townCenterPosition;
	private:
		int			mseed;
	protected:
		// Utility function for random chance based on noise
		bool				Chance ( const ftype chance, const int seed=-1 );
		// Gets a random number from -0.5 to 0.5
		ftype				RandomNumber ( void );
	};
};

#endif//_INTERFACE_PATTERN_GENERATION_H_