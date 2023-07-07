#ifndef CORE_EXT_IO_OSF_STRUCTURE_H_
#define CORE_EXT_IO_OSF_STRUCTURE_H_

#include <vector>
#include <string>
#include "core/containers/arstring.h"
#include "core/debug.h"

#ifndef OSF_API 
#define OSF_API CORE_API
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
		// Value is an array
		kArray		= 11,
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
	class ObjectValue;	

	//===============================================================================================//

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
		void				FreeKeyValues ( void );

		//	CanBeJSON() : Returns if can be represented by JSON. Must have a value XOR a object.
		bool				CanBeJSON ( void );
	};

	//===============================================================================================//

	// Base type all OSF value objects inherit from
	class BaseValue
	{
	public:
		virtual ~BaseValue ( void ) {}

		//	GetType() : Returns type of this base sequence.
		virtual ValueType		GetType ( void ) const =0;

		//	As<Type>() : Casts to the correct type.
		// Returns null if the cast is incorrect.
		template <class SequenceTypeCastTo>
		SequenceTypeCastTo*		As ( void );

		//	As<Type>() : Casts to the correct type.
		// Returns null if the cast is incorrect.
		template <class SequenceTypeCastTo>
		const SequenceTypeCastTo*
								As ( void ) const;
	};

	//===============================================================================================//

	// Defines the value type getters used for safe conversion.
#	define DEFINE_VALUE_TYPE(ValueTypeEnum) \
	virtual ValueType		GetType ( void ) const override \
		{ return ValueTypeEnum; } \
	static constexpr ValueType GetType_Static ( void ) \
		{ return ValueTypeEnum; }


	class MarkerValue : public BaseValue
	{
	public:
		DEFINE_VALUE_TYPE(ValueType::kMarker);

		std::string			value;
	};

	class StringValue : public BaseValue
	{
	public:
		DEFINE_VALUE_TYPE(ValueType::kString);

		std::string			value;
	};

	class IntegerValue : public BaseValue
	{
	public:
		DEFINE_VALUE_TYPE(ValueType::kInteger);

		int64_t				value;
	};

	class FloatValue : public BaseValue
	{
	public:
		DEFINE_VALUE_TYPE(ValueType::kFloat);

		float				value;
	};

	class BooleanValue : public BaseValue
	{
	public:
		DEFINE_VALUE_TYPE(ValueType::kBoolean);

		bool				value;
	};

	class ObjectValue : public BaseValue
	{
	public:
		DEFINE_VALUE_TYPE(ValueType::kObject);

		std::vector<KeyValue*>
							values;

		// Deconstructor frees all keyvalues in the values list.
		virtual					~ObjectValue ( void );

		//	Add(kv) : Adds keyvalue to the object-value.
		KeyValue*				Add (KeyValue* kv)
		{
			values.push_back(kv);
			return values.back();
		}

		//	Remove(kv) : Removes the given k-v from the listing.
		void					Remove (KeyValue* kv)
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

		//	GetAs<type>(string) : Looks up the given key-value and casts it. Returns nullptr if it does not exist.
		template <class SequenceTypeCastTo>
		SequenceTypeCastTo*		GetAs (const char* key_name);
		//	GetAdd<type>(string) : Looks up the given key-value and casts it. Adds it if it does not exist.
		template <class SequenceTypeCastTo>
		SequenceTypeCastTo*		GetAdd (const char* key_name);
		//	GetKeyValueAdd<type>(string) : Looks up the given key-value and returns it. Adds it if it does not exist.
		template <class SequenceTypeCastTo>
		osf::KeyValue*			GetKeyValueAdd (const char* key_name);
		//	GetConvertAdd<type>(string) : Looks up the given key-value and casts or converts it. Adds it if it does not exist.
		template <class SequenceTypeCastTo>
		SequenceTypeCastTo*		GetConvertAdd (const char* key_name)
		{
			GetAdd<SequenceTypeCastTo>(key_name);
			KeyValue* conversionResult = Convert<SequenceTypeCastTo>(key_name);
			return conversionResult ? conversionResult->value->As<SequenceTypeCastTo>() : nullptr;
		}
		//	Convert<type>(index) : Attempts to convert the given index to the correct type.
		// Returns null if the key was not found or conversion could not happen.
		template <class SequenceTypeCastTo>
		KeyValue*				Convert (const char* key_name);


		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		BaseValue*				operator[] (const char* key_name);
		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		BaseValue*				operator[] (const arstring256& key_name)
			{ return operator[](key_name.c_str()); }
		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		BaseValue*				operator[] (const std::string& key_name)
			{ return operator[](key_name.c_str()); }

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		const BaseValue*		operator[] (const char* key_name) const;
		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		const BaseValue*		operator[] (const arstring256& key_name) const
			{ return operator[](key_name.c_str()); }
		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		const BaseValue*		operator[] (const std::string& key_name) const
			{ return operator[](key_name.c_str()); }

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		KeyValue*				GetKeyValue (const char* key_name);
		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		KeyValue*				GetKeyValue (const arstring256& key_name)
			{ return GetKeyValue(key_name.c_str()); }
		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		KeyValue*				GetKeyValue (const std::string& key_name)
			{ return GetKeyValue(key_name.c_str()); }

	public:
		static constexpr size_t
							kNoIndex = (size_t)(-1);

	private:
		//	GetKeyIndex(string) : Looks up the given key-value and returns its index in the list.
		// Returns ::NoIndex if not found.
		size_t					GetKeyIndex (const char* key_name);

		//	CanConvertValue(sourceType, targetType) : Returns if the given value can be converted to the given type.
		static bool				CanConvertValue (ValueType sourceType, ValueType targetType);

		//	ConvertValue(source, target) : Converts the given value. Returns false if conversion failed.
		static bool				ConvertValue (const BaseValue* source, BaseValue* target);
	};

	class ArrayValue : public BaseValue
	{
	public:
		DEFINE_VALUE_TYPE(ValueType::kArray);

		std::vector<BaseValue*>
							values;

		//	operator[](index) : Looks up the given values in the array.
		BaseValue*				operator[] (const size_t index)
			{ return values[index]; }
		//	operator[](index) : Looks up the given values in the array.
		const BaseValue*		operator[] (const size_t index) const
			{ return values[index]; }
	};


#	undef DEFINE_VALUE_TYPE

	//===============================================================================================//

	class KeyValueTree : public ObjectValue
	{
	public:

		//	LoadKeyValues( reader ) : Loads entire OSF file in as a keyvalue structure.
		void				LoadKeyValues ( io::OSFReader* reader );

		//	SaveKeyValues( writer ) : Writes entire keyvalue tree to file.
		void				SaveKeyValues ( io::OSFWriter* writer );

		//	FreeKeyValues() : Recursively frees all keyvalues.
		void				FreeKeyValues ( void )
		{
			for (KeyValue* kv : values)
			{
				kv->FreeKeyValues();
				delete kv;
			}
			values.clear();
		}
	};

	//===============================================================================================//

	template <class SequenceTypeCastTo>
	SequenceTypeCastTo*		ObjectValue::GetAs (const char* key_name)
	{
		KeyValue* kvSource = GetKeyValue(key_name);

		// This is not a valid add.
		const bool isSame = std::is_same<SequenceTypeCastTo,osf::ObjectValue>::value;
		ARCORE_ASSERT(!isSame);

		// Need to add the item
		if (kvSource != nullptr)
		{
			return kvSource->value->As<SequenceTypeCastTo>();
		}
		else
		{
			return nullptr;
		}
	}

	template <class SequenceTypeCastTo>
	SequenceTypeCastTo*		ObjectValue::GetAdd (const char* key_name)
	{
		KeyValue* kvSource = GetKeyValue(key_name);

		// This is not a valid add.
		const bool isSame = std::is_same<SequenceTypeCastTo,osf::ObjectValue>::value;
		ARCORE_ASSERT(!isSame);

		// Need to add the item
		if (kvSource == nullptr)
		{
			kvSource = Add(new osf::KeyValue(key_name, new SequenceTypeCastTo()));
			return kvSource->value->As<SequenceTypeCastTo>();
		}
		else
		{
			return kvSource->value->As<SequenceTypeCastTo>();
		}
	}

	template <class SequenceTypeCastTo>
	osf::KeyValue*			ObjectValue::GetKeyValueAdd (const char* key_name)
	{
		KeyValue* kvSource = GetKeyValue(key_name);

		// Need to add the item
		if (kvSource == nullptr)
		{
			kvSource = Add(new osf::KeyValue(key_name, new SequenceTypeCastTo()));
			return kvSource;
		}
		else
		{
			return kvSource;
		}
	}

	template <class SequenceTypeCastTo>
	KeyValue*				ObjectValue::Convert (const char* key_name)
	{
		KeyValue* kvSource = GetKeyValue(key_name);

		// Cannot convert source objects
		if (kvSource == nullptr
			// We cannot convert objects.
			|| kvSource->value->GetType() == ValueType::kObject)
		{
			return nullptr;
		}
		// Let's create the new keyvalue type now.
		if (SequenceTypeCastTo::GetType_Static() == ValueType::kObject // Again, we cannot convert objects.
			// Cannot convert markers either.
			|| SequenceTypeCastTo::GetType_Static() == ValueType::kMarker)
		{
			return nullptr;
		}

		// Don't convert if the same type
		if (SequenceTypeCastTo::GetType_Static() == kvSource->value->GetType())
		{
			return kvSource;
		}

		// If cannot convert the value, don't try
		if (!CanConvertValue(kvSource->value->GetType(), SequenceTypeCastTo::GetType_Static()))
		{
			return false;
		}

		// Convert the value type now
		SequenceTypeCastTo* vTarget = new SequenceTypeCastTo;
		if (!ConvertValue(kvSource->value, vTarget))
		{
			delete vTarget;
			return nullptr;
		}

		// Now that we're done, save the old one.
		delete kvSource->value;
		kvSource->value = vTarget;

		return kvSource;
	}
};

#endif//CORE_EXT_IO_OSF_STRUCTURE_H_