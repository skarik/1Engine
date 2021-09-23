#include "ArMaterial.h"

#include "core/debug/console.h"

#include "core/utils/string.h"
#include "core-ext/system/io/Resources.h"
#include "core-ext/resources/ResourceManager.h"

#include "core-ext/system/io/osf.h"
#include "core-ext/containers/osfstructure.h"

static void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, PrPhysMaterialType& value )
{
	auto read_keyvalue = object->GetKeyValue(keyvalue);
	if (read_keyvalue != NULL)
	{
		auto read_value = read_keyvalue->value->As<osf::StringValue>();
		value = PrGetPhysMaterialTypeFromName(read_value->value.c_str());
	}
}

static void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, float& value )
{
	auto read_keyvalue = object->Convert<osf::FloatValue>(keyvalue);
	if (read_keyvalue != NULL)
	{
		auto read_value = read_keyvalue->value->As<osf::FloatValue>();
		value = read_value->value;
	}
}

static void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, bool& value )
{
	auto read_keyvalue = object->Convert<osf::BooleanValue>(keyvalue);
	if (read_keyvalue != NULL)
	{
		auto read_value = read_keyvalue->value->As<osf::BooleanValue>();
		value = read_value->value;
	}
}

static void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, arstring128& value )
{
	auto read_keyvalue = object->GetKeyValue(keyvalue);
	if (read_keyvalue != NULL)
	{
		auto read_value = read_keyvalue->value->As<osf::StringValue>();
		value = read_value->value.c_str();
	}
}

static void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, ArRenderMode& value )
{
	auto read_keyvalue = object->GetKeyValue(keyvalue);
	if (read_keyvalue != NULL)
	{
		auto read_value = read_keyvalue->value->As<osf::StringValue>();

			 if (read_value->value == "LitOpaque")		value = ArRenderMode::kLitOpaque;
		else if (read_value->value == "FastDecal")		value = ArRenderMode::kFastDecal;
		else if (read_value->value == "LitFoliage")		value = ArRenderMode::kLitFoliage;
		else
			ARCORE_ERROR("Unrecognized key-value");
	}
}

static void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, ArFacingCullMode& value )
{
	auto read_keyvalue = object->GetKeyValue(keyvalue);
	if (read_keyvalue != NULL)
	{
		auto read_value = read_keyvalue->value->As<osf::StringValue>();

			 if (read_value->value == "None")		value = ArFacingCullMode::kNone;
		else if (read_value->value == "Front")		value = ArFacingCullMode::kFront;
		else if (read_value->value == "Back")		value = ArFacingCullMode::kBack;
		else
			ARCORE_ERROR("Unrecognized key-value");
	}
}

static void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, core::gfx::tex::arSamplingFilter& value )
{
	auto read_keyvalue = object->GetKeyValue(keyvalue);
	if (read_keyvalue != NULL)
	{
		auto read_value = read_keyvalue->value->As<osf::StringValue>();

			 if (read_value->value == "Point")		value = core::gfx::tex::kSamplingPoint;
		else if (read_value->value == "Nearest")	value = core::gfx::tex::kSamplingPoint;
		else if (read_value->value == "Linear")		value = core::gfx::tex::kSamplingLinear;
		else
			ARCORE_ERROR("Unrecognized key-value");
	}
}

static void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, ArMaterial::RenderInfo::TextureEntry& value )
{
	auto read_keyvalue = object->GetKeyValue(keyvalue);
	if (read_keyvalue != NULL)
	{
		auto read_value = read_keyvalue->value->As<osf::StringValue>();
		value.resource_name = read_value->value.c_str();
	}
}

static void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, Vector3f& value )
{
	auto read_keyvalue = object->GetKeyValue(keyvalue);
	if (read_keyvalue != NULL)
	{
		auto read_value = read_keyvalue->value->As<osf::StringValue>();
		std::string set_string = read_value->value;
		auto set_values = core::utils::string::Split(set_string, core::utils::string::kWhitespace);

		for (int i = 0; i < std::min<int>(3, (int)set_values.size()); ++i)
		{
			value[i] = std::stof(set_values[i]);
		}
	}
}

static void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, Vector4f& value )
{
	auto read_keyvalue = object->GetKeyValue(keyvalue);
	if (read_keyvalue != NULL)
	{
		auto read_value = read_keyvalue->value->As<osf::StringValue>();
		std::string set_string = read_value->value;
		auto set_values = core::utils::string::Split(set_string, core::utils::string::kWhitespace);

		for (int i = 0; i < std::min<int>(4, (int)set_values.size()); ++i)
		{
			value[i] = std::stof(set_values[i]);
		}
	}
}


static bool LoadMaterialFromResource ( ArMaterial* material, const char* resource_name )
{
	// Create the filename we need to look for
	arstring256 mat_resource_name = resource_name;
	mat_resource_name += ".at";

	std::string mat_filename;
	if (!core::Resources::MakePathTo(mat_resource_name.c_str(), mat_filename))
	{
		// Couldn't find the material file.
		debug::Console->PrintError( "LoadMaterialFromResource : Could not find the resource \"%s\"\n", mat_resource_name );
		return false;
	}
	else
	{
		FILE* osf_file = fopen( mat_filename.c_str(), "rb" );
		ARCORE_ASSERT(osf_file != NULL);

		// Load in the OSF file
		osf::KeyValueTree material_keys;
		{
			io::OSFReader osf_reader ( osf_file );
			material_keys.LoadKeyValues(&osf_reader);
		}
		fclose(osf_file);

		// Iterate through the level 1 keys:
		for (const auto& kv : material_keys.values)
		{
			if (kv->key.compare("name"))
			{
				material->name = kv->value->As<osf::StringValue>()->value.c_str();
			}
			else if (kv->key.compare("physics"))
			{
				auto contents = kv->object->As<osf::ObjectValue>();

				ReadKeyValue(contents, "physmat", material->physMat);
			}
			else if (kv->key.compare("render"))
			{
				auto contents = kv->object->As<osf::ObjectValue>();

				ReadKeyValue(contents, "render_mode", material->render_info.render_mode);

				ReadKeyValue(contents, "shader_vv", material->render_info.shader_vv);
				ReadKeyValue(contents, "shader_h", material->render_info.shader_h);
				ReadKeyValue(contents, "shader_d", material->render_info.shader_d);
				ReadKeyValue(contents, "shader_g", material->render_info.shader_g);
				ReadKeyValue(contents, "shader_p", material->render_info.shader_p);

				ReadKeyValue(contents, "cull", material->render_info.cull);
				ReadKeyValue(contents, "alpha_test", material->render_info.alpha_test);

				ReadKeyValue(contents, "diffuse_color", material->render_info.diffuse_color);
				ReadKeyValue(contents, "smoothness_bias", material->render_info.smoothness_bias);
				ReadKeyValue(contents, "smoothness_scale", material->render_info.smoothness_scale);
				ReadKeyValue(contents, "metallicness_bias", material->render_info.metallicness_bias);
				ReadKeyValue(contents, "metallicness_scale", material->render_info.metallicness_scale);

				ReadKeyValue(contents, "texture_diffuse", material->render_info.texture_diffuse);
				ReadKeyValue(contents, "texture_normals", material->render_info.texture_normals);
				ReadKeyValue(contents, "texture_surface", material->render_info.texture_surface);
				ReadKeyValue(contents, "texture_overlay", material->render_info.texture_overlay);
				ReadKeyValue(contents, "texture_detail", material->render_info.texture_detail);

				ReadKeyValue(contents, "sampling", material->render_info.sampling);

				ReadKeyValue(contents, "repeat_factor", material->render_info.repeat_factor);

			}
			else if (kv->key.compare("lighting"))
			{
				auto contents = kv->object->As<osf::ObjectValue>();

				ReadKeyValue(contents, "emits", material->lighting_info.emits);
				ReadKeyValue(contents, "emissive", material->lighting_info.emissive);
			}
			else
			{
				ARCORE_ERROR("Unknown material key type \"%s\"", kv->key.c_str());
			}
		}

		// Done with keys, free the tree
		material_keys.FreeKeyValues();
	}

	return true;
}

ArMaterialContainer* ArLoadMaterial ( const char* resource_name )
{
	// Early exit out empty material definitions.
	if (resource_name == NULL)
	{
		return NULL;
	}

	auto resm = core::ArResourceManager::Active();

	// Generate the resource name from the filename:
	arstring256 resource_str_id (resource_name);
	core::utils::string::ToResourceName(resource_str_id);

	// First, find the model in the resource system:
	IArResource* existingResource = resm->Find(core::kResourceTypeArMaterial, resource_str_id);
	if (existingResource != NULL)
	{
		// Found it! Add a reference and return it.
		ArMaterialContainer* existingContainer = (ArMaterialContainer*)existingResource;
		existingContainer->AddReference();
		return existingContainer;
	}
	else
	{
		// Load in the material
		ArMaterial* material = new ArMaterial;
		{
			bool loadResult = LoadMaterialFromResource(material, resource_name);
			ARCORE_ASSERT(loadResult);
		}

		// Create a container that holds the information:
		ArMaterialContainer* container = new ArMaterialContainer(material, resource_str_id);

		// Add self to the resource system:
		if (resm->Contains(container) == false)
			resm->Add(container);

		return container;
	}
}

ArMaterialContainer* ArFindMaterial ( const char* resource_name )
{
	auto resm = core::ArResourceManager::Active();

	// Generate the resource name from the filename:
	arstring256 resource_str_id (resource_name);
	core::utils::string::ToResourceName(resource_str_id);

	// Find the texture in the resource system:
	IArResource* existingResource = resm->Find(core::kResourceTypeArMaterial, resource_str_id);
	if (existingResource != NULL)
	{
		// Found it! Add a reference and return it.
		ArMaterialContainer* existingContainer = (ArMaterialContainer*)existingResource;
		return existingContainer;
	}

	// Otherwise, didn't find anything.
	return NULL;
}