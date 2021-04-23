#ifndef M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_VN_VIEW_LINES_H_
#define M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_VN_VIEW_LINES_H_

#include "../SequenceNodeViews.h"

namespace m04 {
namespace editor {
namespace sequence {
namespace vn {

	class LinesNodeView : public m04::editor::sequence::BarebonesSequenceNodeView
	{
	public:
		EDITOR_API explicit		LinesNodeView ( SequenceNode* in_node );
	};

	class LinesUpdateNodeView : public m04::editor::sequence::BarebonesSequenceNodeView
	{
	public:
		EDITOR_API explicit		LinesUpdateNodeView ( SequenceNode* in_node );
	};

}}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_VN_VIEW_LINES_H_
