
#include "ObjectEditorListing.h"
#include "core-ext/system/io/mccosf.h"
#include "core-ext/system/io/Resources.h"

using namespace M04;

ObjectEditorListing::ObjectEditorListing ( void )
{

}
ObjectEditorListing::~ObjectEditorListing ( void )
{

}

//		LoadListing () : loads a listing from a file
// Opens of the OSF file mentioned in the file.
// Will attempt to read in all entities and other map editing options.
void ObjectEditorListing::LoadListing ( const char* n_filename )
{
	FILE* file = core::Resources::Open( n_filename, "rb" );
	if ( file == NULL )
	{
		throw core::MissingFileException();
	}
	COSF_Loader osf( file );

	// With the loader active, loop through every entry.
	mccOSF_entry_info_t entry;
	do
	{
		osf.GetNext( entry );
		if ( entry.type == MCCOSF_ENTRY_NORMAL )
		{
			// Add an abridge entity
			if ( streq(entry.name,"entity") )
			{
				listingInfo_t info;
				info.name = entry.value;
				listing.push_back(info);
			}
			// Check editor option
			else if ( streq(entry.name,"editor") )
			{
				if ( streq(entry.name,"3d") || streq(entry.name,"3D") ) {
					is3D = true;
				}
				else if ( streq(entry.name,"2d") || streq(entry.name,"2D") ) {
					is3D = false;
				}
			}
		}
		else
		{
			// Add an entity
			if ( streq(entry.name,"entity") )
			{
				listingInfo_t info;
				info.name = entry.value;
				listing.push_back(info);
			}
		}
	}
	while ( entry.type != MCCOSF_ENTRY_EOF );

	// Close the file
	fclose(file);
}

//		List () : returns a read-only reference to the entity listing loaded
const std::vector<ObjectEditorListing::listingInfo_t>& ObjectEditorListing::List ( void ) const
{
	return listing;
}