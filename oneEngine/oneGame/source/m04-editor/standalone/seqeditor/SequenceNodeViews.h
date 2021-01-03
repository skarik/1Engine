#ifndef M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_NODE_VIEWS_H_
#define M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_NODE_VIEWS_H_

#include "./SequenceNode.h"

namespace m04 {
namespace editor {
namespace sequence {

	class BarebonesSequenceNodeView : public m04::editor::ISequenceNodeView
	{
	public:
		EDITOR_API explicit		BarebonesSequenceNodeView ( SequenceNode* in_node );

		EDITOR_API virtual void SetFlow ( const int flowOutputIndex, SequenceNode* newNodeValue ) override;
		EDITOR_API virtual SequenceNode* 
								GetFlow ( const int flowOutputIndex ) override;

		EDITOR_API virtual void	SetOutput ( const int outputIndex, SequenceNode* newNodeValue ) override;
		EDITOR_API virtual SequenceNode*
								GetOuptut ( const int outputIndex ) override;

		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const float newFloatValue ) override;
		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const int newIntValue ) override;
		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const bool newBooleanValue ) override;
		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const char* newStringValue ) override;
	};

	class TaskSeqNodeView : public BarebonesSequenceNodeView
	{
	public:
		EDITOR_API explicit		TaskSeqNodeView ( SequenceNode* in_node );
	};

	class MainTaskSeqNodeView : public TaskSeqNodeView
	{
	public:
		EDITOR_API explicit		MainTaskSeqNodeView ( SequenceNode* in_node )
			: TaskSeqNodeView(in_node)
			{}
	};

	class SidetaskSeqNodeView : public TaskSeqNodeView
	{
	public:
		EDITOR_API explicit		SidetaskSeqNodeView ( SequenceNode* in_node )
			: TaskSeqNodeView(in_node)
			{}
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_NODE_VIEWS_H_
