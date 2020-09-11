#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_DOCKABLE_PANEL_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_DOCKABLE_PANEL_H_

#include "engine-common/dusk/controls/Panel.h"

namespace dusk {
namespace elements {

	enum class DockPosition : uint32_t
	{
		kUndocked		= 0xFFFFFFFF,

		kScreenBottom	= 0xFFFFFFFE,
		kScreenTop		= 0xFFFFFFFD,
		kScreenLeft		= 0xFFFFFFFC,
		kScreenRight	= 0xFFFFFFFB,
	};

	struct DockFlags
	{
		enum : uint32_t
		{
			kDefaultMask	= 0x0000,

			kLocked			= 0x0001,
		};
	};

	class DockablePanel : public dusk::elements::Panel
	{
	public:
		ENGCOM_API explicit		DockablePanel (void) :
			dusk::elements::Panel()
			{}

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;

	public:
		DockPosition		m_dockPosition = DockPosition::kUndocked;
		uint32_t			m_dockFlags = DockFlags::kDefaultMask;
		uint32_t			m_dockOrder = 0; // The higher the value, the later in the dock order.
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_DOCKABLE_PANEL_H_