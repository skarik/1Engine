#include "SequenceSerialization.h"

m04::editor::sequence::OsfSerializer::OsfSerializer ( const char* filename )
{
	osf_fileHandle = fopen(filename, "wb");
	if (osf_fileHandle != NULL)
	{
		osf_writer = new io::OSFWriter(osf_fileHandle);
	}
}
m04::editor::sequence::OsfSerializer::~OsfSerializer ( void )
{
	delete osf_writer;
	osf_writer = NULL;
	fclose(osf_fileHandle);
}

bool m04::editor::sequence::OsfSerializer::IsValid ( void )
{
	return osf_writer != NULL;
}

void m04::editor::sequence::OsfSerializer::SerializeFileBegin ( void ) 
{
	// Write the type of file at the top
	fputs("//!/osf/seq/2\n", osf_fileHandle);
}

void m04::editor::sequence::OsfSerializer::SerializeListBegin ( void )
{
	// Nothing.
}

static void WriteObjectEntry (io::OSFWriter* osf_writer, const osf::ObjectValue* object)
{
	for (osf::KeyValue* kv : object->values)
	{
		io::OSFEntryInfo kvEntry = {io::kOSFEntryTypeNormal, kv->key};

		switch (kv->value->GetType())
		{
		case osf::ValueType::kObject:
			ARCORE_ASSERT(kv->value == kv->object);
			break;

		case osf::ValueType::kString:
		case osf::ValueType::kMarker:
			kvEntry.value = kv->value->As<osf::StringValue>()->value.c_str();
			break;

		case osf::ValueType::kBoolean:
			kvEntry.value = kv->value->As<osf::BooleanValue>()->value ? "true" : "false";
			break;

		case osf::ValueType::kInteger:
			kvEntry.value = std::to_string(kv->value->As<osf::IntegerValue>()->value).c_str();
			break;

		case osf::ValueType::kFloat:
			kvEntry.value = std::to_string(kv->value->As<osf::FloatValue>()->value).c_str();
			break;

		default:
			ARCORE_ERROR("Not yet implemented.");
			break;
		}

		if (kv->object == NULL)
		{
			kvEntry.type = io::kOSFEntryTypeNormal;
			osf_writer->WriteEntry(kvEntry);
		}
		else
		{
			kvEntry.type = io::kOSFEntryTypeObject;
			osf_writer->WriteObjectBegin(kvEntry);
			WriteObjectEntry(osf_writer, kv->object);
			osf_writer->WriteObjectEnd();
		}
	}
}

void m04::editor::sequence::OsfSerializer::SerializeStartpoint ( const m04::editor::SequenceNode* node )
{
	io::OSFEntryInfo startpointEntry;
	startpointEntry.type = io::kOSFEntryTypeObject;
	startpointEntry.name = node->view->classname;
	osf_writer->WriteObjectBegin(startpointEntry);
	{
		io::OSFEntryInfo guidEntry = {io::kOSFEntryTypeNormal, "goto", node->next->data["guid"]->As<osf::StringValue>()->value.c_str()};
		osf_writer->WriteEntry(guidEntry);

		// Write the GUID and the Editor information.
		WriteObjectEntry(osf_writer, &node->data); // TODO: Limit this to the editor information.
	}
	osf_writer->WriteObjectEnd();
}

void m04::editor::sequence::OsfSerializer::SerializeJumptarget ( const m04::editor::SequenceNode* node )
{
	// Create a jump target label
	io::OSFEntryInfo marker = {io::kOSFEntryTypeMarker, node->data["guid"]->As<osf::StringValue>()->value.c_str()};
	osf_writer->WriteEntry(marker);
}

void m04::editor::sequence::OsfSerializer::SerializeNode ( const m04::editor::SequenceNode* node )
{
	io::OSFEntryInfo object = {io::kOSFEntryTypeObject, node->view->classname};
	osf_writer->WriteObjectBegin(object);
	{
		WriteObjectEntry(osf_writer, &node->data);
	}
	osf_writer->WriteObjectEnd();
}
void m04::editor::sequence::OsfSerializer::SerializeListEnd ( const m04::editor::SequenceNode* lastNode )
{
	if (lastNode == NULL || lastNode->next == NULL)
	{
		// Create a "goto end" object.
		io::OSFEntryInfo entry;
		entry.type = io::kOSFEntryTypeNormal;
		entry.name = "goto";
		entry.value = "__END";
		osf_writer->WriteEntry(entry);
	}
	else
	{
		// Create a "goto {NODE GUID}" object.
		io::OSFEntryInfo entry;
		entry.type = io::kOSFEntryTypeNormal;
		entry.name = "goto";
		entry.value = lastNode->next->data["guid"]->As<osf::StringValue>()->value.c_str();
		osf_writer->WriteEntry(entry);
	}
}
void m04::editor::sequence::OsfSerializer::SerializeFileEnd ( void )
{
	// Create an "end" label
	io::OSFEntryInfo entry;
	entry.type = io::kOSFEntryTypeMarker;
	entry.name = "#__END";
	entry.value = "";
	osf_writer->WriteEntry(entry);
}