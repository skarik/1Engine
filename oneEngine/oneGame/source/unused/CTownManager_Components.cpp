
#include "CTownManager.h"
#include "CRegionManager.h"

#include "CVoxelTerrain.h"
#include "CTerraGenDefault.h"

#include "mccvxg.h"
#include "mccosf.h"
#include "png\png.h"

using namespace std;

//  PrepassCultureManifest
// Loads the nodes in the culture manifest.
void CTownManager::PrepassCultureManifest ( void )
{
	for ( int i = 0; i < 8; ++i ) { // Reset positions
		cultureManifestLocations[i].nextchar = 0;
	}

	FILE* file = fopen( ".res/terra/_culture_manifest", "r" );
	COSF_Loader manifest( file );

	mccOSF_entry_info_t entry;
	do {	// Skip past to first marker entry
		manifest.GetNext(entry); 
	}
	while ( entry.type != MCCOSF_ENTRY_MARKER );

	do
	{	// Loop through all the name locations
		manifest.GetNext(entry);
		if ( entry.type == MCCOSF_ENTRY_NORMAL ) {
			int type = atoi( entry.value );
			strcpy( cultureManifestLocations[type].name, entry.name );
		}
	}
	while ( entry.type != MCCOSF_ENTRY_MARKER );

	// Now, loop through the entire file looking for the markers
	do
	{
		// If it's a marker, match it to the list
		if ( entry.type == MCCOSF_ENTRY_MARKER ) {
			for ( int i = 0; i < 8; ++i ) {
				// If there's a match, save the reference data
				if ( strcmp( &(entry.name[1]), cultureManifestLocations[i].name ) == 0 ) {
					cultureManifestLocations[i].nextchar = entry.nextchar;
					cultureManifestLocations[i].level = entry.level;
				}
			}
		}
		// Go to next entry
		manifest.GetNext(entry);
	}
	while ( entry.type != MCCOSF_ENTRY_EOF );

	fclose(file);
}

//	VxgLoadComponent
// Given component name and culture type, will load in a random or designated component.
// Calls VxgToComponent to load the data. Returns false if a failure occurred.
bool CTownManager::LoadComponent ( const World::eCultureType target_culture, const char* component_name, int roll_index )
{
	// Need to open manifest
	FILE* file = fopen( ".res/terra/_culture_manifest", "r" );
	COSF_Loader manifest( file );

	// Seek to target
	if ( cultureManifestLocations[target_culture].nextchar == 0 ) {
		fclose(file);
		return false;
	}
	manifest.GoToMarker( cultureManifestLocations[target_culture] );

	char filenames [8][16];
	int filecount = 0;

	// Loop through the section until next marker
	mccOSF_entry_info_t entry;
	manifest.GetNext(entry);
	manifest.GetNext(entry);
	do
	{
		if ( strcmp( component_name, entry.name ) == 0 )
		{
			if ( roll_index == 0 ) {
				// Looking for first value, return this one.
				strcpy( filenames[filecount], entry.value );
				filecount += 1;
				break;
			}
			else {
				// Add this to a list
				strcpy( filenames[filecount], entry.value );
				filecount += 1;
			}
		}
		// Get next entry
		manifest.GetNext(entry);
	}
	while ( !((entry.type == MCCOSF_ENTRY_MARKER)||(entry.type == MCCOSF_ENTRY_EOF)) );

	// Check for valid filecount
	if ( filecount == 0 ) {
		fclose(file);
		return false;
	}

	// Select target
	int target;
	if ( roll_index == 0 ) {
		target = 0;
	}
	else {
		target = abs(roll_index)%filecount;
	}

	// Load targets and continue
	char temp_filename [64] = ".res/terra/components/";
	strcat( temp_filename, filenames[target] );
	strcat( temp_filename, ".mcc.VXG" );

	// Load file
	VxgToComponent( temp_filename );

	fclose(file);
	return true;
}

//  GetCultureWallheight
// Given target culture, will load in the target wall height.
int CTownManager::GetCultureWallHeight ( const World::eCultureType target_culture )
{
	// Need to open manifest
	FILE* file = fopen( ".res/terra/_culture_manifest", "r" );
	COSF_Loader manifest( file );

	// Seek to target
	if ( cultureManifestLocations[target_culture].nextchar == 0 ) {
		fclose(file);
		return -2; // Bad culture value, return -2
	}
	manifest.GoToMarker( cultureManifestLocations[target_culture] );

	// Loop through the section until next marker
	mccOSF_entry_info_t entry;
	manifest.GetNext(entry);
	do
	{
		if ( strcmp( "wallheight", entry.name ) == 0 ) {
			fclose(file);
			return atoi( entry.value );
		}
		// Get next entry
		manifest.GetNext(entry);
	}
	while ( !((entry.type == MCCOSF_ENTRY_MARKER)||(entry.type == MCCOSF_ENTRY_EOF)) ); 

	// Didn't find it, return -1
	fclose(file);
	return -1;
}

//  GetCultureWallheight
// Given target culture, will load in the target wall height.
int CTownManager::GetCultureRoofOffset ( const World::eCultureType target_culture )
{
	// Need to open manifest
	FILE* file = fopen( ".res/terra/_culture_manifest", "r" );
	COSF_Loader manifest( file );

	// Seek to target
	if ( cultureManifestLocations[target_culture].nextchar == 0 ) {
		fclose(file);
		return -2; // Bad culture value, return -2
	}
	manifest.GoToMarker( cultureManifestLocations[target_culture] );

	// Loop through the section until next marker
	mccOSF_entry_info_t entry;
	manifest.GetNext(entry);
	do
	{
		if ( strcmp( "overhang", entry.name ) == 0 ) {
			fclose(file);
			return atoi( entry.value );
		}
		// Get next entry
		manifest.GetNext(entry);
	}
	while ( !((entry.type == MCCOSF_ENTRY_MARKER)||(entry.type == MCCOSF_ENTRY_EOF)) ); 

	// Didn't find it, return 1
	fclose(file);
	return 1;
}


// FloorplanRotateClockwise
//void CTownManager::FloorplanRotateClockwise ( void )
//{
	/*uint old_width, old_height;
	old_width = floor_plan_width;
	old_height = floor_plan_height;
	floor_plan_width = std::max<uint>( floor_plan_width,floor_plan_height );
	floor_plan_height = floor_plan_width
	unsigned char temp;
	uint x,y;
	for ( x = 0; x < floor_plan_width/2; ++x )
	{
		for ( y = 0; y < floor_plan_height/2; ++y )
		{
			temp = floor_plan_buffer[x+y*floor_plan_width];
			floor_plan_buffer[x+y*floor_plan_width] = floor_plan_buffer[
		}
	}*/
	// Needed? Really? There's flipX and flipY. That's a lot of leeway right there
//}
// FloorplanFlipX
void CTownManager::FloorplanFlipX ( void )
{
	unsigned char temp;
	uint x,y;
	for ( x = 0; x < floor_plan_width/2; ++x )
	{
		for ( y = 0; y < floor_plan_height; ++y )
		{
			temp = floor_plan_buffer[x+y*floor_plan_width];
			floor_plan_buffer[x+y*floor_plan_width] = floor_plan_buffer[(floor_plan_width-x-1)+y*floor_plan_width];
			floor_plan_buffer[(floor_plan_width-x-1)+y*floor_plan_width] = temp;
		}
	}
}
// FloorplanFlipY
void CTownManager::FloorplanFlipY ( void )
{
	unsigned char temp;
	uint x,y;
	for ( x = 0; x < floor_plan_width; ++x )
	{
		for ( y = 0; y < floor_plan_height/2; ++y )
		{
			temp = floor_plan_buffer[x+y*floor_plan_width];
			floor_plan_buffer[x+y*floor_plan_width] = floor_plan_buffer[x+(floor_plan_height-y-1)*floor_plan_width];
			floor_plan_buffer[x+(floor_plan_height-y-1)*floor_plan_width] = temp;
		}
	}
}
// FloorplanRotate180
void CTownManager::FloorplanRotate180 ( void )
{
	unsigned char temp;
	uint x,y;
	for ( x = 0; x < floor_plan_width; ++x )
	{
		for ( y = 0; y < floor_plan_height/2; ++y )
		{
			temp = floor_plan_buffer[x+y*floor_plan_width];
			floor_plan_buffer[x+y*floor_plan_width] = floor_plan_buffer[(floor_plan_width-x-1)+(floor_plan_height-y-1)*floor_plan_width];
			floor_plan_buffer[(floor_plan_width-x-1)+(floor_plan_height-y-1)*floor_plan_width] = temp;
		}
	}
}

// GetFloorplanDoorways
void CTownManager::GetFloorplanDoorways ( vector<sFloorplanNote> & infolist )
{
	// Clear list
	infolist.clear();

	// Loop through the current floorplan's info
	uint x,y;
	for ( x = 0; x < floor_plan_width; ++x ) 
	{
		for ( y = 0; y < floor_plan_height; ++y )
		{
			sFloorplanNote newEntry;
			struct vect2i {
				vect2i( const int a, const int b ) : x(a), y(b) {;};
				int x, y;
				bool operator== ( const vect2i& other ) { return (x==other.x)&&(y==other.y); };
			};
			list<vect2i> recursiveList;
			list<vect2i> doneList;
			switch ( floor_plan_buffer[x+y*floor_plan_width] )
			{
			// in the case of link connectors, need to figure out the direction
			case World::eFloorplanValue::FLR_SubmajorConnector:
			case World::eFloorplanValue::FLR_DoorwayConnector:
			case World::eFloorplanValue::FLR_HallwayConnector:
			//case World::eFloorplanValue::FLR_DoorwayExternal:
				// Create new entry
				newEntry.x = x;
				newEntry.y = y;
				newEntry.w = 0;
				newEntry.h = 0;
				// Loop through x and y at this position to get the size
				recursiveList.clear();
				recursiveList.push_back( vect2i(x,y) );
				while ( !recursiveList.empty() ) {
					vect2i current = recursiveList.front();
					recursiveList.pop_front();
					if ( find( doneList.begin(), doneList.end(), current ) == doneList.end() )
					{
						doneList.push_back( current );
						if ( floor_plan_buffer[current.x+current.y*floor_plan_width] == floor_plan_buffer[x+y*floor_plan_width] )
						{
							if ( current.x != 0 )					recursiveList.push_back( vect2i(current.x-1,current.y) );
							if ( current.x != floor_plan_width-1 )	recursiveList.push_back( vect2i(current.x+1,current.y) );
							if ( current.y != 0 )					recursiveList.push_back( vect2i(current.x,current.y-1) );
							if ( current.y != floor_plan_height-1 )	recursiveList.push_back( vect2i(current.x,current.y+1) );
							// Increase size based on this floor's position
							if ( newEntry.w < 1 + current.x-x ) {
								newEntry.w = 1 + current.x-x;
							}
							if ( newEntry.h < 1 + current.y-y ) {
								newEntry.h = 1 + current.y-y;
							}
						}
					}
				}
				// Floodfilled for size
				if ( newEntry.w == 0 || newEntry.h == 0 ) {
					continue;
				}
				// Now, based on coordinates and size, create direction.
				if ( newEntry.w > newEntry.h ) {
					if ( newEntry.y < floor_plan_height/2 ) {
						newEntry.direction = FRONT;
					}
					else {
						newEntry.direction = BACK;
					}
				}
				else {
					if ( newEntry.x < floor_plan_width/2 ) {
						newEntry.direction = LEFT;
					}
					else {
						newEntry.direction = RIGHT;
					}
				}
				// Get the connection type
				newEntry.type = (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width];
				// Add it to the list
				infolist.push_back( newEntry );
				break;
			};
		}
	}

	// That's it.
}

// ImageToFloorPlan ( PNG filename )
// Loads a PNG file and converts it to a 2D floor plan array.
// The new array is returned. Returns NULL if an error occurs.
// The result is also stored in the fixed size buffer floor_plan_buffer
//  and its width and height stored in floor_plan_width and _height.
char* CTownManager::ImageToFloorPlan ( const char* fname )
{
	int calcW, calcH, calcBPP;
	//unsigned char * pixelData = NULL;

	// Read in the data
	FILE* file = fopen( fname, "rb");  //open the file

	if ( !file )
	{
		fprintf( stderr, "Error reading PNG file %s!!!", fname );
		//pixelData = NULL;
		return NULL;
	}
	else
	{
		unsigned char header[8];    // 8 is the maximum size that can be checked
		fread( header, 1, 8, file );
		if ( png_sig_cmp( header, 0, 8 ) )
			fprintf( stderr, "File %s is not recognized as a PNG file", fname );

		// Define needed vars
		png_structp	png_ptr;
		png_infop	info_ptr;
		png_infop	end_info;

		// Init stuff
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if (!png_ptr) {
			fprintf( stderr, "[loadPNG] png_create_read_struct failed" );
		}

		info_ptr = png_create_info_struct( png_ptr );

		if (!info_ptr) {
			png_destroy_read_struct( &png_ptr, NULL, NULL );
            fprintf( stderr, "[loadPNG] png_create_info_struct failed" );
		}
        if (setjmp(png_jmpbuf(png_ptr))) {
            fprintf( stderr, "[loadPNG] Error during init_io" );
		}

		end_info = png_create_info_struct( png_ptr );
		if (!end_info) {
			png_destroy_read_struct( &png_ptr, &info_ptr, NULL );
            fprintf( stderr, "[loadPNG] png_create_info_struct failed for end_info" );
		}

		// Now, begin the reading
		png_init_io( png_ptr, file );
        png_set_sig_bytes( png_ptr, 8 );

        png_read_info( png_ptr, info_ptr );

        calcW = png_get_image_width(png_ptr, info_ptr);
        calcH = png_get_image_height(png_ptr, info_ptr);

		// Get the color type (this is pretty damn important with PNG's)
		int color_type, bit_depth;
		color_type = png_get_color_type(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);
		if ( color_type == PNG_COLOR_TYPE_RGBA ) {
			calcBPP = (bit_depth / 8)*4;
		}
		else if ( color_type == PNG_COLOR_TYPE_RGB ) {
			calcBPP = (bit_depth / 8)*3;
		}
		else {
			calcBPP = 1;
		}

		int number_of_passes;
        number_of_passes = png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);

		// Read the file
		if (setjmp(png_jmpbuf(png_ptr)))
			fprintf( stderr, "[loadPNG] Error during read_image" );

		// Create input data
		//pixelData = new unsigned char [ calcW * calcH * calcBPP ];

		// Input data to file
		png_bytep * row_pointers;
		row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * calcH);
        for ( int y = 0; y < calcH; y++ ) {
			row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
		}
        png_read_image(png_ptr, row_pointers);

		// Copy PNG data to the pixel data
		/*for ( int y = 0; y < calcH; ++y )
		{
			for ( int x = 0; x < calcW*calcBPP; ++x )
			{
				pixelData[x+(y*calcW*calcBPP)] = row_pointers[y][x];
			}
		}*/
		// Copy the PNG data to the floorplan
		unsigned char r, g, b;
		for ( int y = 0; y < calcH; ++y )
		{
			for ( int x = 0; x < calcW; ++x )
			{
				r = row_pointers[y][x*calcBPP  ];
				g = row_pointers[y][x*calcBPP+1];
				b = row_pointers[y][x*calcBPP+2];
				if ( r == 255 && g == 255 && b == 255 ) {
					floor_plan_buffer[x+y*calcW] = (uchar)World::eFloorplanValue::FLR_WallExternal;
				}
				else if ( r == 255 && g == 128 && b == 128 ) {
					floor_plan_buffer[x+y*calcW] = (uchar)World::eFloorplanValue::FLR_WallInterior;
				}
				else if ( r == 128 && g == 255 && b == 0 ) {
					floor_plan_buffer[x+y*calcW] = (uchar)World::eFloorplanValue::FLR_DoorwayExternal;
				}
				else if ( r == 128 && g == 255 && b == 128 ) {
					floor_plan_buffer[x+y*calcW] = (uchar)World::eFloorplanValue::FLR_DoorwayInterior;
				}
				else if ( r == 0 && g == 64 && b == 64 ) {
					floor_plan_buffer[x+y*calcW] = (uchar)World::eFloorplanValue::FLR_InteriorFlooring;
				}
				else if ( r == 64 && g == 0 && b == 64 ) {
					floor_plan_buffer[x+y*calcW] = (uchar)World::eFloorplanValue::FLR_ExteriorFlooring;
				}
				else if ( r == 255 && g == 192 && b == 128 ) {
					floor_plan_buffer[x+y*calcW] = (uchar)World::eFloorplanValue::FLR_SubmajorConnector;
				}
				else if ( r == 255 && g == 128 && b == 0 ) {
					floor_plan_buffer[x+y*calcW] = (uchar)World::eFloorplanValue::FLR_HallwayConnector;
				}
				else if ( r == 255 && g == 64 && b == 0 ) {
					floor_plan_buffer[x+y*calcW] = (uchar)World::eFloorplanValue::FLR_DoorwayConnector;
				}
				else if ( r == 192 && g == 32 && b == 0 ) {
					floor_plan_buffer[x+y*calcW] = (uchar)World::eFloorplanValue::FLR_StairwellConnector;
				}
				else if ( r == 255 && g == 64 && b == 128 ) {
					floor_plan_buffer[x+y*calcW] = (uchar)World::eFloorplanValue::FLR_InteriorTable;
				}
				else {
					floor_plan_buffer[x+y*calcW] = (uchar)World::eFloorplanValue::FLR_None;
				}
			}
		}

		// Cleanup heap
		for ( int y=0; y<calcH; y++) {
			free(row_pointers[y]);
		}
        free(row_pointers);

		// Free reading
		png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );

		fclose(file);                    //close the file

		// Set plan width and height
		floor_plan_width	= calcW;
		floor_plan_height	= calcH;
		return (char*)floor_plan_buffer;
	}
}

// VxgToComponent ( MCCVXG filename )
// Loads a mcc.VXG file and converts it to a linear component array.
// The new array is returned. Returns NULL if an error occurs.
// The result is also stored in the fixed size buffer component_plan_buffer
//  and its width, height, and depth stored in component_plan_x,*y, and *z.
Terrain::terra_t* CTownManager::VxgToComponent ( const char* fname )
{
	// Read in the data
	FILE* file = fopen( fname, "rb" );  //open the file

	if ( !file )
	{
		fprintf( stderr, "Error reading mcc.VXG file %s!!!", fname );
		return NULL;
	}
	else
	{
		mccVXG_file_infop vxg_infop = mccvxg_begin_io( file );

		// Load info
		mccvxg_read_info( vxg_infop );

		// Load image size
		int bitdepth = 4;
		bitdepth = mccvxg_get_bit_depth( vxg_infop );
		component_plan_x = mccvxg_get_image_size( vxg_infop, MCCVXG_XSIZE );
		component_plan_y = mccvxg_get_image_size( vxg_infop, MCCVXG_YSIZE );
		component_plan_z = mccvxg_get_image_size( vxg_infop, MCCVXG_ZSIZE );

		// Read in data
		mccvxg_read_image( vxg_infop, (void*)(component_plan_buffer) );

		mccvxg_end_io( vxg_infop );

		// Return the plan
		return component_plan_buffer;
	}
	return NULL;
}

// ComponentRotateClockwise
void CTownManager::ComponentRotateClockwise ( void )
{
	memcpy( component_plan_temp, component_plan_buffer, sizeof(Terrain::terra_t) * component_plan_x*component_plan_y*component_plan_z );
	for ( uint x = 0; x < component_plan_x; ++x ) {
		for ( uint y = 0; y < component_plan_y; ++y ) {
			for ( uint z = 0; z < component_plan_z; ++z ) {
				component_plan_buffer[y+x*component_plan_y+z*component_plan_y*component_plan_x] = component_plan_temp[x+y*component_plan_x+z*component_plan_x*component_plan_y];
			}
		}
	}
	uint32_t temp = component_plan_x;
	component_plan_x = component_plan_y;
	component_plan_y = temp;
}
// ComponentFlipX
void CTownManager::ComponentFlipX ( void )
{
	Terrain::terra_t temp;
	for ( uint x = 0; x < component_plan_x/2; ++x ) {
		for ( uint y = 0; y < component_plan_y; ++y ) {
			for ( uint z = 0; z < component_plan_z; ++z ) {
				temp = component_plan_buffer[(component_plan_x-x-1)+y*component_plan_x+z*component_plan_x*component_plan_y];
				component_plan_buffer[(component_plan_x-x-1)+y*component_plan_x+z*component_plan_x*component_plan_y] = component_plan_buffer[x+y*component_plan_x+z*component_plan_x*component_plan_y];
				component_plan_buffer[x+y*component_plan_x+z*component_plan_x*component_plan_y] = temp;
			}
		}
	}
}
// ComponentFlipY
void CTownManager::ComponentFlipY ( void )
{
	Terrain::terra_t temp;
	for ( uint x = 0; x < component_plan_x; ++x ) {
		for ( uint y = 0; y < component_plan_y/2; ++y ) {
			for ( uint z = 0; z < component_plan_z; ++z ) {
				temp = component_plan_buffer[x+(component_plan_y-y-1)*component_plan_x+z*component_plan_x*component_plan_y];
				component_plan_buffer[x+(component_plan_y-y-1)*component_plan_x+z*component_plan_x*component_plan_y] = component_plan_buffer[x+y*component_plan_x+z*component_plan_x*component_plan_y];
				component_plan_buffer[x+y*component_plan_x+z*component_plan_x*component_plan_y] = temp;
			}
		}
	}
}

// ComponentMakeFold
// Makes a line tilable component into a corner piece
void CTownManager::ComponentMakeFold ( void )
{
	memcpy( component_plan_temp, component_plan_buffer, sizeof(Terrain::terra_t) * component_plan_x*component_plan_y*component_plan_z );
	for ( uint x = 0; x < component_plan_x; ++x ) {
		for ( uint y = 0; y < std::min<uint32_t>(component_plan_y,x); ++y ) {
			for ( uint z = 0; z < component_plan_z; ++z ) {
				component_plan_buffer[y+x*component_plan_y+z*component_plan_y*component_plan_x] = component_plan_temp[x+y*component_plan_x+z*component_plan_x*component_plan_y];
			}
		}
	}
	component_plan_x = std::min<uint32_t>( component_plan_x, component_plan_y );
	component_plan_y = component_plan_x;
}
