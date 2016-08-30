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
#include "core/system/io/CBufferIO.h"

#include <algorithm>
#include <vector>
#include <string>

// Class Definition
class CSegmentedFile
{
public:

	// Constructor and Destructor
	CSegmentedFile ( const std::string& sInFilename );
	~CSegmentedFile ( void );

	// ==Data Reading==
	// Reads in the new data
	// Returns false if the data cannot be read
	bool ReadData ( void );
private:
	void CheckExists ( void );

	bool FindSegmentStart ( FILE* );
	std::string sCurrentData;

	//void GetSegmentData ( FILE* );
	//bool AtSegmentEnd ( void );

public:
	// ==Data Accessors==
	
	// Get the desired string stream
	// Returns false if the section can't be found
	bool GetSectionData ( const std::string& sSection, std::string& sOutString );

	// Returns a stream to the section.
	// Returns invalid CBufferIO object if section cannot be found.
	CBufferIO GetSectionStream ( const std::string& sSection, const size_t iMaxSize = 0 );

	// ==Data Writing==

	// Writes out the current data
	bool WriteData ( void );

	// Writes a new sector with the target name and containing the target data
	// Will not replace any existing data.
	void WriteSector ( std::string& name, std::string& data );

private:
	std::string					sFilename;
	// Can the file be opened and written to?
	bool						validFile;
	// Does the file already exist?
	bool						fileExists;
	FILE*						fp_output;
	
	std::vector<arstring<256>>	vsSectorNames;
	std::vector<int32_t>		viSectorPositions;
};


typedef CSegmentedFile CSegFile;

#endif