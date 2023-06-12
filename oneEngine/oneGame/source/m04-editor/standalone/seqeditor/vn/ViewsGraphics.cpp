#include "ViewsGraphics.h"

DECLARE_SEQUENCENODE_CLASS(VN_Portrait, m04::editor::sequence::vn::PortraitNodeView);

m04::editor::sequence::vn::PortraitNodeView::PortraitNodeView ( SequenceNode* in_node )
	: m04::editor::sequence::BarebonesSequenceNodeView(in_node)
{
	propertyViews.push_back({"Sprite Index", "sprite_index", PropertyRenderStyle::kEnumtypeDropdown});
	propertyViews.push_back({"Character", "character", PropertyRenderStyle::kScriptCharacter});
	propertyViews.push_back({"Expression", "expression", PropertyRenderStyle::kEnumtypeDropdown});
	propertyViews.push_back({"Time", "time", PropertyRenderStyle::kFloat, "Time in seconds to fade in character or perform given action over."});
	propertyViews.push_back({"Wait For Finish", "wait_for_finish", PropertyRenderStyle::kBoolean, "If we should wait on this action (true) or immediately go to next node and start a side-task to finish this one (false)."});

	properties::SetProperty(&node->data, "sprite_index", "-1");
	properties::SetProperty(&node->data, "time", 0.2F);
	properties::SetProperty(&node->data, "character", "nobody");
	properties::SetProperty(&node->data, "expression", "neutral");
	properties::SetProperty(&node->data, "wait_for_finish", true);
}

DECLARE_SEQUENCENODE_CLASS(VN_PortraitAction, m04::editor::sequence::vn::PortraitActionNodeView);

m04::editor::sequence::vn::PortraitActionNodeView::PortraitActionNodeView ( SequenceNode* in_node )
	: m04::editor::sequence::vn::PortraitNodeView(in_node)
{
	propertyViews.push_back({"Portrait Action", "portrait_action", PropertyRenderStyle::kEnumtypeDropdown});
	propertyViews.push_back({"Position", "position", PropertyRenderStyle::kFloat2});
	propertyViews.push_back({"Scale", "scale", PropertyRenderStyle::kFloat2});
	propertyViews.push_back({"Color", "color", PropertyRenderStyle::kColor});

	properties::SetProperty(&node->data, "portrait_action", "default");
	properties::SetProperty(&node->data, "position", Vector2f(1.0F, 1.0F));
	properties::SetProperty(&node->data, "scale", Vector2f(1.0F, 1.0F));
	properties::SetProperty(&node->data, "color", Color(1, 1, 1, 1.0));
}

DECLARE_SEQUENCENODE_CLASS(VN_Background, m04::editor::sequence::vn::BackgroundNodeView);

m04::editor::sequence::vn::BackgroundNodeView::BackgroundNodeView ( SequenceNode* in_node )
	: m04::editor::sequence::BarebonesSequenceNodeView(in_node)
{
	propertyViews.push_back({"Setting", "image", PropertyRenderStyle::kText});
	propertyViews.push_back({"Blend Time", "time", PropertyRenderStyle::kFloat});
	propertyViews.push_back({"Wait For Finish", "wait_for_finish", PropertyRenderStyle::kBoolean, "If we should wait on this action (true) or immediately go to next node and start a side-task to finish this one (false)."});

	properties::SetProperty(&node->data, "sprite_index", "default");
	properties::SetProperty(&node->data, "time", 0.2F);
	properties::SetProperty(&node->data, "wait_for_finish", true);
}