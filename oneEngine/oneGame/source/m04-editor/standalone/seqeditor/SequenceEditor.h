#ifndef M04_EDITORS_SEQUENCE_EDITOR_MAIN_
#define M04_EDITORS_SEQUENCE_EDITOR_MAIN_

#include <vector>

#include "osfstructure.h"

#include "engine/behavior/CGameBehavior.h"
#include "m04/eventide/UserInterface.h"

namespace m04 {
namespace editor {

	class SeqeunceNode;
	class SequenceNodeView;

	class SequenceViewFlow;
	class SequenceViewInput;
	class SequenceViewOutput;
	class SequenceViewProperty;

	class SequenceNodeView
	{
	public:
		SeqeunceNode*		node;

		//	FlowList() : Provides the flow information for this node.
		// This is usually one input, and one or more outputs.
		const SequenceViewFlow*	Flow ( void ) const
			{ return flowView; }

		//	InputList() : Provides a list of all the available inputs to this node.
		const std::vector<SequenceViewInput*>&
								InputList ( void ) const
			{ return inputViews; }

		//	OutputList() : Provides a list of all the available outputs from this node.
		const std::vector<SequenceViewOutput*>&
								OutputList ( void ) const
			{ return outputViews; }

		//	PropertyList() : Provides a list of all the tweakable values for this node.
		const std::vector<SequenceViewProperty*>&
								PropertyList ( void ) const
			{ return propertyViews; }


	protected:
		SequenceViewFlow*	flowView = NULL;
		std::vector<SequenceViewInput*>
							inputViews;
		std::vector<SequenceViewOutput*>
							outputViews;
		std::vector<SequenceViewProperty*>
							propertyViews;
	};

	class SeqeunceNode
	{
	public:
		// OSF Native data.
		// Different views on top of this data allow for different editors to be applied to the node.
		osf::KeyValue*		data;
		
		// The view this node is using to display
		SequenceNodeView*	view;

	public:
		// Task sync target.
		SeqeunceNode*		task_sync_target;
	};

	class SequenceViewFlow
	{
	public:
		// Number of flow inputs. They are considered sequential.
		uint				inputCount;
		// Number of flow outputs. They are considered sequential.
		uint				outputCount;
	};

	class SequenceViewInput
	{
		arstring64			label = arstring64("Input");
	};

	enum class OutputType
	{
		// The output is invalid. This cannot be connected to anything.
		kInvalid = 0,
		// The output is a new task. This should be treated as a Flow connection.
		kTask = 1,
	};

	enum class OutputPosition
	{
		// The output is placed opposite of the inputs
		kWithOutputs = 0,
		// The output is placed on the side of the node, between inputs and outputs.
		kOnSide = 1,
	};

	class SequenceViewOutput
	{
	public:
		OutputType			type = OutputType::kInvalid;
		OutputPosition		position = OutputPosition::kWithOutputs;
		arstring64			label = arstring64("Output");
	};

	enum class PropertyRenderStyle
	{
		// Dropdown that appears as the character speaking text.
		kScriptCharacter,
		// Text that the character speaks.
		kScriptText,
	};

	class SequenceViewProperty
	{
	public:
		arstring64			label = arstring64("Property");

	public:
		// each property has information on how to render
	};

	// We need different views on the data....

	class SequenceEditor : public CGameBehavior
	{
	public:
		EDITOR_API explicit		SequenceEditor ( void );
		EDITOR_API				~SequenceEditor ( void );

		EDITOR_API void			Update ( void );

	protected:
		ui::eventide::UserInterface*
							user_interface = NULL;

		ui::eventide::Element*
							test_element = NULL;

		// Is the camera being currently dragged?
		bool				dragging_view = false;
		// XYZ position that we want to keep trained under the mouse
		Vector3f			dragging_reference_position;
		// Is the camera being currently zoom-dragged?
		bool				zooming_view = false;
		//	UpdateCameraControl() : Does camera panning & zooming
		void				UpdateCameraControl ( void );
	};
}};

#endif///M04_EDITORS_SEQUENCE_EDITOR_MAIN_