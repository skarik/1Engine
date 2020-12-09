#ifndef M04_IO_OSF_STRUCTURE_H_
#define M04_IO_OSF_STRUCTURE_H_

#include <vector>
#include <string>
#include "core/containers/arstring.h"
#include "core/debug.h"

#ifndef OSF_API 
#define OSF_API GAME_API
#endif

namespace io
{
	class OSFReader;
	class OSFWriter;
}

namespace osf
{
	enum class ValueType
	{
		// Value holds a vector of keyvalue
		kObject		= 0,
		// Value is a label
		kMarker		= 10,
		// Value holds a string
		kString		= 1,
		// Value holds an integer number
		kInteger	= 2,
		// Value holds a floating point number
		kFloat		= 3,
		// Value holds a value of boolean
		kBoolean	= 4,
	};

	class KeyValue;
	class BaseValue;

	class BaseValue
	{
	public:
		//	GetType() : Returns type of this base sequence.
		virtual ValueType		GetType ( void ) const =0;

		//	As<Type>() : Casts to the correct type.
		// Returns null if the cast is incorrect.
		template <class SequenceTypeCastTo>
		SequenceTypeCastTo*		As ( void )
		{
			return dynamic_cast<SequenceTypeCastTo*>(this);
		}
	};

	class ObjectValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kObject; }

		std::vector<KeyValue*>
							values;

		//	Add(kv) : Adds keyvalue to the object-value.
		OSF_API void			Add (KeyValue* kv)
		{
			values.push_back(kv);
		}

		//	Remove(kv) : Removes the given k-v from the listing.
		OSF_API void			Remove (KeyValue* kv)
		{
			for (auto kvItr = values.begin(); kvItr != values.end(); ++kvItr)
			{
				if (*kvItr == kv)
				{
					values.erase(kvItr);
					return;
				}
			}
			ARCORE_ERROR("Value was not in the array to be removed!");
		}

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		OSF_API KeyValue*		operator[] (const char* key_name);

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		OSF_API KeyValue*		operator[] (const arstring256& key_name)
			{ return operator[](key_name.c_str()); }

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		OSF_API KeyValue*		operator[] (const std::string& key_name)
			{ return operator[](key_name.c_str()); }
	};

	class MarkerValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kMarker; }

		std::string			value;
	};

	class StringValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kString; }

		std::string			value;
	};

	class IntegerValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kInteger; }

		int64_t				value;
	};

	class FloatValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kFloat; }

		float				value;
	};

	class BooleanValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kBoolean; }

		bool				value;
	};

	// Base type of object in the OSF structure.

	class KeyValue
	{
	public:
		// Name of the object
		arstring256			key;

		// The value attached to this key. If NULL, then is a value-less key.
		BaseValue*			value = NULL;
		// The object attached to this key. If NULL, then has no object.
		ObjectValue*		object = NULL;

	public:
		//	Default constructor
		KeyValue (void)
			{}

		//	Constructor helper
		KeyValue (const char* in_key, BaseValue* in_value = NULL, ObjectValue* in_object = NULL)
			: key(in_key)
			, value(in_value)
			, object(in_object)
			{}

		//	FreeKeyValues() : Recursively frees all keyvalues.
		OSF_API void			FreeKeyValues ( void );
	};

	class KeyValueTree
	{
		std::vector<KeyValue*>
							keyvalues;
	public:

		//	LoadKeyValues( reader ) : Loads entire OSF file in as a keyvalue structure.
		void					LoadKeyValues ( io::OSFReader* reader );

		//	FreeKeyValues() : Recursively frees all keyvalues.
		void					FreeKeyValues ( void )
		{
			for (KeyValue* kv : keyvalues)
			{
				kv->FreeKeyValues();
				delete kv;
			}
			keyvalues.clear();
		}

	};
};

#endif//M04_IO_OSF_STRUCTURE_H_