#ifndef M04_EDITORS_SEQUENCE_EDITOR_LISTING_H_
#define M04_EDITORS_SEQUENCE_EDITOR_LISTING_H_

#include <vector>
#include <map>

#include "core/containers/arstring.h"
#include "core-ext/containers/arStringEnum.h"

#include "SequenceNode.h"
#include "SequenceNodeExternalDefinition.h"

namespace m04 {
namespace editor {

	struct SequenceNodePropertyDefinition;
	class SequenceNodeDefinition;
	class SequenceEnumDefinition;

	typedef std::map<arstring128, SequenceEnumDefinition*> SequenceEnumDefinitionMap;
	typedef std::map<arstring128, SequenceNodeDefinition*> SequenceNodeDefinitionMap;

	struct SequenceCategoryDefinition
	{
		arstring64			category;
		SequenceEnumDefinitionMap
							enum_definitions;
		SequenceNodeDefinitionMap
							node_definitions;
	};

	typedef std::vector<SequenceCategoryDefinition> SequenceCategoryDefinitionList;

	class SELInfo
	{
	public:
		//	LoadSequenceEditorListing(filepath) : Loads the given editor info.
		void					LoadSequenceEditorListing ( const char* sel_path );

		void					Free ( void );

	public:
		// Settings about data generated
		SequenceOutputPreference
							output_preference = SequenceOutputPreference::kOsf;
		SequenceGUIDType	guid_preference = SequenceGUIDType::kGUID32;
		SequenceJumpStyle	jump_style = SequenceJumpStyle::kJump;

		// List of all available externally-loaded items
		SequenceCategoryDefinitionList
							definitions;
		// List of all available externally-loaded enums
		/*SequenceEnumDefinitionMap
							enum_definitions;
		// List of all available externally-loaded nodes
		SequenceNodeDefinitionMap
							node_definitions;*/
	};

}};

#endif//M04_EDITORS_SEQUENCE_EDITOR_LISTING_H_