// Segmented files are text-based files of the following format:
//  <<begin,name<{name here}>,
//    (binary data here as characters)
//  -end>>
// Because of this structure, data can be added easily.

// (C) 2012-2013 Joshua Boren.

#ifndef _C_SEGMENTED_FILE_H_
#define _C_SEGMENTED_FILE_H_

// File Includes
#include "core/types/types.h"
#include "core/containers/arstring.h"

#include <algorithm>
#include <vector>
#include <string>
using std::string;

// Class Definition
class CSegmentedFile
{
public:

	// Constructor and Destructor
	CSegmentedFile ( const std::string& sInFilename );
	~CSegmentedFile ( void );

	// ==Data Reading==
	// Reads in the new data
	bool ReadData ( void );
private:
	void CheckExists ( void );

	bool FindSegmentStart ( FILE* );
	string sCurrentData;

	//void GetSegmentData ( FILE* );
	//bool AtSegmentEnd ( void );

public:
	// ==Data Accessors==
	// Get the desired string stream
	//  Returns false if the section can't be found
	bool GetSectionData ( const std::string& sSection, string& sOutString );

	// ==Data Writing==
	// Writes out the current data
	bool WriteData ( void );
	// Writes a new sector with the target name and containing the target data
	void WriteSector ( string& name, string& data, bool bAppend=false );
	// 

private:
	string					sFilename;
	bool					bValidFile;
	FILE*					fp_output;
	//vector<string*>			vpsTitleList;
	//vector<stringstream*>	vpssDataList;
	//vector<string*>			vpsDataList;
	std::vector<arstring<256>>	vsSectorNames;
	std::vector<int32_t>		viSectorPositions;
};


typedef CSegmentedFile CSegFile;

#endif