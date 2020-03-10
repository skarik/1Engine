
#ifndef _BEHAVIOR_LIST_H_
#define _BEHAVIOR_LIST_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "core/containers/arsingleton.h"
#include "core/exceptions/exceptions.h"
#include "core/math/Math3d.h"

#include <vector>
#include <utility>
#include <unordered_map>

class CGameBehavior;

//===============================================================================================//
// ENUMERATIONS
//===============================================================================================//

enum fieldtype_t : uint32_t
{
	// Default serialization mode. Automatically attempts to match data size and performs lossless compression.
	FIELD_DEFAULT,

	// Specific field for serializing Vector3's that represent positions.
	// Only one FIELD_POSITION is valid per object.
	FIELD_POSITION,
	// Specific field for serializing Vector2's that represent positions.
	// Only one FIELD_ROTATION is valid per object.
	FIELD_POSITION2D,
	// Specific field for serializing Vector4's or Quaternion's that represent rotation.
	// Only one FIELD_ROTATION is valid per object.
	FIELD_ROTATION,
	// Specific field for serializing Vector3's that represent scaling.
	// Only one FIELD_SCALE is valid per object.
	FIELD_SCALE,
	// Specific field for serializing Vector4's or Color's that represent main rendering color.
	// Only one FIELD_COLOR is valid per object.
	FIELD_COLOR,

	FIELD_MATRIX3x3,
	FIELD_MATRIX4x4,
};

enum valuetype_t : uint32_t
{
	VALUE_INVALID,

	VALUE_INT32,

	VALUE_FLOAT,
	VALUE_FLOAT2,
	VALUE_FLOAT3,
	VALUE_FLOAT4,

	VALUE_FLOAT3x3,
	VALUE_FLOAT4x4,
};

enum fielddisplay_t : uint32_t
{
	// Draw box as a display in editor.
	// The second argument is a string number that scales a 32x32x32 block.
	DISPLAY_BOX,
	DISPLAY_2D_SPRITE,
	DISPLAY_3D_MODEL,
	DISPLAY_LIGHT
};

enum metadataNamedKey_t : uint32_t
{
	METADATA_NONE				= 0,
	METADATA_DISPLAY_MODE		= 1,
	METADATA_DISPLAY_FILENAME	= 2,
};


//===============================================================================================//
// CLASSES / FUNCTIONS
//===============================================================================================//

namespace engine
{
	//===============================================================================================//
	// METADATA STORAGE BASE
	//===============================================================================================//

	class Metadata {};
	// Stored metadata
	template <typename VALUE_TYPE>
	class MetadataValue : public Metadata
	{
	public:
		MetadataValue ( const VALUE_TYPE& _source ) : source(_source) {;}
		VALUE_TYPE		source;
	};
	// Pointer to saved data to serialize
	class MetadataPData : public Metadata
	{
	public:
		MetadataPData ( void* _source, size_t _size ) : source_size((uint32_t)_size), source(_source) {;}
		uint32_t		source_size;
		void*			source;
	};
	template <typename VALUE_TYPE>
	class MetadataPValue : public MetadataPData
	{
	public:
		MetadataPValue ( VALUE_TYPE* _source, size_t _size ) : MetadataPData(_source, _size) {;}
	};
	// Stored metadata grabber
	template <typename VALUE_TYPE>
	const VALUE_TYPE& MetadataTo ( const Metadata* metadata )
	{
		return ((MetadataValue<VALUE_TYPE>*)(metadata))->source;
	}

	// Pair type
	typedef std::pair<arstring128,Metadata*> MetadataPair;
	typedef std::pair<uint32_t,MetadataPData*> MetadataPointerPair;
	typedef std::pair<uint32_t,valuetype_t> MetadataPointerTypePair;
	typedef std::pair<uint32_t,fieldtype_t> MetadataPointerFieldPair;
	typedef std::pair<uint32_t,arstring128> MetadataPointerNamePair;
	typedef std::pair<int,Metadata*> MetadataNamedPair;
	// Table type
	class MetadataTable
	{
	public:
		std::vector<MetadataPointerPair>		data;
		std::vector<MetadataPointerTypePair>	data_type;
		std::vector<MetadataPointerFieldPair>	data_field;
		std::vector<MetadataPointerNamePair>	data_name;
		std::vector<MetadataNamedPair>	named_info;
	public:
		const Metadata* Get ( const char* const key ) const
		{
			for ( size_t i = 0; i < data.size(); ++i ) {
				if ( data_name[i].second.compare(key) ) {
					return data[i].second;
				}
			}
			return NULL;
		}
		template <int SPECIAL_LISTING_INDEX>
		const Metadata* Get ( void ) const
		{
			for ( size_t i = 0; i < named_info.size(); ++i ) {
				if ( named_info[i].first == SPECIAL_LISTING_INDEX ) {
					return named_info[i].second;
				}
			}
			return NULL;
		}
	};

	template <class BEHAVIOR>
	const MetadataTable* const GetBehaviorMetadata ( void )
	{
		return NULL; // Return no table by default
	}

	//===============================================================================================//
	// SERIAL STORAGE BASE
	//===============================================================================================//

	class ObjectBase
	{
	public:
		virtual CGameBehavior* Instantiate ( void )=0;
		virtual const engine::MetadataTable* GetMetadata ( void )=0;
	};

	// == GLOBAL INSTANTIATION TEMPLATE ==
	// USED FOR CLASS REGISTRATION SYSTEM
	template<typename T> ObjectBase * _instObject( void ) { return new T; }

	//===============================================================================================//
	// REGISTRATION LISTING
	//===============================================================================================//

	class BehaviorList
	{
		// Singleton definition
		ARSINGLETON_H_STORAGE(BehaviorList,ENGINE_API)
		ARSINGLETON_H_ACCESS(BehaviorList)

	public:
		// Typedefs for readable registration
		typedef CGameBehavior* (*BehaviorInstFunction) (void);
		typedef ObjectBase* (*ObjectInstFunction) (void);
		typedef const MetadataTable* const (*GetMetadataFunction) (void);
		// Registration storage type
		struct Registration
		{
			BehaviorInstFunction	engine_inst;
			ObjectInstFunction		editor_inst;
			GetMetadataFunction		metadata;

			Registration() : engine_inst(NULL), editor_inst(NULL), metadata(NULL) {}
		};

	private:
		std::unordered_map<arstring128,Registration>	m_registry;

	public:
		//		class Registrar
		// Used to build the actual registration and lookup of class to instantiation
		template <class OBJECT, class BEHAVIOR>
		class Registrar
		{
		public:
			explicit Registrar ( const char* objectName )
			{
				BehaviorList* listing = BehaviorList::Active();
				Registration reg;
				reg.engine_inst = _instGameBehavior<BEHAVIOR>;
				reg.editor_inst = _instObject<OBJECT>;
				reg.metadata	= GetBehaviorMetadata<BEHAVIOR>;
				listing->m_registry[arstring128(objectName)] = reg;
			}
		};

		//		GetRegistration
		// Returns found registration for the given typename, or NULL if not found.
		static const Registration GetRegistration ( const char* objectName )
		{
			auto registry = Active()->m_registry;
			auto find_result = registry.find(arstring128(objectName));
			if ( find_result != registry.end() )
			{
				return find_result->second;
			}
			return Registration();
		}

	};

}

namespace Network
{
	ENGINE_API valuetype_t ValueTypeFromName ( const char* n_typename );
}


//===============================================================================================//
// MACROS
//===============================================================================================//

//		LINK_OBJECT_TO_CLASS
// Used once in the Header of the class. Builds the prototype for the metadata and extra shit.
#define LINK_OBJECT_TO_CLASS(behaviorname,cppclass) \
template<> const engine::MetadataTable* const engine::GetBehaviorMetadata<cppclass> ( void ) { return cppclass::__GetMetadataTable(); } \
class behaviorname : public ::engine::ObjectBase \
{ \
public: \
	CGameBehavior* Instantiate ( void ) override { \
		engine::BehaviorList::BehaviorInstFunction function = engine::BehaviorList::GetRegistration( #behaviorname ).engine_inst; \
		if ( function != NULL ) return function(); throw core::InvalidInstantiationException(); } \
	const engine::MetadataTable* GetMetadata ( void ) override { \
		return engine::GetBehaviorMetadata<cppclass>(); } \
	\
};
//		DECLARE_OBJECT_REGISTRAR
// Used once in the CPP file of the class. Creates a link between a behavior and a C++ class
#define DECLARE_OBJECT_REGISTRAR(behaviorname,cppclass) \
	LINK_OBJECT_TO_CLASS( behaviorname, cppclass ) \
	static ::engine::BehaviorList::Registrar< behaviorname , cppclass > __REGISTRAR_##behaviorname ( #behaviorname );


//		BEGIN_OBJECT_DESC
// Begins definition of a metadata table grab, to be used in the header
#define BEGIN_OBJECT_DESC(cppclass) \
	public: \
	static const engine::MetadataTable* const __GetMetadataTable ( void ) { \
	static engine::MetadataTable* table = NULL; if ( table == NULL ) { table = new engine::MetadataTable(); \
	typedef cppclass CPP_CLASS;
//		END_OBJECT_DESC
// End definition of a metadata table grab, to be used in the header
#define END_OBJECT_DESC() } return table; };
//		DEFINE_VALUE
// Defines a variable that is saved/loaded from disk
#define DEFINE_VALUE(variable,type,classification) \
	table->data.push_back(		engine::MetadataPointerPair(	  (uint32_t)offsetof(CPP_CLASS, variable), new engine::MetadataPValue<type>((type*)offsetof(CPP_CLASS, variable), sizeof(type)) )); \
	table->data_type.push_back(	engine::MetadataPointerTypePair(  (uint32_t)offsetof(CPP_CLASS, variable), Network::ValueTypeFromName( #type ) )); \
	table->data_field.push_back(engine::MetadataPointerFieldPair( (uint32_t)offsetof(CPP_CLASS, variable), classification )); \
	table->data_name.push_back(	engine::MetadataPointerNamePair(  (uint32_t)offsetof(CPP_CLASS, variable), #variable ));
//		DEFINE_KEYVALUE
// Defines a variable that is saved/loaded from disk and is visible in the editor
#define DEFINE_KEYVALUE(variable,type)
//		DEFINE_DISPLAY
// Defines a display mode, as well as a display file
#define DEFINE_DISPLAY(mode,file) \
	table->named_info.push_back(engine::MetadataNamedPair(METADATA_DISPLAY_MODE,new engine::MetadataValue<fielddisplay_t>( mode ))); \
	table->named_info.push_back(engine::MetadataNamedPair(METADATA_DISPLAY_FILENAME,new engine::MetadataValue<arstring128>( arstring128(file) )));

#endif//_BEHAVIOR_LIST_H_