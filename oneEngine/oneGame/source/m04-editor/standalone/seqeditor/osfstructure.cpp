#include "osfstructure.h"

#include "core-ext/system/io/osf.h"
#include "core/debug.h"

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
	}

SpecializeOSFCastTemplate(osf::ObjectValue, kObject);
SpecializeOSFCastTemplate(osf::MarkerValue, kMarker);
SpecializeOSFCastTemplate(osf::StringValue, kString);
SpecializeOSFCastTemplate(osf::IntegerValue, kInteger);
SpecializeOSFCastTemplate(osf::FloatValue, kFloat);
SpecializeOSFCastTemplate(osf::BooleanValue, kBoolean);

#undef SpecializeOSFCastTemplate


osf::KeyValue* osf::ObjectValue::operator[] (const char* key_name)
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


void osf::KeyValue::FreeKeyValues ( void )
{
	if (value != NULL)
	{
		if (value->GetType() == ValueType::kObject)
		{
			for (KeyValue* kv : value->As<ObjectValue>()->values)
			{
				kv->FreeKeyValues();
			}
			value->As<ObjectValue>()->values.clear();
		}
		delete value;
		value = NULL;
	}

	if (object != NULL)
	{
		if (object->GetType() == ValueType::kObject)
		{
			for (KeyValue* kv : object->As<ObjectValue>()->values)
			{
				kv->FreeKeyValues();
			}
			object->As<ObjectValue>()->values.clear();
		}
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