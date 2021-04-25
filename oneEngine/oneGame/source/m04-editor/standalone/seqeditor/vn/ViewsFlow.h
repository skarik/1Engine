#ifndef M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_VN_VIEW_FLOW_H_
#define M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_VN_VIEW_FLOW_H_

#include "../SequenceNodeViews.h"

namespace m04 {
namespace editor {
namespace sequence {
namespace vn {

	class WaitNodeView : public m04::editor::sequence::BarebonesSequenceNodeView
	{
	public:
		EDITOR_API explicit		WaitNodeView ( SequenceNode* in_node );
	};

}}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_VN_VIEW_FLOW_H_
