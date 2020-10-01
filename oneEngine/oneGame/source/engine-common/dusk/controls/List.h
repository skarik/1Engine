#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_LIST_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_LIST_H_

#include "engine-common/dusk/Element.h"
#include <vector>

namespace dusk {
namespace elements {

	template <typename PairedType>
	class List : public dusk::Element
	{
	public:
		explicit		List()
			: dusk::Element()
			{}

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override
			{}
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override
			{}

		//	HasSelection() : If this list has something valid selected
		bool					HasSelection ( void )
		{
			return m_selection >= 0 && m_selection < m_list.size();
		}

		//	Selection() : Get the current selection
		const std::pair<std::string, PairedType>&
								Selection ( void )
		{
			if (HasSelection())
			{
				return m_list[m_selection];
			}
			else
			{
				return m_DefaultValue;
			}
		}

		void					SetSelection ( int in_selection );

	public:
		std::vector<std::pair<std::string, PairedType>>
							m_list;

	protected:
		// List state
		int					m_selection = -1; // If value is outside of m_list's range, no selection.

		// Default value for no selection
		const std::pair<std::string, PairedType>
							m_DefaultValue = std::pair<std::string, PairedType>();
	};

#	if _REMOVE_IFDEFS_WHEN_IMPLEMENTED_
	template<>
	class List<void> : public dusk::Element
	{

	public:
		std::vector<std::string>
							m_list;
	};
#	endif

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_LIST_H_