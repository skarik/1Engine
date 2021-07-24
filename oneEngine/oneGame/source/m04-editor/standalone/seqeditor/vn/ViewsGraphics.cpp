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

	SetProperty("sprite_index", "-1");
	SetProperty("time", 0.2F);
	SetProperty("character", "nobody");
	SetProperty("expression", "neutral");
	SetProperty("wait_for_finish", true);
}

DECLARE_SEQUENCENODE_CLASS(VN_PortraitAction, m04::editor::sequence::vn::PortraitActionNodeView);

m04::editor::sequence::vn::PortraitActionNodeView::PortraitActionNodeView ( SequenceNode* in_node )
	: m04::editor::sequence::vn::PortraitNodeView(in_node)
{
	propertyViews.push_back({"Portrait Action", "portrait_action", PropertyRenderStyle::kEnumtypeDropdown});
	propertyViews.push_back({"Position", "position", PropertyRenderStyle::kFloat2});
	propertyViews.push_back({"Scale", "scale", PropertyRenderStyle::kFloat2});
	propertyViews.push_back({"Color", "color", PropertyRenderStyle::kColor});

	SetProperty("portrait_action", "default");
	SetProperty("position", Vector2f(1.0F, 1.0F));
	SetProperty("scale", Vector2f(1.0F, 1.0F));
	SetProperty("color", Color(1, 1, 1, 1.0));
}

DECLARE_SEQUENCENODE_CLASS(VN_Background, m04::editor::sequence::vn::BackgroundNodeView);

m04::editor::sequence::vn::BackgroundNodeView::BackgroundNodeView ( SequenceNode* in_node )
	: m04::editor::sequence::BarebonesSequenceNodeView(in_node)
{
	propertyViews.push_back({"Setting", "image", PropertyRenderStyle::kText});
	propertyViews.push_back({"Blend Time", "time", PropertyRenderStyle::kFloat});
	propertyViews.push_back({"Wait For Finish", "wait_for_finish", PropertyRenderStyle::kBoolean, "If we should wait on this action (true) or immediately go to next node and start a side-task to finish this one (false)."});

	SetProperty("sprite_index", "default");
	SetProperty("time", 0.2F);
	SetProperty("wait_for_finish", true);
}