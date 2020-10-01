#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_DROPDOWN_LIST_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_DROPDOWN_LIST_H_

#include "engine-common/dusk/controls/List.h"

namespace dusk {
namespace elements {

	template <typename PairedType>
	class DropdownList : public dusk::elements::List<PairedType>
	{
	public:
		explicit				DropdownList()
			: List<PairedType>()
		{}
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_DROPDOWN_LIST_H_