
#include <stdio.h>
#include "FileUtils.h"
#include "core/exceptions.h"

bool IO::FileExists ( const std::string& n_filename )
{
	return IO::FileExists( n_filename.c_str() );
}

bool IO::FileExists ( const char* n_filename )
{
	// Check file exists by simple open for read
	FILE* fp = fopen( n_filename, "r" );
	if ( fp != NULL ) {
		fclose( fp );
		return true;
	}
	else {
		return false;
	}
}

std::string IO::FilenameStandardize ( const std::string& n_filename )
{
	std::string t_resultName = n_filename;
	for ( unsigned int i = 0; i < t_resultName.length(); ++i ) {
		if ( t_resultName[i] == '\\' ) {
			t_resultName[i] = '/';
		}
		else {
			t_resultName[i] = tolower(t_resultName[i]);
		}
	}
	return t_resultName;
}

bool IO::ClearFile ( const char* n_filename )
{
	FILE* fp = fopen( n_filename, "wb" );
	fclose( fp );
	return true;
}

bool IO::AppendFile ( const char* n_targetfile, const char* n_sourcefile )
{
	FILE* fp_source = fopen( n_sourcefile, "rb" );
	if ( fp_source == NULL ) {
		return false;
	}
	FILE* fp_target = fopen( n_targetfile, "ab" );
	if ( fp_target == NULL ) {
		fclose( fp_source );
		return false;
	}

	char buf [256];
	size_t readcount;

	do
	{
		readcount = fread( buf, 1, 256, fp_source );
		if ( readcount > 0 ) {
			fwrite( buf, 1, readcount, fp_target );
		}
	} while ( readcount > 0 );

	fclose( fp_target );
	fclose( fp_source );

	return true;
}

bool IO::AppendStringToFile ( const char* n_targetfile, const char* n_sourcestring )
{
	FILE* fp_target = fopen( n_targetfile, "ab" );
	fwrite( n_sourcestring, 1, strlen(n_sourcestring), fp_target );
	fclose( fp_target );
	return true;
}

bool IO::ReadLine ( FILE* n_file, char* n_buffer )
{
	int spot = 0;
	char val;
	do
	{
		val = getc(n_file);
		n_buffer[spot++] = val;
		if ( val == '\n' ) {
			n_buffer[spot-1] = 0;
		}
	}
	while ( val != '\n' && !feof(n_file) );
	if ( feof(n_file) ) {
		return false;
	}
	return true;
}
