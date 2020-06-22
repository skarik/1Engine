#include "engine-common/dusk/controls/DockablePanel.h"

#include "core/system/Screen.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::elements::DockablePanel::Update ( const UIStepInfo* stepinfo )
{
	dusk::elements::Panel::Update(stepinfo);

	if (m_dockPosition != DockPosition::kUndocked)
	{
		m_overrideLayout = true;

		switch (m_dockPosition)
		{
		case DockPosition::kScreenTop:
			m_absoluteRect = Rect(Vector2f(0, 0), Vector2f((Real32)Screen::Info.width, m_localRect.size.y));
			break;
		case DockPosition::kScreenBottom:
			m_absoluteRect = Rect(Vector2f(0.0F, (Real32)Screen::Info.height - m_localRect.size.y), Vector2f((Real32)Screen::Info.width, m_localRect.size.y));
			break;
		case DockPosition::kScreenLeft:
			m_absoluteRect = Rect(Vector2f(0, 0), Vector2f(m_localRect.size.x, (Real32)Screen::Info.height));
			break;
		case DockPosition::kScreenRight:
			m_absoluteRect = Rect(Vector2f((Real32)Screen::Info.width - m_localRect.size.x, 0.0F), Vector2f(m_localRect.size.x, (Real32)Screen::Info.height));
			break;
		}
	}
	else
	{	// If undocked, then we use default layout
		m_overrideLayout = false;
	}
}