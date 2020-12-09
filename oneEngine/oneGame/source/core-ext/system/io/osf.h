//===============================================================================================//
//
//		OSF File Interface
//		a similar format to Valve KeyValue, but without the JSON style ugliness!
//
// Provided below is an interface for reading and writing OSF files.
// Object Serialization Format. Plaintext, human-readable format.
// Is used for generation definitions across the engine, included materials, sound manifests,
//  cutscenes, editor definitions, and weapon files.
// Uses the C Standard Library to red and write files, and thus should be compatible across
//  multiple platforms.
//
//===============================================================================================//
#ifndef CORE_OSF_IO_H_
#define CORE_OSF_IO_H_

#include "core/types/types.h"
#include <stdio.h>
#include <stdlib.h>

namespace io
{
	// OSF entry type
	enum eOSFEntryType
	{
		// Normal entry. This is a normal keyValue space-separated entry.
		kOSFEntryTypeNormal = 0,
		// Object entry. This is a keyValue space-separated entry that is followed by a {} group.
		// After this is returned, the entry can be entered by using OSFReader::GoInto().
		kOSFEntryTypeObject = 1,
		// The given entry is a marker/marco type entry.
		// The positions of all markers are saved and can be used to "mark" a location in the OSF file to seek to later.
		// In the OSF file, their syntax is of the form `#Marker` or `$Marker`.
		kOSFEntryTypeMarker = 2,
		// Object-source entry. This is a keyValue space-separated entry, with a specific value that is followed by a {} group.
		// Currently valid values are `$code`, `$lua`, and `$js`, which tell the parser that the next {} group contains source code.
		kOSFEntryTypeSource = 3,

		// Invalid entry type.
		// The end of an Object, which is the `}` of a `{...}` group, has been reached.
		kOSFEntryTypeEnd = -1,
		// Invalid entry type.
		// The end of the file has been reached.
		kOSFEntryTypeEoF = -2,
		// Invalid entry type.
		// The entry hasn't been parsed yet.
		kOSFEntryTypeUnknown = -3,
	};

	// OSF Entry information
	struct OSFEntryInfo
	{
		eOSFEntryType	type = kOSFEntryTypeUnknown;
		char			name [64];		// The `key` of the key-value pair
		char			value [128];	// The `value` of the key-value pair
		int				level = 0;
		unsigned long	nextchar = 0;
	};
	typedef OSFEntryInfo* OSFEntryInfop;

	// Class for reading from OSF formatted text files.
	// It will not write take ownership of the file. The file must be closed by the user.
	// Note: This is currently not performant when reading large OSF files or blocks of code. Try to save results.
	class OSFReader
	{
	public:
		// Initializes loader with the FILE as input. Does not take ownership.
		CORE_API explicit		OSFReader ( FILE* file );
		// Does not close the input FILE.
		CORE_API				~OSFReader( void );

	public:
		//		GetNext( entry, output_value, output_value_len ) : searches for and reads next entry.
		// If output_value is not NULL, data may be read into the buffer. It can be used to read in code blocks.
		// Object entries are not automatically entered. Instead, the closing brace "}" is searched for, and next search starts from there.
		// Returns false, with an entry type of kOSFEntryTypeEoF when no entry is found.
		// Arguments:
		//	OUT entry:			next entry that is read
		//	output_value:		buffer to read long values into. can be NULL
		//	output_value_len:	length of the buffer. ignored if output_value is null
		// Returns:
		//	bool: No errors on read and no EoF found.
		CORE_API bool			GetNext ( OSFEntryInfo& out_entry, char* output_value = NULL, const size_t output_value_len = 0 );
		//		GoInto( object_entry ) : enters the given entry, assuming it is an object
		// Enters the given object. GetNext() will return an entry of type kOSFEntryTypeEnd at the closing brace "}"
		//	bool: No errors on set and entry is valid object.
		CORE_API bool			GoInto ( const OSFEntryInfo& object_entry );
		//		GoToMarker( marker_entry ) : returns file cursor to the given entry
		// Will change what is read next to the entry after the marker.
		// Returns:
		//	bool: No errors on set and entry is valid marker.
		CORE_API bool			GoToMarker ( const OSFEntryInfo& marker_entry );
		//		SearchToMarker () : locate a marker
		// Looks for marker (#Marker) with given name. Sets next entry to the one after the marker.
		// Returns:
		//	bool: true when found.
		CORE_API bool			SearchToMarker ( const char* name );

	private:
		static const int	kLineBufferLen = 1024;

		FILE*				m_file;
		unsigned long		m_currentline;
		unsigned long		m_currentchar;
		char				m_linebuffer [1024];
		int					m_level;
	};


	enum eOSFWriteMode
	{
		// Rewrites the file completely. 
		// Is the default mode, and doesn't have extra requirements.
		kOSFWriteModeFast,

		// Tries to make sure that comments are preserved while writing.
		// Requires that the file is opened in read-append mode.
		kOSFWriteModePreserveComments,
	};

	// Class for writing OSF data to files.
	// It will not write take ownership of the file. The file must be closed by the user.
	class OSFWriter
	{
	public:
		// Initializes loader with the FILE as input. Does not take ownership.
		CORE_API explicit		OSFWriter ( FILE* file, eOSFWriteMode writeMode = kOSFWriteModeFast );
		// NOTE: Does not close the input FILE.
		CORE_API				~OSFWriter( void );

	public:
		//		WriteEntry( entry ) : Writes the given entry in the current object.
		// Indenting is done with 4-space tab characters.
		// This cannot write object entries. Those must be done with WriteObjectBegin and WriteObjectEnd.
		// Arguments:
		//	entry:				entry to write
		//	output_value:		buffer to write, used when entry is kOSFEntryTypeSource
		//	output_value_len:	length of the buffer. ignored if output_value is null
		// Returns:
		//	bool: true when written.
		CORE_API bool			WriteEntry ( const OSFEntryInfo& entry, char* output_value = NULL, const size_t output_value_len = 0 );
		//		WriteObjectBegin( entry ) : Writes the given entry as the start of an object.
		// The entry is written normally, with a following { character. The scope is incremented.
		// Returns:
		//	bool: true when written.
		CORE_API bool			WriteObjectBegin ( const OSFEntryInfo& object_entry );
		//		WriteObjectEnd( entry ) : Writes end of an arbitrary object `}` and decrements scope.
		// Returns:
		//	bool: true when written. false if there are scope issues.
		CORE_API bool			WriteObjectEnd ( void );

	private:
		static const int	kLineBufferLen = 1024;

		FILE*				m_file;
		int					m_level;
		eOSFWriteMode		m_mode;
	};
}

#endif//CORE_OSF_IO_H_