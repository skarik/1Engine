#ifndef M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_NODE_H_
#define M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_NODE_H_

#include <vector>
#include <unordered_map>
#include "core-ext/containers/osfstructure.h"
#include "core/utils/string.h"

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
		osf::ObjectValue	data;
		
		// The view this node is using to display.
		// Essentially dictates the type of node this is.
		ISequenceNodeView*	view = NULL;

	public:
		// Default next node in the sequence.
		SequenceNode*		next = NULL;

		// Task sync target.
		SequenceNode*		task_sync_target = NULL;

	public:
		// Destructor for clearing out the view.
		~SequenceNode ( void );

	public:
		static SequenceNode*	CreateWithEditorView ( const char* className );
	};

	class SequenceViewFlow
	{
	public:
		// Number of flow inputs. They are considered sequential.
		// Only zero (0) or one (1) are valid vaues.
		uint				inputCount = 1;
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

		// Dropdown that appears, attempting to match the property with an enum
		kEnumtypeDropdown,

		// Node. Usually only used by flow outputs.
		kSequenceNode,
	};

	class SequenceViewProperty
	{
	public:
		arstring64			label = "Property";
		arstring64			identifier = "property";

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
		EDITOR_API virtual SequenceNode* 
								GetFlow ( const int flowOutputIndex );

		EDITOR_API virtual void	SetOutput ( const int outputIndex, SequenceNode* newNodeValue ) =0;
		EDITOR_API virtual SequenceNode*
								GetOuptut ( const int outputIndex ) =0;

		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const float newFloatValue ) =0;
		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const int newIntValue ) =0;
		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const bool newBooleanValue ) =0;
		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const char* newStringValue ) =0;

		EDITOR_API virtual void	SetProperty ( const char* stringIndex, const float newFloatValue ) =0;
		EDITOR_API virtual void	SetProperty ( const char* stringIndex, const int newIntValue ) =0;
		EDITOR_API virtual void	SetProperty ( const char* stringIndex, const bool newBooleanValue ) =0;
		EDITOR_API virtual void	SetProperty ( const char* stringIndex, const char* newStringValue ) =0;
		EDITOR_API virtual float
								GetPropertyAsFloat ( const char* stringIndex ) =0;
		EDITOR_API virtual int	GetPropertyAsint ( const char* stringIndex ) =0;
		EDITOR_API virtual bool	GetPropertyAsBool ( const char* stringIndex ) =0;
		EDITOR_API virtual const char*
								GetPropertyAsString ( const char* stringIndex ) =0;

	public:
		SequenceNode*		node;

		arstring128			classname;

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

	class ISequenceNodeClassInfo
	{
	public:
		//	CreateNew() : Create a new instance of the given sequence node.
		virtual SequenceNode*	CreateNew (void) = 0;

		//	GetInfo(classname) : Locates classinfo instance with the match class name.
		// NULL is returned if a matching class name cannot be found.
		EDITOR_API static ISequenceNodeClassInfo*
								GetInfo ( const char* classname )
		{
			auto classInfoItr = m_registry.find(classname);
			if (classInfoItr == m_registry.end())
			{
				return NULL;
			}
			else
			{
				return classInfoItr->second;
			}
		}
	public:
		static std::unordered_map<arstring128,ISequenceNodeClassInfo*>
							m_registry;
		static std::vector<ISequenceNodeClassInfo*>
							m_ordereredRegistry;

		// Node's raw class name
		arstring128			m_classname;
		// Node's class name for display.
		arstring128			m_displayname;
	};
	
#	define DECLARE_SEQUENCENODE_CLASS(SequenceNodeClass, EditorView) \
	static class SequenceNodeClass_##SequenceNodeClass : public m04::editor::ISequenceNodeClassInfo \
	{ \
	public: \
		/* Constructor, adds self to the registry. */ \
		SequenceNodeClass_##SequenceNodeClass() \
		{ \
			m_registry[ #SequenceNodeClass ] = this;\
			m_ordereredRegistry.push_back(this);\
			m_classname = #SequenceNodeClass;\
			m_displayname = core::utils::string::CamelCaseToReadable( #SequenceNodeClass, strlen( #SequenceNodeClass ) );\
		} \
		\
		/* CreateNew override, constructs new node & view. */ \
		virtual m04::editor::SequenceNode*	CreateNew (void) override \
		{ \
			m04::editor::SequenceNode* node = new m04::editor::SequenceNode;\
			node->view = new EditorView(node); \
			node->view->classname = #SequenceNodeClass; \
			return node; \
		} \
	} InstSequenceNodeClass_##SequenceNodeClass;\

}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_NODE_H_