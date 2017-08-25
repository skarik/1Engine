
#include "core-ext/system/io/Resources.h"
#include "core-ext/system/io/FileUtils.h"

bool IO::ModelExists ( const std::string& n_filename )
{
	// First build the filename base string
	std::string mfilename = n_filename;
	size_t pos = mfilename.find_last_of( "." );
	if ( (pos > 4) && pos != std::string::npos ) {
		mfilename.assign( mfilename.begin(), mfilename.begin() + pos );
	}

	// Now check both path types
	std::string results [2];
	results[0] = core::Resources::PathTo( mfilename + ".PAD" );
	results[1] = core::Resources::PathTo( mfilename + ".FBX" );

	// Check file exists by simple open for read
	return FileExists( results[0] ) || FileExists( results[1] );
}