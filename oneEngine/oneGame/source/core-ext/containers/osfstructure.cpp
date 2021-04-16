#include "osfstructure.h"

#include "core-ext/system/io/osf.h"
#include "core/debug.h"
#include "core/utils/string.h"

#define SpecializeOSFCastTemplate(Class, ValueEnum) \
	template<> \
	Class* osf::BaseValue::As< Class > ( void ) \
	{ \
		if (this->GetType() == osf::ValueType::ValueEnum) \
		{ \
			return static_cast< Class* >(this); \
		} \
		else \
		{ \
			return NULL; \
		} \
	} \
	template<> \
	const Class* osf::BaseValue::As< Class > ( void ) const \
	{ \
		if (this->GetType() == osf::ValueType::ValueEnum) \
		{ \
			return static_cast< const Class* >(this); \
		} \
		else \
		{ \
			return NULL; \
		} \
	} \
	template Class* osf::BaseValue::As< Class > ( void ); \
	template const Class* osf::BaseValue::As< Class > ( void ) const; 

SpecializeOSFCastTemplate(osf::ObjectValue, kObject);
SpecializeOSFCastTemplate(osf::MarkerValue, kMarker);
SpecializeOSFCastTemplate(osf::StringValue, kString);
SpecializeOSFCastTemplate(osf::IntegerValue, kInteger);
SpecializeOSFCastTemplate(osf::FloatValue, kFloat);
SpecializeOSFCastTemplate(osf::BooleanValue, kBoolean);

#undef SpecializeOSFCastTemplate

osf::ObjectValue::~ObjectValue (void)
{
	for (KeyValue* kv : values)
	{
		delete kv;
	}
	values.empty();
}

osf::BaseValue* osf::ObjectValue::operator[] (const char* key_name)
{
	for (KeyValue* kv : values)
	{
		if (kv->key.compare(key_name))
		{
			return kv->value;
		}
	}
	return nullptr;
}

const osf::BaseValue* osf::ObjectValue::operator[] (const char* key_name) const
{
	for (KeyValue* kv : values)
	{
		if (kv->key.compare(key_name))
		{
			return kv->value;
		}
	}
	return nullptr;
}

//	operator[](string) : Looks up the given key-value. Returns NULL if not found.
osf::KeyValue* osf::ObjectValue::GetKeyValue (const char* key_name)
{
	for (KeyValue* kv : values)
	{
		if (kv->key.compare(key_name))
		{
			return kv;
		}
	}
	return nullptr;
}

size_t osf::ObjectValue::GetKeyIndex (const char* key_name)
{
	for (size_t kvIndex = 0; kvIndex < values.size(); ++kvIndex)
	{
		if (values[kvIndex]->key.compare(key_name))
		{
			return kvIndex;
		}
	}
	return kNoIndex;
}

bool osf::ObjectValue::ConvertValue (const BaseValue* source, BaseValue* target)
{
	// Convert the value type
	if (target->GetType() == ValueType::kString)
	{
		switch (source->GetType())
		{
		case ValueType::kFloat:
			{
				std::string& valueStringRef = target->As<StringValue>()->value;
				valueStringRef = std::to_string(source->As<FloatValue>()->value);
				// Remove trailing zeroes or decimal points.
				valueStringRef.erase(valueStringRef.find_last_not_of('0') + 1, std::string::npos);
				valueStringRef.erase(valueStringRef.find_last_not_of('.') + 1, std::string::npos); 
			}
			return true;
		case ValueType::kInteger:
			target->As<StringValue>()->value = std::to_string(source->As<IntegerValue>()->value);
			return true;
		case ValueType::kBoolean:
			target->As<StringValue>()->value = source->As<BooleanValue>()->value ? "true" : "false";
			return true;
		case ValueType::kString:
			target->As<StringValue>()->value = source->As<StringValue>()->value;
			return true;
		}
	}
	else if (target->GetType() == ValueType::kBoolean)
	{
		switch (source->GetType())
		{
		case ValueType::kInteger:
			target->As<BooleanValue>()->value = source->As<IntegerValue>()->value ? true : false;
			return true;
		case ValueType::kString:
			{
				std::string source_string = source->As<StringValue>()->value;
				core::utils::string::ToLower(source_string);
				try
				{
					if (source_string == "true" || source_string == "t"
						|| source_string == "yes" || source_string == "y"
						|| source_string == "enable" || source_string == "enabled"
						|| source_string == "on")
					{
						target->As<BooleanValue>()->value = true;
						return true;
					}
					else if (source_string == "false" || source_string == "f"
						|| source_string == "no" || source_string == "n"
						|| source_string == "disable" || source_string == "disabled"
						|| source_string == "off")
					{
						target->As<BooleanValue>()->value = true;
						return true;
					}
					else if (std::stoi(source_string) != 0)
					{
						target->As<BooleanValue>()->value = true;
						return true;
					}
					// Following case will only be hit when value is "0". Otherwise, will throw an std::invalid_argument.
					else
					{
						target->As<BooleanValue>()->value = false;
						return true;
					}
				}
				// Catch invalid cases for the std::stoi call.
				catch (std::invalid_argument&)
				{
					return false;
				}
			}
			return true;
		case ValueType::kBoolean:
			target->As<BooleanValue>()->value = source->As<BooleanValue>()->value;
			return true;
		}
	}
	else if (target->GetType() == ValueType::kInteger)
	{
		switch (source->GetType())
		{
		case ValueType::kInteger:
			target->As<IntegerValue>()->value = source->As<IntegerValue>()->value;
			return true;
		case ValueType::kBoolean:
			target->As<IntegerValue>()->value = source->As<BooleanValue>()->value ? 1 : 0;
			return true;
		case ValueType::kString:
			try
			{
				target->As<IntegerValue>()->value = std::stoi(source->As<StringValue>()->value);
			}
			catch (std::invalid_argument&)
			{
				return false;
			}
			return true;
		}
	}
	else if (target->GetType() == ValueType::kFloat)
	{
		switch (source->GetType())
		{
		case ValueType::kFloat:
			target->As<FloatValue>()->value = source->As<FloatValue>()->value;
			return true;
		case ValueType::kString:
			try
			{
				target->As<FloatValue>()->value = std::stof(source->As<StringValue>()->value);
			}
			catch (std::invalid_argument&)
			{
				return false;
			}
			return true;
		}
	}
	return false;
}

void osf::KeyValue::FreeKeyValues ( void )
{
	if (value != NULL)
	{
		/*if (value->GetType() == ValueType::kObject)
		{
			for (KeyValue* kv : value->As<ObjectValue>()->values)
			{
				kv->FreeKeyValues();
			}
			value->As<ObjectValue>()->values.clear();
		}*/
		delete value;
		value = NULL;
	}

	if (object != NULL)
	{
		/*if (object->GetType() == ValueType::kObject)
		{
			for (KeyValue* kv : object->As<ObjectValue>()->values)
			{
				kv->FreeKeyValues();
			}
			object->As<ObjectValue>()->values.clear();
		}*/
		delete object;
		object = NULL;
	}
}


void osf::KeyValueTree::LoadKeyValues( io::OSFReader* reader )
{
	ARCORE_ASSERT(reader != NULL);

	constexpr size_t kBufferSize = 1 << 14;
	char* l_buffer = new char [kBufferSize];

	io::OSFEntryInfo currentEntry;

	std::vector<osf::KeyValue*> stackParentKeyvalue;
	std::vector<io::OSFEntryInfo> stackParentEntries;

	do
	{
		reader->GetNext(currentEntry, l_buffer, kBufferSize);

		if (currentEntry.type != io::kOSFEntryTypeEnd
			&& currentEntry.type != io::kOSFEntryTypeEoF)
		{
			// Create the keyvalue
			KeyValue* kv = new KeyValue;

			// Add the kv to the current parent level we're at.
			if (stackParentKeyvalue.empty())
			{
				keyvalues.push_back(kv);
			}
			else
			{
				ARCORE_ASSERT(stackParentKeyvalue.back()->object != NULL)
				stackParentKeyvalue.back()->object->values.push_back(kv);
			}

			// Set the kv's values
			kv->key = currentEntry.name;
			if (currentEntry.type == io::kOSFEntryTypeNormal)
			{
				kv->value = new StringValue;
				kv->value->As<StringValue>()->value = currentEntry.value;
			}
			else if (currentEntry.type == io::kOSFEntryTypeMarker)
			{
				kv->value = new MarkerValue;
				kv->value->As<MarkerValue>()->value = currentEntry.value;
			}
			else if (currentEntry.type == io::kOSFEntryTypeSource)
			{
				kv->value = new StringValue;
				kv->value->As<StringValue>()->value = l_buffer;
			}
			else if (currentEntry.type == io::kOSFEntryTypeObject)
			{
				kv->value = new StringValue;
				kv->value->As<StringValue>()->value = currentEntry.value;
				kv->object = new ObjectValue;

				stackParentKeyvalue.push_back(kv);
				stackParentEntries.push_back(currentEntry);

				reader->GoInto(currentEntry);
			}
		}
		else if (currentEntry.type == io::kOSFEntryTypeEnd)
		{
			// Go up a level
			stackParentKeyvalue.pop_back();
			stackParentEntries.pop_back();
		}
	}
	while (currentEntry.type != io::kOSFEntryTypeEoF);

	delete[] l_buffer;
}