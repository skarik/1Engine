#include "ViewsLines.h"

DECLARE_SEQUENCENODE_CLASS(VN_Lines, m04::editor::sequence::vn::LinesNodeView);

m04::editor::sequence::vn::LinesNodeView::LinesNodeView ( SequenceNode* in_node )
	: m04::editor::sequence::BarebonesSequenceNodeView(in_node)
{
	propertyViews.push_back({"Line", "line0", PropertyRenderStyle::kScriptText});
	propertyViews.push_back({"Character", "character", PropertyRenderStyle::kScriptCharacter});
	propertyViews.push_back({"Box Style", "style", PropertyRenderStyle::kEnumtypeDropdown});
	propertyViews.push_back({"Ending Style", "end", PropertyRenderStyle::kEnumtypeDropdown});
	; // TODO

	properties::SetProperty(&node->data, "count", 1);
	properties::SetProperty(&node->data, "audio0", ".");
	properties::SetProperty(&node->data, "character", "nobody");
	properties::SetProperty(&node->data, "style", "default");
	properties::SetProperty(&node->data, "end", "default");
}

DECLARE_SEQUENCENODE_CLASS(VN_LinesUpdate, m04::editor::sequence::vn::LinesUpdateNodeView);

m04::editor::sequence::vn::LinesUpdateNodeView::LinesUpdateNodeView ( SequenceNode* in_node )
	: m04::editor::sequence::BarebonesSequenceNodeView(in_node)
{
	propertyViews.push_back({"Line", "line0", PropertyRenderStyle::kScriptText});
	propertyViews.push_back({"Character", "character", PropertyRenderStyle::kScriptCharacter});

	properties::SetProperty(&node->data, "count", 1);
	properties::SetProperty(&node->data, "audio0", ".");
	properties::SetProperty(&node->data, "character", "nobody");
}