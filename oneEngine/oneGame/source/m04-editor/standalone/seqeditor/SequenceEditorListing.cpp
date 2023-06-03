#include "SequenceEditor.h"

#include "core/debug/Console.h"

#include "core/utils/stringcase.h"

#include "core-ext/system/io/Resources.h"
#include "core-ext/containers/arStringEnum.h"
#include "core-ext/system/io/osf.h"

// NOTE: SEL's will be created by users, so none of these can error out.

static void LoadSettings ( m04::editor::SELInfo& sel, io::OSFReader& reader, io::OSFEntryInfo& entrySettings )
{
	if (entrySettings.type != io::kOSFEntryTypeObject)
	{
		debug::LogWarn("SEL: found settings structure but it was not an object!\n");
		return;
	}
}

static void LoadEnumType ( m04::editor::SELInfo& sel, io::OSFReader& reader, io::OSFEntryInfo& entryEnumtype )
{
	if (entryEnumtype.type != io::kOSFEntryTypeObject)
	{
		debug::LogWarn("SEL: found enumtype \"%s\" but it was not an object!\n", entryEnumtype.value.c_str());
		return;
	}
	debug::Log("SEL: found enumtype \"%s\".\n", entryEnumtype.value.c_str());

	std::string enumtypeName = entryEnumtype.value;
	core::utils::string::ToLower(enumtypeName);

	std::vector<arStringEnumDefinition::NameValue> nameValues;
	std::vector<arstring64> displayValues;

	reader.GoInto(entryEnumtype);
	
	io::OSFEntryInfo entry;
	reader.GetNext(entry);
	do
	{
		if (entry.type == io::kOSFEntryTypeNormal)
		{
			// The first value is the value of the enum entry
			int32_t value = atoi(entry.name);
					
			std::string parsed_nameEntry, parsed_readableEntry;
			std::string unparsed_nameEntry = entry.value;
			// Split on the first space
			auto unparsed_firstSpace = unparsed_nameEntry.find_first_of(core::utils::string::kWhitespace, 0);
			if (unparsed_firstSpace != string::npos)
			{	
				// If space, then we have a readable name
				parsed_nameEntry = unparsed_nameEntry.substr(0, unparsed_firstSpace);
				parsed_readableEntry = unparsed_nameEntry.substr(unparsed_firstSpace + 1);
				parsed_readableEntry = core::utils::string::FullTrim(parsed_readableEntry);
			}
			// No readable name yet?
			if (unparsed_firstSpace == string::npos || parsed_readableEntry.size() <= 0)
			{	
				parsed_nameEntry = unparsed_nameEntry;
				parsed_readableEntry = unparsed_nameEntry;
			}
			// Modify first level of the readable name to be capital.
			ARCORE_ASSERT(parsed_readableEntry.size() > 0);
			parsed_readableEntry[0] = ::toupper(parsed_readableEntry[0]);

			debug::Log("SEL: %d -> \"%s\", readable \"%s\".\n", value, parsed_nameEntry.c_str(), parsed_readableEntry.c_str());

			nameValues.push_back({parsed_nameEntry, value});
			displayValues.push_back(parsed_readableEntry.c_str());
		}

		reader.GetNext(entry);
	}
	while (entry.type != io::kOSFEntryTypeEnd);

	// Save new enumtype
	sel.enum_definitions[enumtypeName.c_str()] = new m04::editor::SequenceEnumDefinition(arStringEnumDefinition::CreateNew(nameValues), std::move(displayValues));
}

static void LoadNodeProperty ( m04::editor::SequenceNodePropertyDefinition* outProperty, io::OSFReader& reader, const io::OSFEntryInfo& parentNode );
static void LoadNodeArray ( m04::editor::SequenceNodePropertyDefinition* outProperty, io::OSFReader& reader, const io::OSFEntryInfo& parentNode );

static void LoadNodeProperty ( m04::editor::SequenceNodePropertyDefinition* outProperty, io::OSFReader& reader, const io::OSFEntryInfo& parentNode )
{
	if (parentNode.type != io::kOSFEntryTypeObject)
	{
		debug::LogWarn("SEL: attempted to call LoadNodeProperty on something that was not a property.\n");
		return;
	}

	outProperty->name = parentNode.name;

	reader.GoInto(parentNode);
	io::OSFEntryInfo entry;
	for (reader.GetNext(entry); entry.type != io::kOSFEntryTypeEnd; reader.GetNext(entry))
	{
		string_switch(entry.name)
		{
			string_case("displayAs"):
				outProperty->displayName = entry.value;
				break;
			string_case("type"):
				outProperty->type = m04::editor::StringToPropertyRenderStyle(entry.value);
				if (outProperty->type == m04::editor::PropertyRenderStyle::kArray)
				{
					LoadNodeArray(outProperty, reader, entry);
				}
				break;
			string_case("enum"):
				outProperty->enumName = entry.value;
				break;
			string_case("default"):
				outProperty->defaultValue = entry.value;
				break;
		}
	}
}

static void LoadNodeArray ( m04::editor::SequenceNodePropertyDefinition* outProperty, io::OSFReader& reader, const io::OSFEntryInfo& parentNode )
{
	if (parentNode.type != io::kOSFEntryTypeObject)
	{
		debug::LogWarn("SEL: attempted to call LoadNodeProperty on something that was not a property.\n");
		return;
	}

	if (outProperty->arraySubproperties == nullptr)
	{
		outProperty->AllocateSubproperties();
	}

	reader.GoInto(parentNode);
	io::OSFEntryInfo entry;
	for (reader.GetNext(entry); entry.type != io::kOSFEntryTypeEnd; reader.GetNext(entry))
	{
		if (entry.type == io::kOSFEntryTypeObject)
		{
			outProperty->arraySubproperties->push_back(m04::editor::SequenceNodePropertyDefinition{});
			LoadNodeProperty(&outProperty->arraySubproperties->back(), reader, entry);
		}
		else
		{
			debug::LogWarn("SEL:  -> unrecognized name \"%s\".\n", entry.name.c_str());
		}
	}
}

static void LoadNodeType ( m04::editor::SELInfo& sel, io::OSFReader& reader, const io::OSFEntryInfo& entryNodeType )
{
	if (entryNodeType.type != io::kOSFEntryTypeObject)
	{
		debug::LogWarn("SEL: found nodetype \"%s\" but it was not an object!\n", entryNodeType.value.c_str());
		return;
	}
	debug::Log("SEL: found nodetype \"%s\"\n", entryNodeType.value.c_str());

	m04::editor::SequenceNodeDefinition* node_definition = new m04::editor::SequenceNodeDefinition();

	reader.GoInto(entryNodeType);
	//io::OSFEntryInfo entry;
	//reader.GetNext(entry);
	
	io::OSFEntryInfo entry;
	for (reader.GetNext(entry); entry.type != io::kOSFEntryTypeEnd; reader.GetNext(entry))
	//do
	{
		if (entry.name.compare("displayAs"))
		{
			node_definition->displayName = entry.value;
		}
		else if (entry.name.compare("outputCount"))
		{
			long outputCount = strtol(entry.value, nullptr, 0); // TODO
			if (outputCount == LONG_MAX || outputCount == LONG_MIN)
			{
				outputCount = -1;
				debug::LogWarn("SEL:  -> issue reading outputCount: value \"%s\" is not a number.\n", entry.value.c_str());
			}
			// If it's negative, it's going to be adaptive value, so it's fine to read it directly.
			node_definition->outputCount = outputCount;
		}
		else if (entry.name.compare("type") && entry.value.compare("array"))
		{
			// We have a high-level array? This is basically a non-named array:
			node_definition->properties.push_back(m04::editor::SequenceNodePropertyDefinition{});
			LoadNodeArray(&node_definition->properties.back(), reader, entry);
		}
		else if (entry.type == io::kOSFEntryTypeObject)
		{
			node_definition->properties.push_back(m04::editor::SequenceNodePropertyDefinition{});
			LoadNodeProperty(&node_definition->properties.back(), reader, entry);
		}
		else
		{
			debug::LogWarn("SEL:  -> unrecognized name \"%s\".\n", entry.name.c_str());
		}

		//reader.GetNext(entry);
	}
	//while (entry.type != io::kOSFEntryTypeEnd);

	// Save new nodetype
	sel.node_definitions[entryNodeType.value] = node_definition;
}

void m04::editor::SELInfo::LoadSequenceEditorListing ( const char* sel_path )
{
	// first find the file via resources
	FILE* sel_fp = core::Resources::Open(sel_path, "rb");
	if (sel_fp == NULL)
	{
		throw core::InvalidArgumentException();
		return;
	}
	
	// create OSF for reading
	io::OSFReader reader (sel_fp);

	// the sel has enum types and node types.
	// for now we skip the node types because that's a pain to define properly

	// read in the osf entry-by-entry
	//io::OSFEntryInfo entry;
	//do
	io::OSFEntryInfo entry;
	for (reader.GetNext(entry); entry.type != io::kOSFEntryTypeEoF; reader.GetNext(entry))
	{
		//reader.GetNext(entry);

		string_switch(entry.name)
		{
			string_case("settings"):
				LoadSettings(*this, reader, entry);
				break;
			string_case("enumtype"):
				LoadEnumType(*this, reader, entry);
				break;
			string_case("nodetype"):
				LoadNodeType(*this, reader, entry);
				break;
		}
	}
	//while (entry.type != io::kOSFEntryTypeEoF);

	// close file
	fclose(sel_fp);
}

void m04::editor::SELInfo::Free ( void )
{
	for (auto enumDef : enum_definitions)
		delete enumDef.second;
	enum_definitions.clear();

	for (auto nodeDef : node_definitions)
	{
		for (auto nodeProperty : nodeDef.second->properties)
			nodeProperty.Free();
		delete nodeDef.second;
	}
	node_definitions.clear();
}


m04::editor::SequenceEnumDefinition::~SequenceEnumDefinition ( void )
{
	arStringEnumDefinition::Free(enumDefinition);
	enumDefinition = nullptr;
}