#include "osf.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "core/debug.h"

//===============================================================================================//

io::OSFReader::OSFReader ( FILE* file )
	: m_file(file), m_currentline(0), m_currentchar(0), m_level(0)
{}
io::OSFReader::~OSFReader ( void )
{}

template <class OSFEntryType>
bool io::OSFReader::GetNext ( OSFEntryType& nextEntry, char* output_value, const size_t output_value_len )
{
	int line_length;

	nextEntry.type = kOSFEntryTypeUnknown;
	nextEntry.name[0] = 0;
	if constexpr (std::is_same<OSFEntryInfo, OSFEntryType>::value)
		nextEntry.value[0] = 0;
	else if constexpr (std::is_same<OSFEntryInfoLarge, OSFEntryType>::value)
		nextEntry.value.clear();
	else
		static_assert(false, "Invalid OSF entry type");

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
			int currentDepth = 1;
			do
			{
				int val = fgetc( m_file );
				if (val == '{')
				{
					currentDepth++;
				}
				else if (val == '}')
				{
					currentDepth--;
				}
			}
			while (!feof(m_file) && currentDepth > 0);
			continue; // And skip (skip areas with no designator)
		}
		// Check for an entry-end naively. 
		// This ruins support for 1-liners, but why support them if you rely on whitespace? TODO: Rememdy one-liners
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
			if constexpr (std::is_same<OSFEntryInfo, OSFEntryType>::value)
				nextEntry.value[0] = 0;
			else if constexpr (std::is_same<OSFEntryInfoLarge, OSFEntryType>::value)
				nextEntry.value.clear();
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
				for ( i = 0; i < line_length-fins-2; ++i ) {
					if ( m_linebuffer[i] == '"' ) {
						m_linebuffer[i] = 0;
					}
				}
			}

			if constexpr (std::is_same<OSFEntryInfo, OSFEntryType>::value)
			{
				// Now copy string over to smaller buffer
				memcpy( nextEntry.value, m_linebuffer, ( line_length-fins < 128 ? line_length-fins : 128 ) );
				size_t finalLength = strlen(nextEntry.value);
				if ( finalLength > 0 && isspace(nextEntry.value[finalLength-1]) ) {
					nextEntry.value[finalLength-1] = 0;
				}
			}
			else if constexpr (std::is_same<OSFEntryInfoLarge, OSFEntryType>::value)
			{
				// Copy entry string remainder over
				size_t finalLength = strnlen(m_linebuffer, kLineBufferLen);
				nextEntry.value.assign(m_linebuffer, finalLength);
			}
		}

		// If it's a marker, it cannot be an object nor source, so we return true if that's the case here
		if (nextEntry.type == kOSFEntryTypeMarker)
		{
			return true;
		}
		
		// Now, check if the value expects source code node:
		bool expects_source_code = false;
		if constexpr (std::is_same<OSFEntryInfo, OSFEntryType>::value)
		{
			if (   !strncmp(nextEntry.value, "$code", 6)
				|| !strncmp(nextEntry.value, "$lua", 5)
				|| !strncmp(nextEntry.value, "$js", 4)
				|| !strncmp(nextEntry.value, "$c", 3))
			{
				expects_source_code = true;
			}
		}
		else if constexpr (std::is_same<OSFEntryInfoLarge, OSFEntryType>::value)
		{
			if (   !strncmp(nextEntry.value.c_str(), "$code", 6)
				|| !strncmp(nextEntry.value.c_str(), "$lua", 5)
				|| !strncmp(nextEntry.value.c_str(), "$js", 4)
				|| !strncmp(nextEntry.value.c_str(), "$c", 3))
			{
				expects_source_code = true;
			}
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
						int currentDepth = 1;
						do
						{
							int val = fgetc( m_file );
							if (val == '{')
							{
								currentDepth++;
							}
							else if (val == '}')
							{
								currentDepth--;
							}
						}
						while (!feof(m_file) && currentDepth > 0);
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
template
CORE_API bool io::OSFReader::GetNext<io::OSFEntryInfo> ( io::OSFEntryInfo& nextEntry, char* output_value, const size_t output_value_len );
template
CORE_API bool io::OSFReader::GetNext<io::OSFEntryInfoLarge> ( io::OSFEntryInfoLarge& nextEntry, char* output_value, const size_t output_value_len );

template <class OSFEntryType>
bool io::OSFReader::GoInto ( const OSFEntryType& entry_info )
{
	fseek( m_file, entry_info.nextchar, SEEK_SET );
	m_level = entry_info.level+1;

	return true;
}
template
CORE_API bool io::OSFReader::GoInto<io::OSFEntryInfo> ( const io::OSFEntryInfo& entry_info );
template
CORE_API bool io::OSFReader::GoInto<io::OSFEntryInfoLarge> ( const io::OSFEntryInfoLarge& entry_info );

template <class OSFEntryType>
bool io::OSFReader::GoToMarker ( const OSFEntryType& entry_info )
{
	fseek( m_file, entry_info.nextchar, SEEK_SET );
	m_level = entry_info.level;

	return true;
}
template
CORE_API bool io::OSFReader::GoToMarker<io::OSFEntryInfo> ( const io::OSFEntryInfo& entry_info );
template
CORE_API bool io::OSFReader::GoToMarker<io::OSFEntryInfoLarge> ( const io::OSFEntryInfoLarge& entry_info );

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

//===============================================================================================//

io::OSFWriter::OSFWriter ( FILE* file, eOSFWriteMode writeMode )
	: m_file(file), m_level(0), m_mode(writeMode)
{
	ARCORE_ASSERT_MSG(m_mode != kOSFWriteModePreserveComments, "PreserveComments mode is currently unsupported with OSF writing.");
}
io::OSFWriter::~OSFWriter ( void )
{}

template <class OSFEntryType>
bool io::OSFWriter::WriteEntry ( const OSFEntryType& entry, const char* output_value, const size_t output_value_len )
{
	// Validate the type of entries we can write
	if ( entry.type == kOSFEntryTypeObject
		|| entry.type == kOSFEntryTypeEnd
		|| entry.type == kOSFEntryTypeEoF
		|| entry.type == kOSFEntryTypeUnknown )
	{
		return false;
	}

	// Validate the value for kOSFEntryTypeSource entries
	if ( entry.type == kOSFEntryTypeSource )
	{
		if constexpr (std::is_same<OSFEntryInfo, OSFEntryType>::value)
		{
			if (   !strncmp(entry.value, "$code", 6)
				|| !strncmp(entry.value, "$lua", 5)
				|| !strncmp(entry.value, "$js", 4)
				|| !strncmp(entry.value, "$c", 3))
			{
				return false;
			}
		}
		else if constexpr (std::is_same<OSFEntryInfoLarge, OSFEntryType>::value)
		{
			if (   !strncmp(entry.value.c_str(), "$code", 6)
				|| !strncmp(entry.value.c_str(), "$lua", 5)
				|| !strncmp(entry.value.c_str(), "$js", 4)
				|| !strncmp(entry.value.c_str(), "$c", 3))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	size_t l_fieldw = strlen(entry.name);
	if ( l_fieldw == 0 )
	{	// Make sure the name exists. We cannot have a key-less value!
		return false;
	}

	if constexpr (std::is_same<OSFEntryInfo, OSFEntryType>::value)
	{
		size_t l_linepos = 0;
		char l_linebuffer[kLineBufferLen] = {0};

		// Add the tab characters
		for ( int i = 0; i < m_level; ++i )
		{
			l_linebuffer[l_linepos++] = '\t';
		}

		// Marker/macro objects need a leading # or $
		if ( entry.type == kOSFEntryTypeMarker ) 
		{
			if ( entry.name[0] != '#' && entry.name[0] != '$' )
			{
				l_linebuffer[l_linepos++] = '#';
			}
		}
		// Copy the name over
		for ( size_t i = 0; i < l_fieldw; ++i )
		{
			l_linebuffer[l_linepos++] = entry.name[i];
		}

		// Check for the value now:
		l_fieldw = strlen(entry.value);
		if ( l_fieldw != 0 )
		{
			// Add a space
			l_linebuffer[l_linepos++] = ' ';

			// Copy the field over
			for ( size_t i = 0; i < l_fieldw; ++i )
			{
				l_linebuffer[l_linepos++] = entry.value[i];
			}
		}
		// Add the final newlines
		l_linebuffer[l_linepos++] = '\r';
		l_linebuffer[l_linepos++] = '\n';

		// Write to file
		fwrite(l_linebuffer, 1, l_linepos, m_file);
	}
	else if constexpr (std::is_same<OSFEntryInfoLarge, OSFEntryType>::value)
	{
		std::string l_linebuffer;

		// Add the tab characters
		for ( int i = 0; i < m_level; ++i )
		{
			l_linebuffer += '\t';
		}

		// Marker/macro objects need a leading # or $
		if ( entry.type == kOSFEntryTypeMarker ) 
		{
			if ( entry.name[0] != '#' && entry.name[0] != '$' )
			{
				l_linebuffer += '#';
			}
		}
		// Copy the name over
		l_linebuffer += entry.name;

		// Check for the value now:
		if (!entry.value.empty())
		{
			// Add a space
			l_linebuffer += ' ';

			// Copy the field over
			l_linebuffer += entry.value;
		}
		// Add the final newlines
		l_linebuffer += "\r\n";

		// Write to file
		fwrite(l_linebuffer.c_str(), 1, l_linebuffer.length(), m_file);
	}
	else
	{
		static_assert(false, "Invalid OSF entry type");
	}

	// Now, if it's a source object, we need also write out the contents of output_value.
	if ( entry.type == kOSFEntryTypeSource )
	{
		size_t l_linepos = 0;
		char l_linebuffer[kLineBufferLen] = {0};

		// Opening bracket:
		l_linepos = 0;
		for ( int i = 0; i < m_level; ++i )
		{
			l_linebuffer[l_linepos++] = '\t';
		}
		l_linebuffer[l_linepos++] = '{';
		l_linebuffer[l_linepos++] = '\r';
		l_linebuffer[l_linepos++] = '\n';
		fwrite(l_linebuffer, 1, l_linepos, m_file);

		// Output the source:
		fwrite(output_value, 1, output_value_len, m_file);

		// Closing bracket:
		l_linepos = 0;
		for ( int i = 0; i < m_level; ++i )
		{
			l_linebuffer[l_linepos++] = '\t';
		}
		l_linebuffer[l_linepos++] = '}';
		l_linebuffer[l_linepos++] = '\r';
		l_linebuffer[l_linepos++] = '\n';
		fwrite(l_linebuffer, 1, l_linepos, m_file);
	}

	return true; // Success! Nothing blew up!
}
template
CORE_API bool io::OSFWriter::WriteEntry<io::OSFEntryInfo> ( const io::OSFEntryInfo& entry, const char* output_value, const size_t output_value_len );
template
CORE_API bool io::OSFWriter::WriteEntry<io::OSFEntryInfoLarge> ( const io::OSFEntryInfoLarge& entry, const char* output_value, const size_t output_value_len );

template <class OSFEntryType>
bool io::OSFWriter::WriteObjectBegin ( const OSFEntryType& object_entry )
{
	/*if ( object_entry.type != kOSFEntryTypeObject )
	{
		return false;
	}*/

	// Create a temp type of object to write the beginning entry:
	OSFEntryType temp_entry = object_entry;
	temp_entry.type = kOSFEntryTypeNormal;
	WriteEntry(temp_entry);

	// Now write the object start
	size_t l_linepos = 0;
	char l_linebuffer[32] = {0};
	for ( int i = 0; i < m_level; ++i )
	{
		l_linebuffer[l_linepos++] = '\t';
	}
	l_linebuffer[l_linepos++] = '{';
	l_linebuffer[l_linepos++] = '\r';
	l_linebuffer[l_linepos++] = '\n';
	fwrite(l_linebuffer, 1, l_linepos, m_file);

	// Increment the level now
	m_level += 1;

	return true;
}
template
CORE_API bool io::OSFWriter::WriteObjectBegin<io::OSFEntryInfo> ( const io::OSFEntryInfo& object_entry );
template
CORE_API bool io::OSFWriter::WriteObjectBegin<io::OSFEntryInfoLarge> ( const io::OSFEntryInfoLarge& object_entry );

bool io::OSFWriter::WriteObjectEnd ( void )
{
	if (m_level <= 0)
	{
		return false;
	}

	// Decrement the level now
	m_level -= 1;

	// Now write the object ending
	size_t l_linepos = 0;
	char l_linebuffer[32] = {0};
	for ( int i = 0; i < m_level; ++i )
	{
		l_linebuffer[l_linepos++] = '\t';
	}
	l_linebuffer[l_linepos++] = '}';
	l_linebuffer[l_linepos++] = '\r';
	l_linebuffer[l_linepos++] = '\n';
	fwrite(l_linebuffer, 1, l_linepos, m_file);

	return true;
}