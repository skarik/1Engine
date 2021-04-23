#ifndef M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_VN_VIEW_GRAPHICS_H_
#define M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_VN_VIEW_GRAPHICS_H_

#include "../SequenceNodeViews.h"

namespace m04 {
namespace editor {
namespace sequence {
namespace vn {

	class PortraitNodeView : public m04::editor::sequence::BarebonesSequenceNodeView
	{
	public:
		EDITOR_API explicit		PortraitNodeView ( SequenceNode* in_node );
	};

	class PortraitActionNodeView : public m04::editor::sequence::BarebonesSequenceNodeView
	{
	public:
		EDITOR_API explicit		PortraitActionNodeView ( SequenceNode* in_node );
	};

	class BackgroundNodeView : public m04::editor::sequence::BarebonesSequenceNodeView
	{
	public:
		EDITOR_API explicit		BackgroundNodeView ( SequenceNode* in_node );
	};

}}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_VN_VIEW_GRAPHICS_H_
