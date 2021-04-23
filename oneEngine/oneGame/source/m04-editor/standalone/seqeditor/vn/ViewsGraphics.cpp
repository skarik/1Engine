#include "ViewsGraphics.h"

DECLARE_SEQUENCENODE_CLASS(VN_Portrait, m04::editor::sequence::vn::PortraitNodeView);

m04::editor::sequence::vn::PortraitNodeView::PortraitNodeView ( SequenceNode* in_node )
	: m04::editor::sequence::BarebonesSequenceNodeView(in_node)
{
	propertyViews.push_back({"Sprite Index", "sprite_index", PropertyRenderStyle::kEnumtypeDropdown});
	propertyViews.push_back({"Character", "character", PropertyRenderStyle::kScriptCharacter});
	propertyViews.push_back({"Blend Time", "time", PropertyRenderStyle::kFloat});

	SetProperty("sprite_index", "-1");
	SetProperty("time", 0.2F);
	SetProperty("character", "nobody");
}

DECLARE_SEQUENCENODE_CLASS(VN_PortraitAction, m04::editor::sequence::vn::PortraitNodeView);

m04::editor::sequence::vn::PortraitActionNodeView::PortraitActionNodeView ( SequenceNode* in_node )
	: m04::editor::sequence::BarebonesSequenceNodeView(in_node)
{
	propertyViews.push_back({"Sprite Index", "sprite_index", PropertyRenderStyle::kEnumtypeDropdown});
	propertyViews.push_back({"Character", "character", PropertyRenderStyle::kScriptCharacter});
	propertyViews.push_back({"Portrait Action", "action", PropertyRenderStyle::kEnumtypeDropdown});
	propertyViews.push_back({"Action Time", "time", PropertyRenderStyle::kFloat});

	SetProperty("sprite_index", "-1");
	SetProperty("time", 0.2F);
	SetProperty("character", "nobody");
}

DECLARE_SEQUENCENODE_CLASS(VN_Background, m04::editor::sequence::vn::BackgroundNodeView);

m04::editor::sequence::vn::BackgroundNodeView::BackgroundNodeView ( SequenceNode* in_node )
	: m04::editor::sequence::BarebonesSequenceNodeView(in_node)
{
	propertyViews.push_back({"Setting", "image", PropertyRenderStyle::kText});
	propertyViews.push_back({"Blend Time", "time", PropertyRenderStyle::kFloat});

	SetProperty("time", 0.2F);
}