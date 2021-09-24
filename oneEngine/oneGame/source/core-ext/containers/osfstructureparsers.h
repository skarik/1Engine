#ifndef CORE_EXT_IO_OSF_STRUCTURE_PARSERS_H_
#define CORE_EXT_IO_OSF_STRUCTURE_PARSERS_H_

#include "core/math/Math3d.h"
#include "core/containers/arstring.h"
#include "core/utils/string.h"

#include "core-ext/containers/osfstructure.h"

namespace osf
{
	void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, float& value )
	{
		auto read_keyvalue = object->Convert<osf::FloatValue>(keyvalue);
		if (read_keyvalue != NULL)
		{
			auto read_value = read_keyvalue->value->As<osf::FloatValue>();
			value = read_value->value;
		}
	}

	void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, bool& value )
	{
		auto read_keyvalue = object->Convert<osf::BooleanValue>(keyvalue);
		if (read_keyvalue != NULL)
		{
			auto read_value = read_keyvalue->value->As<osf::BooleanValue>();
			value = read_value->value;
		}
	}

	void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, arstring128& value )
	{
		auto read_keyvalue = object->GetKeyValue(keyvalue);
		if (read_keyvalue != NULL)
		{
			auto read_value = read_keyvalue->value->As<osf::StringValue>();
			value = read_value->value.c_str();
		}
	}
	
	void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, Vector3f& value )
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

	void ReadKeyValue ( osf::ObjectValue* object, const char* keyvalue, Vector4f& value )
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
}

#endif//CORE_EXT_IO_OSF_STRUCTURE_PARSERS_H_
