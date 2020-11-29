#ifndef M04_IO_OSF_STRUCTURE_H_
#define M04_IO_OSF_STRUCTURE_H_

#include <vector>
#include <string>
#include "core/containers/arstring.h"

namespace osf
{
	enum class ValueType
	{
		// Value holds a vector of keyvalue
		kObject		= 0,
		// Value is a label
		kLabel		= 10,
		// Value holds a string
		kString		= 1,
		// Value holds an integer number
		kInteger	= 2,
		// Value holds a floating point number
		kFloat		= 3,
		// Value holds a value of boolean
		kBoolean	= 4,
	};

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

		std::vector<BaseValue*>
							values;
	};

	class LabelValue : public BaseValue
	{
	public:
		virtual ValueType		GetType ( void ) const override
			{ return ValueType::kLabel; }
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
		// Name of the object
		arstring256			key;

		// The value attached to this key. If NULL, then is a value-less key.
		BaseValue*			value = NULL;
		// The object attached to this key. If NULL, then has no object.
		ObjectValue*		object = NULL;
	};
};

#endif//M04_IO_OSF_STRUCTURE_H_