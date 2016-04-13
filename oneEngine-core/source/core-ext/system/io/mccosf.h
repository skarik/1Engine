
#ifndef _MCC_OSF_IO_H_
#define _MCC_OSF_IO_H_

// =============================================================
//					  MCC OSF File Interface
// Interface for reading and writing mcc.OSF files
// mcc.Object Serialization Format. Plaintext, human-readable format.
// Is used for generation definitions across the engine, from
//  materials to sound manifests to weapon files.
// Uses the C Standard Library to open, parse, and write files.
// Implementation (C) 2013 by EpicHouse Studios and Joshua Boren
// =============================================================

// Include default system IO
#include "stdio.h"
#include "stdlib.h"

// MCC OSF Enumerations
enum mccOSF_entrytype_enum
{
	MCCOSF_ENTRY_NORMAL = 0,
	MCCOSF_ENTRY_OBJECT,
	MCCOSF_ENTRY_MARKER,

	MCCOSF_ENTRY_END = -1,
	MCCOSF_ENTRY_EOF = -2
};

// MCC OSF Structs
struct mccOSF_entry_info_t
{
	mccOSF_entrytype_enum	type;
	char	name	[64];
	char	value	[128];
	int		level;
	unsigned long nextchar;
};
typedef mccOSF_entry_info_t* mccOSF_entry_infop;

// Loader class definition
class COSF_Loader
{
public:
	//		Constructor
	// Initializes loader with the FILE as input. Does not take ownership.
	explicit				COSF_Loader ( FILE* file );
	//		Destructor
	// NOTE: Does not close the input FILE.
							~COSF_Loader( void );

public:
	//		GetNext () : read next entry.
	// If output_value is not NULL, up to 8K of data may be read into the buffer. It is specifically used for code blocks.
	// Object entries are not automatically entered. Instead, the closing brace "}" is searched for, and next entry searched from there.
	// Returns false, with an entry type of MCCOSF_ENTRY_EOF when no entry is found.
	bool					GetNext ( mccOSF_entry_info_t&, char* output_value=NULL );
	//		GoInto () : enters the given entry, assuming it is an object
	// Enters the given object. GetNext() will return an entry of type MCCOSF_ENTRY_END at the closing brace "}"
	bool					GoInto ( const mccOSF_entry_info_t& );
	//		GoToMarker () : returns file cursor to the given entry
	// Will change what is read next to the entry after the marker.
	bool					GoToMarker ( const mccOSF_entry_info_t& );
	//		SearchToMarker () : locate a marker
	// Looks for marker (#marker) with given name. Returns true when found and sets next entry to the one
	// after the marker.
	bool					SearchToMarker ( const char* name );

private:
	FILE*			m_file;
	unsigned long	m_currentline;
	unsigned long	m_currentchar;
	char			m_linebuffer [256];
	int				m_level;
};

// Write class definition
class COSF_Writer
{
	// TODO
};

#endif//_MCC_OSF_IO_H_