#ifndef M04_IO_OSF_STRUCTURE_H_
#define M04_IO_OSF_STRUCTURE_H_

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
		SequenceTypeCastTo*
								As ( void );
		/*{
			return dynamic_cast<SequenceTypeCastTo*>(this);
		}*/

		//	As<Type>() : Casts to the correct type.
		// Returns null if the cast is incorrect.
		template <class SequenceTypeCastTo>
		const SequenceTypeCastTo*
								As ( void ) const;
		/*{
			return dynamic_cast<const SequenceTypeCastTo*>(this);
		}*/
	};

	class ObjectValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kObject; }

		std::vector<KeyValue*>
							values;

		// Deconstructor frees all keyvalues in the values list.
		~ObjectValue ( void );

		//	Add(kv) : Adds keyvalue to the object-value.
		/*OSF_API*/ KeyValue*	Add (KeyValue* kv)
		{
			values.push_back(kv);
			return values.back();
		}

		//	Remove(kv) : Removes the given k-v from the listing.
		/*OSF_API*/ void		Remove (KeyValue* kv)
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

		//	GetAdd<type>(string) : Looks up the given key-value and casts it. Adds it if it does not exist.
		template <class SequenceTypeCastTo>
		/*OSF_API*/ SequenceTypeCastTo*
								GetAdd (const char* key_name)
		{
			KeyValue* kvSource = GetKeyValue(key_name);

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

		//	GetConvertAdd<type>(string) : Looks up the given key-value and casts or converts it. Adds it if it does not exist.
		template <class SequenceTypeCastTo>
		/*OSF_API*/ SequenceTypeCastTo*
								GetConvertAdd (const char* key_name)
		{
			GetAdd<SequenceTypeCastTo>(key_name);
			return Convert<SequenceTypeCastTo>(key_name)->value->As<SequenceTypeCastTo>();
		}

		//	Convert<type>(index) : Attempts to convert the given index to the correct type.
		// Returns null if the key was not found or conversion could not happen.
		template <class SequenceTypeCastTo>
		/*OSF_API*/ KeyValue*	Convert (const char* key_name)
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

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		/*OSF_API*/ BaseValue*	operator[] (const char* key_name);

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		/*OSF_API*/ BaseValue*	operator[] (const arstring256& key_name)
			{ return operator[](key_name.c_str()); }

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		/*OSF_API*/ BaseValue*	operator[] (const std::string& key_name)
			{ return operator[](key_name.c_str()); }

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		/*OSF_API*/ const BaseValue*
								operator[] (const char* key_name) const;

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		/*OSF_API*/ const BaseValue*
								operator[] (const arstring256& key_name) const
			{ return operator[](key_name.c_str()); }

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		/*OSF_API*/ const BaseValue*
								operator[] (const std::string& key_name) const
			{ return operator[](key_name.c_str()); }

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		/*OSF_API*/ KeyValue*	GetKeyValue (const char* key_name);

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		/*OSF_API*/ KeyValue*	GetKeyValue (const arstring256& key_name)
			{ return GetKeyValue(key_name.c_str()); }

		//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
		/*OSF_API*/ KeyValue*	GetKeyValue (const std::string& key_name)
			{ return GetKeyValue(key_name.c_str()); }

	public:
		static constexpr size_t
							kNoIndex = (size_t)(-1);

	private:
		//	GetKeyIndex(string) : Looks up the given key-value and returns its index in the list.
		// Returns ::NoIndex if not found.
		/*OSF_API*/ size_t		GetKeyIndex (const char* key_name);

		//	ConvertValue(source, target) : Converts the given value. Returns false if conversion failed.
		/*OSF_API*/ bool		ConvertValue (const BaseValue* source, BaseValue* target);
	};

	class MarkerValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kMarker; }
		static constexpr ValueType
								GetType_Static ( void )
			{ return ValueType::kMarker; }

		std::string			value;
	};

	class StringValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kString; }
		static constexpr ValueType
								GetType_Static ( void )
			{ return ValueType::kString; }

		std::string			value;
	};

	class IntegerValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kInteger; }
		static constexpr ValueType
								GetType_Static ( void )
			{ return ValueType::kInteger; }

		int64_t				value;
	};

	class FloatValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kFloat; }
		static constexpr ValueType
								GetType_Static ( void )
			{ return ValueType::kFloat; }

		float				value;
	};

	class BooleanValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kBoolean; }
		static constexpr ValueType
								GetType_Static ( void )
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