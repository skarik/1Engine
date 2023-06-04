#ifndef M04_EDITORS_SEQUENCE_EDITOR_LISTING_H_
#define M04_EDITORS_SEQUENCE_EDITOR_LISTING_H_

#include <vector>
#include <map>

#include "core/containers/arstring.h"
#include "core-ext/containers/arStringEnum.h"

#include "SequenceNode.h"

namespace m04 {
namespace editor {

	struct SequenceNodePropertyDefinition;
	struct SequenceNodePropertyDefinition
	{
		// Internal name of the property, used for serialization
		arstring64			name;
		// Visible name of the property, displayed in the editor
		arstring64			displayName;
		// Type this property is. Controls the type of property view on this property.
		PropertyRenderStyle	type = PropertyRenderStyle::kUnknown;
		// If an enum, controls the name of the enum used to set values.
		arstring64			enumName;
		// The default value of the property, in string format.
		arstring64			defaultValue;
		// If the type is an Array, this points to a list of properties per each index of the array.
		// The entire list of properties describes one item.
		std::vector<SequenceNodePropertyDefinition>*
							arraySubproperties = nullptr;
		
		//	AllocateSubproperties() : Call to allocate arraySubproperties
		void AllocateSubproperties ( void )
		{
			if (!arraySubproperties)
			{
				arraySubproperties = new std::vector<SequenceNodePropertyDefinition>();
			}
		}

		//	Free() : Call to clean up. Recursively cleans up any properties.
		void Free ( void )
		{
			if (arraySubproperties)
			{
				for (auto& nodeProperty : *arraySubproperties)
				{
					nodeProperty.Free();
				}
				delete_safe(arraySubproperties);
			}
		}
	};

	class SequenceNodeDefinition
	{
	public:
		arstring64			displayName;
		arstring64			category;
		// Number of outputs to the node. Negative means the number of outputs is automatically determined.
		int					outputCount = 1;
		std::vector<SequenceNodePropertyDefinition>
							properties;
	};

	enum class SequenceOutputPreference
	{
		kOsf,
		kJson,
	};

	enum class SequenceGUIDType
	{
		kGUID32,
		kUUID4,
	};

	enum class SequenceJumpStyle
	{
		kJump,
		kLink,
	};

	SequenceOutputPreference StringToSequenceOutputPreference ( const char* str );
	SequenceGUIDType StringToSequenceGUIDType ( const char* str );
	SequenceJumpStyle StringToSequenceJumpStyle ( const char* str );

	class SequenceEnumDefinition
	{
	public:
		SequenceEnumDefinition ( arStringEnumDefinition* def, std::vector<arstring64>&& names )
			: enumDefinition(def)
			, displayNames(names)
		{}
		~SequenceEnumDefinition ( void );

		arStringEnumDefinition*	enumDefinition = nullptr;
		std::vector<arstring64> displayNames;
	};

	class SELInfo
	{
	public:
		//	LoadSequenceEditorListing(filepath) : Loads the given editor info.
		void					LoadSequenceEditorListing ( const char* sel_path );

		void					Free ( void );

	public:
		// Settings about data generated
		arstring64			next_node_category = "";
		SequenceOutputPreference
							output_preference = SequenceOutputPreference::kOsf;
		SequenceGUIDType	guid_preference = SequenceGUIDType::kGUID32;
		SequenceJumpStyle	jump_style = SequenceJumpStyle::kJump;

		// List of all available externally-loaded enums
		std::map<arstring128, SequenceEnumDefinition*>
							enum_definitions;
		// List of all available externally-loaded nodes
		std::map<arstring128, SequenceNodeDefinition*>
							node_definitions;
	};

}};

#endif//M04_EDITORS_SEQUENCE_EDITOR_LISTING_H_