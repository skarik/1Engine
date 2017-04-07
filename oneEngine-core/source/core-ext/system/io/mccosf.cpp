
#include "mccosf.h"

// Include default system IO
#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"

#include <exception>


COSF_Loader::COSF_Loader ( FILE* file )
	: m_file(file), m_currentline(0), m_currentchar(0), m_level(0)
{
	
}

COSF_Loader::~COSF_Loader ( void )
{
	//fclose( m_file );
}
bool COSF_Loader::GetNext ( mccOSF_entry_info_t& nextEntry, char* output_value )
{
	//mccOSF_entry_info_t nextEntry;
	int line_length;

	nextEntry.name[0] = 0;
	nextEntry.value[0] = 0;

	nextEntry.level = m_level;

	while ( true )
	{
		// Load in the next line
		if ( !fgets( m_linebuffer, 255, m_file ) ) {
			// Hit EOF, return null entry
			nextEntry.type = MCCOSF_ENTRY_EOF;
			//return nextEntry;
			return true;
		}
		line_length = (signed)strlen( m_linebuffer );
		if ( line_length <= 0 ) {
			continue; // Line is too short (or too long), skip it
		}

		// Parse the line for the first command
		// look for first space
		int i = 0;
		while ( i < line_length && isspace(m_linebuffer[i]) ) ++i;
		if ( i == line_length ) {
			continue; // Nothing on this line
		}

		// Check for a comment
		if ( line_length > 1 ) {
			if ( m_linebuffer[i] == '/' && m_linebuffer[i+1] == '/' ) {
				continue; // Nothing on this line
			}
			if (( m_linebuffer[0] == '#' )&&( m_linebuffer[1] == 0 || isspace(m_linebuffer[1]) )) {
				continue; // Nothing on this line either
			}
		}

		// Check for an entry begin
		if ( m_linebuffer[i] == '{' ) {
			// Read in until found entry end
			int val;
			do
			{
				val = fgetc( m_file );
			}
			while ( val != '}' );
			continue; // And skip (skip areas with no designator)
		}
		// Check for an entry end
		if ( m_linebuffer[i] == '}' ) {
			// Go down a level
			m_level -= 1;
			// Hit end, return end entry
			nextEntry.type = MCCOSF_ENTRY_END;
			//return nextEntry;
			return true;
		}

		// Find the next space
		int fins = i;
		while ( fins < line_length && !isspace(m_linebuffer[fins]) ) ++fins;

		// Copy the key to the entry
		memcpy( nextEntry.name, &(m_linebuffer[i]), fins-i );
		nextEntry.name[fins-i] = 0;

		// Set the key as valid
		nextEntry.type = MCCOSF_ENTRY_NORMAL;

		// Move the fins cursor to next non-space
		while ( fins < line_length && isspace(m_linebuffer[fins]) ) ++fins;

		// If there's no value, set string as such
		if ( fins == line_length ) {
			nextEntry.value[0] = 0;
			// If it's a macro, then it's not an object
			if ( nextEntry.name[0] == '#' || nextEntry.name[0] == '$' ) {
				nextEntry.nextchar = ftell( m_file );
				nextEntry.type = MCCOSF_ENTRY_MARKER;
				//return nextEntry;
				return true;
			}
		}
		else {
			// Now, read in the rest of the string to the endline to the buffer
			memcpy( &(m_linebuffer[0]), &(m_linebuffer[fins]), line_length-fins );
			m_linebuffer[line_length-fins-1] = 0;

			// Now, parse out comments from the end of the entry
			for ( i = 0; i < line_length-fins-2; ++i ) {
				if ( m_linebuffer[i] == '/' && m_linebuffer[i+1] == '/' ) {
					m_linebuffer[i] = 0;
				}
			}

			// Now, if there are quotes, remove them
			if ( m_linebuffer[0] == '"' ) {
				memcpy( m_linebuffer, &(m_linebuffer[1]), line_length-fins-2 );
				for ( i = 1; i < line_length-fins-2; ++i ) {
					if ( m_linebuffer[i] == '"' ) {
						m_linebuffer[i] = 0;
					}
				}
			}

			// Now copy string over to smaller buffer
			memcpy( nextEntry.value, m_linebuffer, ( line_length-fins < 128 ? line_length-fins : 128 ) );
			int finalLength = (signed)strlen(nextEntry.value);
			if ( (finalLength > 0) && (nextEntry.value[finalLength-1] == '\r') ) {
				nextEntry.value[finalLength-1] = 0;
			}
		}

		// Object detection code
		{
			// Save the current position
			nextEntry.nextchar = ftell(m_file);
			// Check the next line for a {
			if ( !fgets( m_linebuffer, 255, m_file ) ) {
				// Hit EOF, return null entry
				nextEntry.type = MCCOSF_ENTRY_END;
				//return nextEntry;
				return true;
			}
			line_length = (signed)strlen( m_linebuffer );
			if ( line_length <= 0 ) continue;
			// Check the line for a '{'
			for ( i = 0; i < line_length; ++i )
			{
				if ( m_linebuffer[i] == '{' ) {
					nextEntry.type = MCCOSF_ENTRY_OBJECT; // Found it, return that it's an object
					nextEntry.level = m_level;
					// Save the current position
					nextEntry.nextchar = ftell(m_file);
					// And instead go to the next entry
					{
						// Read in until found entry end
						int val;
						do
						{
							val = fgetc( m_file );
						}
						while ( val != '}' );
					}
					//return nextEntry; // Return object entry
					return true;
				}
				else if ( i+2 < line_length && strncmp( "(!{", m_linebuffer+i, 3 ) == 0 )
				{
					if ( output_value == NULL ) {
						//throw std::exception(); // You didn't pass in anything for the second argument.
					}
					// It's a massive data object.
					int os_length = 0;
					bool continueRead = true;
					char inputBuffer [3] = {0,0,0};
					// Read in until the "}!)" information stop
					do
					{
						inputBuffer[0] = inputBuffer[1];
						inputBuffer[1] = inputBuffer[2];
						inputBuffer[2] = fgetc( m_file );

						// Read int 
						if ( strncmp( "}!)", inputBuffer, 3 ) == 0 ) {
							continueRead = false;
						}
						if ( inputBuffer[0] && continueRead && output_value ) {
							output_value[os_length] = inputBuffer[0];
							os_length += 1;
						}
					}
					while ( continueRead );
					// Set last character as null character
					if ( output_value ) {
						output_value[os_length] = 0;
					}
					// Set value to NULL
					nextEntry.value[0] = 0;
					return true;
				}
				else if ( i+6 < line_length && strncmp( "JSBEGIN", m_linebuffer+i, 7 ) == 0 )
				{
					if ( output_value == NULL ) {
						//throw std::exception(); // You didn't pass in anything for the second argument.
					}
					// It's a massive data object.
					int os_length = 0;
					bool continueRead = true;
					char inputBuffer [5] = {0,0,0,0,0};
					// Read in until the "}!)" information stop
					do
					{
						inputBuffer[0] = inputBuffer[1];
						inputBuffer[1] = inputBuffer[2];
						inputBuffer[2] = inputBuffer[3];
						inputBuffer[3] = inputBuffer[4];
						inputBuffer[4] = fgetc( m_file );

						// Read int 
						if ( strncmp( "JSEND", inputBuffer, 5 ) == 0 ) {
							continueRead = false;
						}
						if ( inputBuffer[0] && continueRead && output_value ) {
							output_value[os_length] = inputBuffer[0];
							os_length += 1;
						}
					}
					while ( continueRead );
					// Set last character as null character
					if ( output_value ) {
						output_value[os_length] = 0;
					}
					// Set value to NULL
					nextEntry.value[0] = 0;
					return true;
				}
			}
			// If here, then it's not an entry, so go back to previous position
			fseek( m_file, nextEntry.nextchar, SEEK_SET );
		}

		// Return the entry we found
		//return nextEntry;
		return true;
	}
	return false;
}
bool COSF_Loader::GoInto ( const mccOSF_entry_info_t& entry_info )
{
	fseek( m_file, entry_info.nextchar, SEEK_SET );
	m_level = entry_info.level+1;

	return true;
}
bool COSF_Loader::GoToMarker ( const mccOSF_entry_info_t& entry_info )
{
	fseek( m_file, entry_info.nextchar, SEEK_SET );
	m_level = entry_info.level;

	return true;
}

bool COSF_Loader::SearchToMarker ( const char* name )
{
	fseek( m_file, 0, SEEK_SET );
	mccOSF_entry_info_t entry;
	do 
	{
		GetNext( entry );
		if ( (entry.type == MCCOSF_ENTRY_MARKER) && (strcmp( entry.name, name ) == 0) ) {
			return true;
		}
	} while ( entry.type != MCCOSF_ENTRY_EOF );
	return false;
}
