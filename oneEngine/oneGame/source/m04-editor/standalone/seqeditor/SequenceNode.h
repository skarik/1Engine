#ifndef M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_NODE_H_
#define M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_NODE_H_

#include <vector>
#include "osfstructure.h"

namespace m04 {
namespace editor {

	class SequenceNode;
	class ISequenceNodeView;

	class SequenceViewFlow;
	class SequenceViewInput;
	class SequenceViewOutput;
	class SequenceViewProperty;

	class SequenceNode
	{
	public:
		// OSF Native data.
		// Different views on top of this data allow for different editors to be applied to the node.
		std::vector<osf::KeyValue*>
							data;
		
		// The view this node is using to display.
		// Essentially dictates the type of node this is.
		ISequenceNodeView*	view = NULL;

	public:
		// Default next node in the sequence.
		SequenceNode*		next = NULL;

		// Task sync target.
		SequenceNode*		task_sync_target = NULL;
	};

	class SequenceViewFlow
	{
	public:
		// Number of flow inputs. They are considered sequential.
		//uint				inputCount = 1;
		// There is always one flow input - since flow is one way, a single input can take multiple values.
		// Number of flow outputs. They are considered sequential.
		uint				outputCount = 1;
	};

	class SequenceViewInput
	{
		arstring64			label = "Input";
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
		arstring64			label = "Output";
	};

	enum class PropertyRenderStyle
	{
		// Unknown property type
		kUnknown,

		// A generic text box
		kText,
		// A generic integer input
		kInteger,
		// A generic float input
		kFloat,
		// A generic boolean input
		kBoolean,

		// Dropdown that appears as the character speaking text.
		kScriptCharacter,
		// Text that the character speaks.
		kScriptText,
	};

	class SequenceViewProperty
	{
	public:
		arstring64			label = "Property";

		PropertyRenderStyle	renderstyle = PropertyRenderStyle::kUnknown;
	public:
		// each property has information on how to render
	};

	// Interface describing the node it's attached to.
	class ISequenceNodeView
	{
	public:
		EDITOR_API explicit		ISequenceNodeView ( SequenceNode* in_node )
			: node(in_node)
			{}

		EDITOR_API virtual void SetFlow ( const int flowOutputIndex, SequenceNode* newNodeValue );

		EDITOR_API virtual void	SetOutput ( const int outputIndex, SequenceNode* newNodeValue ) =0;

		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const float newFloatValue ) =0;
		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const int newIntValue ) =0;
		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const bool newBooleanValue ) =0;
		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const char* newStringValue ) =0;

	public:
		SequenceNode*		node;

		//	FlowList() : Provides the flow information for this node.
		// This is usually one input, and one or more outputs.
		const SequenceViewFlow&	Flow ( void ) const
			{ return flowView; }

		//	InputList() : Provides a list of all the available inputs to this node.
		const std::vector<SequenceViewInput>&
								InputList ( void ) const
			{ return inputViews; }

		//	OutputList() : Provides a list of all the available outputs from this node.
		const std::vector<SequenceViewOutput>&
								OutputList ( void ) const
			{ return outputViews; }

		//	PropertyList() : Provides a list of all the tweakable values for this node.
		const std::vector<SequenceViewProperty>&
								PropertyList ( void ) const
			{ return propertyViews; }


	protected:
		SequenceViewFlow	flowView;
		std::vector<SequenceViewInput>
							inputViews;
		std::vector<SequenceViewOutput>
							outputViews;
		std::vector<SequenceViewProperty>
							propertyViews;
	};


}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_NODE_H_