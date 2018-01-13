#include "osf.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

io::OSFReader::OSFReader ( FILE* file )
	: m_file(file), m_currentline(0), m_currentchar(0), m_level(0)
{
	
}
io::OSFReader::~OSFReader ( void )
{
}

bool io::OSFReader::GetNext ( OSFEntryInfo& nextEntry, char* output_value, const size_t output_value_len )
{
	int line_length;

	nextEntry.type = kOSFEntryTypeUnknown;
	nextEntry.name[0] = 0;
	nextEntry.value[0] = 0;

	nextEntry.level = m_level;

	while ( true )
	{
		// Load in the next line
		if ( !fgets( m_linebuffer, kLineBufferLen - 1, m_file ) ) {
			// Hit EOF, return null entry
			nextEntry.type = kOSFEntryTypeEoF;
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

		// Check for a comment.
		// Comments can have two forms:
		//	`// this is a comment`
		//	`# this is a comment`
		// For the latter, the separating characteristic is the space after the #.
		if ( i < line_length - 1 )
		{
			if ( m_linebuffer[i] == '/' && m_linebuffer[i+1] == '/' ) {
				continue; // Nothing on this line
			}
			if ( m_linebuffer[i] == '#' &&( m_linebuffer[i+1] == ' ' || isspace(m_linebuffer[i+1]) )) {
				continue; // Nothing on this line
			}
		}

		// Check for an invalid inline entry begin, so we can skip it.
		// Due to the whitespace reliant behavior of OSF, we don't want to support inline begins.
		if ( m_linebuffer[i] == '{' )
		{
			// Read in until found entry end
			int val;
			do
			{
				val = fgetc( m_file );
			}
			while ( val != '}' );
			continue; // And skip (skip areas with no designator)
		}
		// Check for an entry-end naively. 
		// This ruins support for 1-liners, but why support them if you rely on whitespace?
		if ( m_linebuffer[i] == '}' )
		{
			// Go down a level
			m_level -= 1;
			// Hit end, return end entry
			nextEntry.type = kOSFEntryTypeEnd;
			//return nextEntry;
			return true;
		}

		// Pull out the name of the entry:
		int fins = i;
		{
			// Find the next space
			while ( fins < line_length && !isspace(m_linebuffer[fins]) ) ++fins;

			// Copy the key to the entry
			memcpy( nextEntry.name, &(m_linebuffer[i]), fins-i );
			nextEntry.name[fins-i] = 0;

			// Set the key as valid!
			nextEntry.type = kOSFEntryTypeNormal;

			// But also check if it's a marker or macro!
			// Marker/macros can have two forms:
			//	`#Marker`
			//	`$Marker`
			// So we check both types.
			if ( nextEntry.name[0] == '#' || nextEntry.name[0] == '$' )
			{
				nextEntry.nextchar = ftell( m_file );
				nextEntry.type = kOSFEntryTypeMarker;
			}
		}

		// Move the fins cursor to next non-space
		while ( fins < line_length && isspace(m_linebuffer[fins]) ) ++fins;

		// If there's no value, set an empty value
		if ( fins == line_length )
		{
			nextEntry.value[0] = 0;
		}
		else
		{
			// Now, read in the rest of the string to the endline to the buffer
			memcpy( &(m_linebuffer[0]), &(m_linebuffer[fins]), line_length-fins );
			m_linebuffer[line_length-fins-1] = 0;

			// Parse out comments from the end of the entry
			bool comments_enabled = true;
			for ( i = 0; i < line_length-fins-2; ++i )
			{	
				// Check comments are enabled (quotes will toggle state):
				if ( m_linebuffer[i] == '"' ) {
					comments_enabled = !comments_enabled;
				}
				if (!comments_enabled) continue;
				// Check for both styles of comments:
				if ( m_linebuffer[i] == '/' && m_linebuffer[i+1] == '/' ) {
					m_linebuffer[i] = 0;
					break;
				}
				if ( m_linebuffer[i] == '#' &&( m_linebuffer[i+1] == ' ' || isspace(m_linebuffer[i+1]) )) {
					m_linebuffer[i] = 0;
					break;
				}
			}

			// If there are quotes, remove them
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
			size_t finalLength = strlen(nextEntry.value);
			if ( finalLength > 0 && isspace(nextEntry.value[finalLength-1]) ) {
				nextEntry.value[finalLength-1] = 0;
			}
		}

		// If it's a marker, it cannot be an object nor source, so we return true if that's the case here
		if (nextEntry.type == kOSFEntryTypeMarker)
		{
			return true;
		}
		
		// Now, check if the value expects source code node:
		bool expects_source_code = false;
		if (   !strncmp(nextEntry.value, "$code", 6)
			|| !strncmp(nextEntry.value, "$lua", 5)
			|| !strncmp(nextEntry.value, "$js", 4)
			|| !strncmp(nextEntry.value, "$c", 3))
		{
			expects_source_code = true;
		}

		// Object detection code
		if ( !expects_source_code )
		{
			// Save the current position
			nextEntry.nextchar = ftell(m_file);
			// Check the next line for a {
			if ( !fgets( m_linebuffer, kLineBufferLen - 1, m_file ) )
			{
				// Hit EOF, return null entry
				nextEntry.type = kOSFEntryTypeEnd;
				return true;
			}
			line_length = (signed)strlen( m_linebuffer );
			if ( line_length <= 0 ) continue;
			// Check the line for a '{'
			for ( i = 0; i < line_length; ++i )
			{
				if ( m_linebuffer[i] == '{' )
				{
					nextEntry.type = kOSFEntryTypeObject; // Found it, return that it's an object
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
					return true; // Definitely an object!
				}
			}
			// If here, then it's not an object, so go back to previous position
			fseek( m_file, nextEntry.nextchar, SEEK_SET );
		}
		else
		{
			// Save the current position
			nextEntry.nextchar = ftell(m_file);
			// Check the next line for a {
			if ( !fgets( m_linebuffer, kLineBufferLen - 1, m_file ) )
			{
				// Hit EOF, return null entry
				nextEntry.type = kOSFEntryTypeEnd;
				return true;
			}
			line_length = (signed)strlen( m_linebuffer );
			if ( line_length <= 0 ) continue;
			// Check the line for a '{'
			for ( i = 0; i < line_length; ++i )
			{
				if ( m_linebuffer[i] == '{' )
				{
					nextEntry.type = kOSFEntryTypeSource; // Found it, return that it's source code.
					nextEntry.level = m_level;

					// And instead go to the next entry
					int os_level = 0;
					size_t os_length = 0;
					bool continueRead = true;
					char os_input = 0;
					do
					{
						os_input = fgetc( m_file );

						// Keep track of the scope level
						if (os_input == '{') ++os_level;
						else if (os_input == '}') --os_level;
						// Check for end of code
						if (os_input < 0) {
							continueRead = false;
						}
						// Add input to the string
						if ( os_input && continueRead && output_value ) {
							output_value[os_length] = os_input;
							os_length += 1;
						}
					}
					while ( continueRead && os_length < output_value_len - 1 );

					// Set last character as null character
					if ( output_value ) {
						output_value[os_length] = 0;
					}

					// Save the current position
					nextEntry.nextchar = ftell(m_file);

					return true; // Definitely an object!
				}
			}
			// If here, then it's not an object, so go back to previous position
			fseek( m_file, nextEntry.nextchar, SEEK_SET );
		}

		// If we're somehow here...I don't know! It's valid, somehow!
		return true;
	}
	return false;
}
bool io::OSFReader::GoInto ( const OSFEntryInfo& entry_info )
{
	fseek( m_file, entry_info.nextchar, SEEK_SET );
	m_level = entry_info.level+1;

	return true;
}
bool io::OSFReader::GoToMarker ( const OSFEntryInfo& entry_info )
{
	fseek( m_file, entry_info.nextchar, SEEK_SET );
	m_level = entry_info.level;

	return true;
}

bool io::OSFReader::SearchToMarker ( const char* name )
{
	fseek( m_file, 0, SEEK_SET );
	OSFEntryInfo entry;
	do 
	{
		GetNext( entry, NULL, 0 );
		if ( (entry.type == kOSFEntryTypeMarker) && (strcmp( entry.name, name ) == 0) ) {
			return true;
		}
	} while ( entry.type != kOSFEntryTypeEoF );
	return false;
}
