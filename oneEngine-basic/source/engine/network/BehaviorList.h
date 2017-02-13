
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

namespace Engine
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
		MetadataPData ( void* _source, size_t _size ) : source_size(_size), source(_source) {;}
		size_t			source_size;
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
	typedef std::pair<int,Metadata*> MetadataSpecialPair;
	// Table type
	class MetadataTable
	{
	public:
		std::vector<MetadataPair>			data;
		std::vector<MetadataSpecialPair>	sp_data;
	public:
		const Metadata* Get ( const char* const key ) const
		{
			for ( size_t i = 0; i < data.size(); ++i ) {
				if ( data[i].first.compare(key) ) {
					return data[i].second;
				}
			}
			return NULL;
		}
		template <int SPECIAL_LISTING_INDEX>
		const Metadata* Get ( void ) const
		{
			for ( size_t i = 0; i < sp_data.size(); ++i ) {
				if ( sp_data[i].first == SPECIAL_LISTING_INDEX ) {
					return sp_data[i].second;
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
		virtual const Engine::MetadataTable* GetMetadata ( void )=0;
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
		// Registration storage type
		struct Registration
		{
			BehaviorInstFunction	engine_inst;
			ObjectInstFunction		editor_inst;

			Registration() : engine_inst(NULL), editor_inst(NULL) {}
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

//===============================================================================================//
// ENUMERATIONS
//===============================================================================================//

enum fieldtype_t : uint32_t
{
	FIELD_DEFAULT,
	FIELD_POSITION,
	FIELD_ROTATION,
	FIELD_SCALE,
	FIELD_COLOR,

	FIELD_MATRIX3x3,
	FIELD_MATRIX4x4,
};

enum fielddisplay_t : uint32_t
{
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
// MACROS
//===============================================================================================//

//		LINK_OBJECT_TO_CLASS
// Used once in the Header of the class. Builds the prototype for the metadata and extra shit.
#define LINK_OBJECT_TO_CLASS(behaviorname,cppclass) \
template<> const Engine::MetadataTable* const Engine::GetBehaviorMetadata<cppclass> ( void ) { return cppclass::__GetMetadataTable(); } \
class behaviorname : public ::Engine::ObjectBase \
{ \
public: \
	CGameBehavior* Instantiate ( void ) override { \
		Engine::BehaviorList::BehaviorInstFunction function = Engine::BehaviorList::GetRegistration( #behaviorname ).engine_inst; \
		if ( function != NULL ) return function(); throw Core::InvalidInstantiationException(); } \
	const Engine::MetadataTable* GetMetadata ( void ) override { \
		return Engine::GetBehaviorMetadata<cppclass>(); } \
	\
};
//		DECLARE_OBJECT_REGISTRAR
// Used once in the CPP file of the class. Creates a link between a behavior and a C++ class
#define DECLARE_OBJECT_REGISTRAR(behaviorname,cppclass) static ::Engine::BehaviorList::Registrar< behaviorname , cppclass > __REGISTRAR_##behaviorname ( #behaviorname );


//		BEGIN_OBJECT_DESC
// Begins definition of a metadata table grab, to be used in the header
#define BEGIN_OBJECT_DESC(cppclass) \
	public: \
	static const Engine::MetadataTable* const __GetMetadataTable ( void ) { \
	static Engine::MetadataTable* table = NULL; if ( table == NULL ) { table = new Engine::MetadataTable(); \
	typedef cppclass CPP_CLASS;
//		END_OBJECT_DESC
// End definition of a metadata table grab, to be used in the header
#define END_OBJECT_DESC() } return table; };
//		DEFINE_VALUE
// Defines a variable that is saved/loaded from disk
#define DEFINE_VALUE(variable,type,classification) \
	table->data.push_back(Engine::MetadataPair( #variable, new Engine::MetadataPValue<type>((type*)offsetof(CPP_CLASS, variable), sizeof(type)) ));
//		DEFINE_KEYVALUE
// Defines a variable that is saved/loaded from disk and is visible in the editor
#define DEFINE_KEYVALUE(variable,type)
//		DEFINE_DISPLAY
// Defines a display mode, as well as a display file
#define DEFINE_DISPLAY(mode,file) \
	table->sp_data.push_back(Engine::MetadataSpecialPair(METADATA_DISPLAY_MODE,new Engine::MetadataValue<fielddisplay_t>( mode ))); \
	table->sp_data.push_back(Engine::MetadataSpecialPair(METADATA_DISPLAY_FILENAME,new Engine::MetadataValue<arstring128>( arstring128(file) )));

#endif//_BEHAVIOR_LIST_H_