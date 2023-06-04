#include "SequenceNode.h"
#include "core/containers/arstring.h"
#include "core/utils/stringcase.h"

m04::editor::PropertyRenderStyle m04::editor::StringToPropertyRenderStyle (const char* str)
{
	string_switch_begin_suppress_warnings();
	string_switch(str)
	{
		string_case("text"):		return PropertyRenderStyle::kText;
		string_case("integer"):		return PropertyRenderStyle::kInteger;
		string_case("float"):		return PropertyRenderStyle::kFloat;
		string_case("bool"):
		string_case("boolean"):		return PropertyRenderStyle::kBoolean;

		string_case("float2"):		return PropertyRenderStyle::kFloat2;
		string_case("float3"):		return PropertyRenderStyle::kFloat3;
		string_case("color"):		return PropertyRenderStyle::kColor;

		string_case("array"):		return PropertyRenderStyle::kArray;

		string_case("scriptCharacter"):
									return PropertyRenderStyle::kScriptCharacter;
		string_case("scriptText"):	return PropertyRenderStyle::kScriptText;

		string_case("list"):	
		string_case("dropdown"):	return PropertyRenderStyle::kEnumtypeDropdown;

		string_case("node"):		return PropertyRenderStyle::kSequenceNode;
			
		string_case("unknown"):	
		default:					return PropertyRenderStyle::kUnknown;
	};
	string_switch_end_suppress_warnings();
}
