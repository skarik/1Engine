#ifndef M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_NODE_H_
#define M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_NODE_H_

#include <vector>
#include <unordered_map>
#include "core-ext/containers/osfstructure.h"
#include "core/utils/string.h"

#include "core/math/Vector2.h"
#include "core/math/Vector3.h"
#include "core/math/Color.h"

namespace m04 {
namespace editor {
namespace sequence {
	struct BoardEditorData;
}}}

namespace m04 {
namespace editor {

	class SequenceNode;
	class ISequenceNodeView;

	class SequenceViewFlow;
	class SequenceViewInput;
	class SequenceViewOutput;
	class SequenceViewProperty;

	struct SequenceNodePropertyDefinition;
	class SequenceNodeDefinition;


	//	SequenceNode : Main data storage class for editor nodes.
	// Holds information of the key-value data itself, editor information, and connections.
	class SequenceNode
	{
	public:
		// OSF Native data.
		// Different views on top of this data allow for different editors to be applied to the node.
		osf::ObjectValue	data;
		
		// The view this node is using to display.
		// Essentially dictates the type of node this is.
		ISequenceNodeView*	view = NULL;

		// Data that exists only within the editor.
		// Defined in NodeBoardState.h. Not valid when node loaded for cutscenes.
		m04::editor::sequence::BoardEditorData*
							editorData = nullptr;

	public:
		// List of all nodes that come next in the sequence
		std::vector<SequenceNode*>
							nextNodes;

		// Task sync target. Nodes in parallel tasks that have a sync node will attempt to wait for the given node to pass before continuing.
		SequenceNode*		syncNode = nullptr;

	public:
		// Destructor for clearing out the view.
		~SequenceNode ( void );

	public:
		static SequenceNode*	CreateWithEditorView ( const char* className );
		static SequenceNode*	CreateWithEditorView ( const SequenceNodeDefinition* definition, const char* className );
	};

	class SequenceViewFlow
	{
	public:
		// If this node has a flow input. They are considered sequential.
		// Only zero (0) or one (1) are valid vaues.
		bool				hasInput = true;
		// There is always one flow input - since flow is one way, a single input can take multiple values.
		// Number of flow outputs. They are considered sequential.
		uint				outputCount = 1;
		// If this flow has a sync input/output.
		bool				hasSync = false;
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

		// A generic 2-component float vector input
		kFloat2,
		// A generic 3-component float vector input
		kFloat3,
		// A generic color input
		kColor,

		// A generic array. Usually used only for externally-defined nodes.
		kArray,

		// Dropdown that appears as the character speaking text.
		kScriptCharacter,
		// Text that the character speaks.
		kScriptText,

		// Dropdown that appears, attempting to match the property with an enum
		kEnumtypeDropdown,

		// Node. Usually only used by flow outputs.
		kSequenceNode,

		// Constant. Value that needs to be saved, but isn't visible to editors.
		kConstant,
	};

	PropertyRenderStyle StringToPropertyRenderStyle ( const char* str );

	class SequenceViewProperty
	{
	public:
		arstring64			label = "Property";
		arstring64			identifier = "property";
		PropertyRenderStyle	renderstyle = PropertyRenderStyle::kUnknown;

		const char*			description = nullptr;

		const SequenceNodePropertyDefinition*
							definition = nullptr;
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

		/*EDITOR_API virtual void SetFlow ( const int flowOutputIndex, SequenceNode* newNodeValue );
		EDITOR_API virtual SequenceNode* 
								GetFlow ( const int flowOutputIndex );

		EDITOR_API virtual void	SetOutput ( const int outputIndex, SequenceNode* newNodeValue ) =0;
		EDITOR_API virtual SequenceNode*
								GetOuptut ( const int outputIndex ) =0;*/

		/*EDITOR_API virtual void	SetProperty ( const int propertyIndex, const float newFloatValue ) =0;
		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const int newIntValue ) =0;
		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const bool newBooleanValue ) =0;
		EDITOR_API virtual void	SetProperty ( const int propertyIndex, const char* newStringValue ) =0;

		EDITOR_API virtual void	SetProperty ( const char* stringIndex, const float newFloatValue ) =0;
		EDITOR_API virtual void	SetProperty ( const char* stringIndex, const int newIntValue ) =0;
		EDITOR_API virtual void	SetProperty ( const char* stringIndex, const bool newBooleanValue ) =0;
		EDITOR_API virtual void	SetProperty ( const char* stringIndex, const char* newStringValue ) =0;
		EDITOR_API virtual void	SetProperty ( const char* stringIndex, const Vector2f& newVectorValue ) =0;
		EDITOR_API virtual void	SetProperty ( const char* stringIndex, const Vector3f& newVectorValue ) =0;
		EDITOR_API virtual void	SetProperty ( const char* stringIndex, const Color& newVectorValue ) =0;
		EDITOR_API virtual float
								GetPropertyAsFloat ( const char* stringIndex ) =0;
		EDITOR_API virtual int	GetPropertyAsInt ( const char* stringIndex ) =0;
		EDITOR_API virtual bool	GetPropertyAsBool ( const char* stringIndex ) =0;
		EDITOR_API virtual const char*
								GetPropertyAsString ( const char* stringIndex ) =0;
		EDITOR_API virtual Vector2f
								GetPropertyAsVector2f ( const char* stringIndex ) =0;
		EDITOR_API virtual Vector3f
								GetPropertyAsVector3f ( const char* stringIndex ) =0;
		EDITOR_API virtual Color
								GetPropertyAsColor ( const char* stringIndex ) =0;*/

	public:
		SequenceNode*		node;

		arstring128			classname;
		bool				isExternalClass = false;
		const SequenceNodeDefinition*
							externalClass = nullptr;

		//	FlowList() : Provides the flow information for this node.
		// This is usually one input, and one or more outputs.
		const SequenceViewFlow&	Flow ( void ) const
			{ return flowView; }

		//	InputList() : Provides a list of all the available inputs to this node.
		/*const std::vector<SequenceViewInput>&
								InputList ( void ) const
			{ return inputViews; }

		//	OutputList() : Provides a list of all the available outputs from this node.
		const std::vector<SequenceViewOutput>&
								OutputList ( void ) const
			{ return outputViews; }*/

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


	namespace sequence {
	namespace properties
	{
		EDITOR_API extern void SetProperty ( osf::ObjectValue* data, const char* stringIndex, const float value );
		EDITOR_API extern void SetProperty ( osf::ObjectValue* data, const char* stringIndex, const int value );
		EDITOR_API extern void SetProperty ( osf::ObjectValue* data, const char* stringIndex, const bool value );
		EDITOR_API extern void SetProperty ( osf::ObjectValue* data, const char* stringIndex, const char* value );
		EDITOR_API extern void SetProperty ( osf::ObjectValue* data, const char* stringIndex, const Vector2f value );
		EDITOR_API extern void SetProperty ( osf::ObjectValue* data, const char* stringIndex, const Vector3f value );
		EDITOR_API extern void SetProperty ( osf::ObjectValue* data, const char* stringIndex, const Color value );

		template <typename ReturnType>
		ReturnType GetProperty ( osf::ObjectValue* data, const char* stringIndex );

		extern template EDITOR_API float GetProperty ( osf::ObjectValue* data, const char* stringIndex );
		extern template EDITOR_API int GetProperty ( osf::ObjectValue* data, const char* stringIndex );
		extern template EDITOR_API bool GetProperty ( osf::ObjectValue* data, const char* stringIndex );
		extern template EDITOR_API const char* GetProperty ( osf::ObjectValue* data, const char* stringIndex );
		extern template EDITOR_API Vector2f GetProperty ( osf::ObjectValue* data, const char* stringIndex );
		extern template EDITOR_API Vector3f GetProperty ( osf::ObjectValue* data, const char* stringIndex );
		extern template EDITOR_API Color GetProperty ( osf::ObjectValue* data, const char* stringIndex );
	}}

}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_SQUENCE_NODE_H_