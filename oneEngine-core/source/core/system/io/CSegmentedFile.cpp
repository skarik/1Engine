
// Includes
#include "CSegmentedFile.h"

#include <cstdio>
#include <cstdlib>

using std::string;

// == Constructor ==
CSegmentedFile::CSegmentedFile ( const string& sInFilename )
	: validFile(false), fileExists(false), fp_output(NULL)
{
	sFilename = sInFilename;
	// Check if the file exists
	CheckExists();
	// Check if can write to the file
	if (!fileExists)
	{
		fp_output = fopen( sFilename.c_str(), "w" );
		if ( fp_output != NULL )
		{
			validFile = true;
			fclose( fp_output );
			remove( sFilename.c_str() );
		}
		fp_output = NULL;
	}
}

// == Destructor ==
CSegmentedFile::~CSegmentedFile ( void )
{
	if ( fp_output )
	{
		fclose( fp_output );
		fp_output = NULL;
	}
}

// == File Reading ==
void CSegmentedFile::CheckExists ( void )
{
	FILE* fp;
	fp = fopen( sFilename.c_str(), "r" );
	if ( fp != NULL )
	{
		fclose( fp );
		validFile = true;
		fileExists = true;
	}
}

bool CSegmentedFile::ReadData ( void )
{
	/*if ( bValidFile )
	{
		// Open file
		//ifstream ifile( sFilename.c_str(), ios::binary );
		//ifile.
		FILE* ifile = fopen( sFilename.c_str(), "rb" );

		if ( ifile == NULL )
		{
			bValidFile = false;
			return false;
		}

		//while ( !ifile.eof() )
		while ( !feof( ifile ) )
		{
			if ( FindSegmentStart( ifile ) )
				GetSegmentData( ifile );
		}

		fclose( ifile );
	}
	return bValidFile;*/
	if ( validFile )
	{
		// Open file
		FILE* fp = fopen( sFilename.c_str(), "r+b" );
		if ( fp == NULL )
		{
			fileExists = false;
			validFile = false;
			return false;
		}
		validFile = true;

		// Loop through the entire file, looking for the positions of data
		while ( !feof( fp ) ) {
			FindSegmentStart( fp );
		}

		// Close file
		fclose( fp );
	}
	return validFile;
}

//==Locate Start and Read Infos==
bool CSegmentedFile::FindSegmentStart ( FILE* fp )
{
	bool added = false;
	unsigned char temp;

	sCurrentData = "";

	const char matchBuffer [8] = "<<begin";
	while ( true )
	{
		// Exit if at the end of file
		if ( feof( fp ) ) {
			return false;
		}
		// Otherwise read shit
		char tempBuffer [1024];
		uint readAmount = fread( tempBuffer, 1, 1024, fp );
		uint matchpoint = 0;
		uint offset = 0;
		for ( uint i = 0; i < readAmount; ++i )
		{
			if ( tempBuffer[i] == matchBuffer[matchpoint] )
			{
				if ( i >= readAmount-(7-matchpoint) )
				{
					matchpoint = 0;
					fseek( fp, std::max<long>( ftell(fp)-8, 0 ), SEEK_SET );
					break;
				}
				else
				{
					matchpoint += 1;
					if ( matchpoint == 7 )
					{
						offset = i;
						break;
					}
				}
			}
			else {
				matchpoint = 0;
			}
		}
		// Now check match
		if ( matchpoint == 7 )
		{
			long seekpos = ftell(fp)-(readAmount-offset-1);
			//printf( "Seeking to position %d for new segment found\n", seekpos );
			fseek( fp, seekpos, SEEK_SET );
			sCurrentData = "<<begin";
			break;
		}
	}

	// Read in first character to check for keys
	fread( &temp, sizeof( unsigned char ), 1, fp );
	if ( temp == ',' )
	{
		// Read in the key name
		sCurrentData = "";
		while ( temp != '<' )
		{
			fread( &temp, sizeof( unsigned char ), 1, fp );
			sCurrentData += temp;
		}
		sCurrentData = sCurrentData.substr( 0,sCurrentData.length()-1 );

		// If the key is 'name'
		if ( sCurrentData == "name" )
		{
			// We want to get the name
			sCurrentData = "";
			while ( temp != '>' )
			{
				//ifile >> temp;
				fread( &temp, sizeof( unsigned char ), 1, fp );
				sCurrentData += temp;
			}
			// Remove the end carat
			sCurrentData = sCurrentData.substr( 0,sCurrentData.length()-1 );

			// Add the new name to a list
			vsSectorNames.push_back( arstring<256>( sCurrentData.c_str() ) );
			added = true;
		}
		else
		{
			// If it's an unrecognized key, just ignore it and continue
			//  (though, this surely causes more problems than it causes)
			while ( temp != '>' )
			{
				fread( &temp, sizeof( unsigned char ), 1, fp );
				sCurrentData += temp;
			}
		}
	}
	else { // Section ALWAYS has a key, so if no key, then invalid format
		return false;
	}

	// Read in final comma
	fread( &temp, sizeof( unsigned char ), 1, fp );

	// Save current cursor position
	if ( added )
	{
		viSectorPositions.push_back( ftell( fp ) );
	}

	return true;
}
//==Read in Segment Data==
//void CSegmentedFile::GetSegmentData ( FILE* ifile )
//{
//	unsigned char temp;
//	uint32_t iDataSize;
//	sCurrentData = "";
//	char* pTempString;
//	// Read data until we see the closing thing
//	/*while ( !AtSegmentEnd() )
//	{
//		ifile >> temp;			// TODO: this line is slow!
//		sCurrentData += temp;
//	}*/
//	// Remove the closing thing of stuff
//	//sCurrentData = sCurrentData.substr(0,sCurrentData.length()-6);
//	
//	// Read the amount of data sitting there
//	fread( &iDataSize, sizeof( uint32_t ), 1, ifile );
//
//	// Read in that data to C-string
//	pTempString = new char [iDataSize+1];
//	fread( pTempString, sizeof( char ), iDataSize, ifile );
//	pTempString[iDataSize] = 0;
//	//sCurrentData += pTempString;
//	sCurrentData = string( pTempString, iDataSize );
//	//cout << iDataSize <<  " " << sCurrentData << sCurrentData.length() << endl;
//
//	//cout << "Current Data: " << sCurrentData << endl;
//
//	// Create an 'input' stream of new data
//	//stringstream* pNewStringStream = new stringstream ( sCurrentData, stringstream::in );
//	string* pNewString = new string( pTempString, iDataSize );
//	delete [] pTempString;
//	// Add the new data to a list
//	//vpssDataList.push_back( pNewStringStream );
//	vpsDataList.push_back( pNewString );
//
//	//cout << "Data Saved: " << *pNewString << endl;
//
//	sCurrentData = "";
//	// Read data until we see the closing thing
//	while ( !AtSegmentEnd() )
//	{
//		//ifile >> temp;			// TODO: this line is slow!
//		fread( &temp, sizeof( unsigned char ), 1, ifile );
//		sCurrentData += temp;
//	}
//}
//==Check for the closing thing==
/*bool CSegmentedFile::AtSegmentEnd ( void )
{
	return sCurrentData.length()>=6 ? ( sCurrentData.substr( sCurrentData.length()-6 ) == "-end>>" ) : false;
}*/

//==Return a section of data==
bool CSegmentedFile::GetSectionData ( const string& sSection, string& sOutString )
{
	uint sectionIndex = (unsigned)-1;

	// Search for the target title
	for ( uint i = 0; i < vsSectorNames.size(); ++i )
	{
		if ( strcmp( sSection.c_str(), vsSectorNames[i].c_str() ) == 0 )
		{
			sectionIndex = i;
			break;
		}
	}
	// If didn't find it, return failure
	if ( sectionIndex == (unsigned)-1 )
	{
		return false;
	}
	else
	{
		uint32_t datasize;

		// Open file
		FILE* fp = fopen( sFilename.c_str(), "rb" );
		if ( fp == NULL )
		{
			validFile = false;
			fileExists = false;
			return false;
		}

		// Seek to the start position of the sector
		fseek( fp, viSectorPositions[sectionIndex], 0 );

		// Read in the size of the data block
		fread( (char*)&datasize, sizeof(uint32_t),1, fp );

		// Read in contents to the string
		sOutString.resize( datasize, 0 );
		fread( &sOutString[0], sizeof(char), datasize, fp );
	
		// Close file
		fclose( fp );

		// Return end
		return true;
	}
}

// Moves the stream to the section, returning the section's size. Must be deleted by the user.
// Returns 0 if the section cannot be found or section empty.
CBufferIO CSegmentedFile::GetSectionStream ( const std::string& sSection, const size_t iMaxSize )
{
	uint sectionIndex = (unsigned)-1;

	// Search for the target title
	for ( uint i = 0; i < vsSectorNames.size(); ++i )
	{
		if ( strcmp( sSection.c_str(), vsSectorNames[i].c_str() ) == 0 ) {
			sectionIndex = i;
			break;
		}
	}
	// If didn't find it, return failure
	if ( sectionIndex == (unsigned)-1 )
	{
		return CBufferIO(NULL);
	}
	else
	{
		uint32_t datasize;
		char* data;

		// Open file
		FILE* fp = fopen( sFilename.c_str(), "rb" );
		if ( fp == NULL )
		{
			validFile = false;
			fileExists = false;
			return CBufferIO(NULL);
		}

		// Seek to the start position of the sector
		fseek( fp, viSectorPositions[sectionIndex], 0 );

		// Read in the size of the data block
		fread( (char*)&datasize, sizeof(uint32_t),1, fp );
		// Set size to actually read
		if ( iMaxSize != 0 )
		{
			datasize = std::min( datasize, iMaxSize );
		}

		// Read in contents to a string
		data = new char [datasize];
		fread( data, sizeof(char), datasize, fp );

		// Close file
		fclose( fp );

		// Return buffer read
		CBufferIO buffer ( data, datasize );
		buffer.ReleaseOwnership();
		return buffer;
	}
}

// == File Writing ==
// Saves the file to the filename.
bool CSegmentedFile::WriteData ( void )
{
	if ( validFile )
	{
		// Open real output file
		FILE* ofile = fopen( sFilename.c_str(), "wb" );
		// Open temp file
		FILE* ifile = fopen( (sFilename + ".temp").c_str(), "rb" );

		// Copy the data over
		size_t count;
		char buf [256];
		while ( count = fread( buf, sizeof(char), 256, ifile ) ) {
			fwrite( buf, sizeof(char), count, ofile );
		}

		// Close files
		fclose( ofile );
		fclose( ifile );

		// Remove temp file
		remove( (sFilename + ".temp").c_str() );

		return true;
	}
	else
	{
		return false;
	}
}

//==Write Segment Data==
// Writes shit.
void CSegmentedFile::WriteSector( string& sName, string& sData )
{
	/*unsigned int iDataIndex = (unsigned) -1;

	// If we don't append it, look for a title to replace
	if ( !bAppend )
	{
		for ( iDataIndex = 0; iDataIndex < vpsTitleList.size(); iDataIndex += 1 )
		{
			if ( sName.compare( *(vpsTitleList[iDataIndex]) ) == 0 )
			{
				// Found, break loop
				break;
			}
		}
	}

	// If the index is -1 or beyond end of list, make a new sector
	if (( iDataIndex == (unsigned) -1 )||( iDataIndex == vpsTitleList.size() ))
	{
		iDataIndex = vpsTitleList.size();
		vpsTitleList.push_back( new string );
		(*vpsTitleList[iDataIndex]) = sName;
		vpsDataList.push_back( new string );
		(*vpsDataList[iDataIndex]) = "";
		(*vpsDataList[iDataIndex]).reserve( sData.length() );
	}

	// Edit the data already there.
	(*vpsDataList[iDataIndex]) = sData;
	*/

	//
	if ( validFile || fileExists )
	{
		// Open temp file
		fp_output = fopen( (sFilename + ".temp").c_str(), "ab+" );
		if ( fp_output == NULL )
		{
			validFile = false;
			return;
		}
		else
		{
			fileExists = true;
		}

		// Write the section header
		fwrite( "<<begin,", sizeof(char), 8, fp_output );
		// Write the keys. Currently, only key is name
		{
			fwrite( "name<", sizeof(char), 5, fp_output );
			fwrite( sName.c_str(), sizeof(char), sName.length(), fp_output );
			fwrite( ">,", sizeof(char), 2, fp_output );
		}
		// Write out the data
		{
			// Write the size of the data as a 32 bit integer
			uint32_t datasize;
			datasize = sData.length();
			fwrite( &datasize, sizeof(uint32_t), 1, fp_output );
				
			// Write out the actual data
			fwrite( sData.c_str(), sizeof(char), datasize, fp_output );
		}
		// Write out the ending of this section
		fwrite( "-end>>", sizeof(char), 6, fp_output );

		// Close output
		fclose( fp_output );
		fp_output = NULL;
	}
}