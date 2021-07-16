#include "PersistentSettings.h"

#include "core/settings/CGameSettings.h"

#include "core-ext/core-ext.h"
#include "core-ext/system/services/Profiles.h"

#include "core-ext/system/io/osf.h"
#include "core-ext/containers/osfstructure.h"

core::settings::StringSettingGroup* core::settings::Persistent::Global;
core::settings::StringSettingGroup* core::settings::Persistent::User;
core::settings::Persistent::PersistentState* core::settings::Persistent::g_state;

static const char* kPersistentFilename = ".game/persistent.cfg";
static const char* kSectionNameGlobal = "~!GLOBAL";

void core::settings::Persistent::PrepareForSettings ( void )
{
	static bool g_loaded = false;

	if (!g_loaded)
	{
		g_state = new PersistentState;
		Global = new StringSettingGroup;
		User = new StringSettingGroup;

		g_loaded = true;
	}
}

static void LoadPersistentGroup ( core::settings::StringSettingGroup& group, const osf::ObjectValue* osf_object )
{
	ARCORE_ASSERT(osf_object != nullptr);

	for (auto variablePair : group.GetSettings())
	{
		if (variablePair.second == nullptr) continue;

		auto variableEntry = (*osf_object)[variablePair.first.c_str()];
		if (variableEntry != nullptr)
		{
			const osf::StringValue* stringEntry = variableEntry->As<osf::StringValue>();
			ARCORE_ASSERT(stringEntry != nullptr); // It MUST be a string entry. Anything else is invalid.
			variablePair.second->SetFromString(stringEntry->value.c_str());
		}
	}
}

static void SavePersistentGroup ( const core::settings::StringSettingGroup& group, osf::ObjectValue* osf_object )
{
	ARCORE_ASSERT(osf_object != nullptr);

	for (auto variablePair : group.GetSettingsReadOnly())
	{
		if (variablePair.second == nullptr) continue;

		osf::StringValue* stringEntry = osf_object->GetAdd<osf::StringValue>(variablePair.first.c_str());
		ARCORE_ASSERT(stringEntry != nullptr);
		// Convert & save the value.
		stringEntry->value = variablePair.second->ToString();
	}
}

void core::settings::Persistent::Load ( void )
{
	// We need to open and read in the entire file, so that on writing, sections don't get deleted.
	const std::string userProfileName = core::os::profiles::GetCurrentProfileName();
	printf(userProfileName.c_str());

	CGameSettings::Active()->MakeDirectory( ".game" );
	FILE* fp = fopen(kPersistentFilename, "rb");
	if (fp)
	{
		io::OSFReader reader(fp);
		if (g_state->all_kv_data == nullptr)
		{
			g_state->all_kv_data = new osf::KeyValueTree;
		}
		g_state->all_kv_data->LoadKeyValues(&reader);
		fclose(fp);
	}

	if (g_state->all_kv_data != nullptr)
	{
		auto& kvData = *g_state->all_kv_data;
		
		auto globalData = kvData.GetKeyValue(kSectionNameGlobal);
		if (globalData != nullptr)
		{
			auto dataAsOsfObject = globalData->object;
			if (dataAsOsfObject != nullptr)
			{
				LoadPersistentGroup(*Global, dataAsOsfObject);
			}
		}

		auto userData = kvData.GetKeyValue(userProfileName.c_str());
		if (userData != nullptr)
		{
			auto dataAsOsfObject = userData->object;
			if (dataAsOsfObject != nullptr)
			{
				LoadPersistentGroup(*User, dataAsOsfObject);
			}
		}
	}
}

void core::settings::Persistent::Save ( void )
{
	const std::string userProfileName = core::os::profiles::GetCurrentProfileName();

	// Create kv tree if it doesn't yet exist.
	if (g_state->all_kv_data == nullptr)
	{
		g_state->all_kv_data = new osf::KeyValueTree;
	}

	// Update the OSF tree for each bit of data:
	auto& kvData = *g_state->all_kv_data;

	auto globalData = kvData.GetKeyValueAdd<osf::StringValue>(kSectionNameGlobal);
	if (globalData->object == nullptr) globalData->object = new osf::ObjectValue();
	SavePersistentGroup(*Global, globalData->object);

	auto userData = kvData.GetKeyValueAdd<osf::StringValue>(userProfileName.c_str());
	if (userData->object == nullptr) userData->object = new osf::ObjectValue();
	SavePersistentGroup(*User, userData->object);

	// Save the entire loaded kv tree to file
	CGameSettings::Active()->MakeDirectory( ".game" );
	FILE* fp = fopen(kPersistentFilename, "wb");
	if (fp)
	{
		io::OSFWriter writer(fp);
		g_state->all_kv_data->SaveKeyValues(&writer);
		fclose(fp);
	}
}

void core::settings::Persistent::CleanUp ( void )
{
	if (g_state->all_kv_data != nullptr)
	{
		delete g_state->all_kv_data;
		g_state->all_kv_data = nullptr;
	}

	delete g_state;
	delete Global;
	delete User;
}

static void LocalLoadSettings ( void )
{ 
	core::settings::Persistent::PrepareForSettings();
	core::settings::Persistent::Load();
}
REGISTER_ON_APPLICATION_STARTUP(LocalLoadSettings);

static void LocalSaveSettings ( void )
{
	core::settings::Persistent::Save();
	core::settings::Persistent::CleanUp();
}
REGISTER_ON_APPLICATION_END(LocalSaveSettings);