#ifndef _OBJECT_EDITOR_LISTING_H_
#define _OBJECT_EDITOR_LISTING_H_

#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include <vector>

//=========================================//
// Class definition
//=========================================//

namespace M04
{
	class ObjectEditorListing
	{
	public:
		//		struct listingInfo_t
		// Holds information of the entity needed for the editor.
		struct listingInfo_t
		{
			arstring128	name;
		};

	public:
		explicit	ObjectEditorListing ( void );
					~ObjectEditorListing ( void );

		//		LoadListing () : loads a listing from a file
		// Opens of the OSF file mentioned in the file.
		// Will attempt to read in all entities and other map editing options.
		void		LoadListing ( const char* n_filename );

		//		List () : returns a read-only reference to the entity listing loaded
		const std::vector<listingInfo_t>&	List ( void ) const;

	private:
		bool						is3D;
		std::vector<listingInfo_t>	listing;
	};
}

#endif//_OBJECT_EDITOR_LISTING_H_