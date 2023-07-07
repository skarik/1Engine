#include "SequenceNode.h"

namespace m04 {
namespace editor {
namespace sequence {

	void properties::SetProperty ( osf::ObjectValue* data, const char* stringIndex, const float value )
	{
		data->GetConvertAdd<osf::FloatValue>(stringIndex)->value = value;
	}

	void properties::SetProperty ( osf::ObjectValue* data, const char* stringIndex, const int value )
	{
		data->GetConvertAdd<osf::IntegerValue>(stringIndex)->value = value;
	}

	void properties::SetProperty ( osf::ObjectValue* data, const char* stringIndex, const bool value )
	{
		data->GetConvertAdd<osf::BooleanValue>(stringIndex)->value = value;
	}

	void properties::SetProperty ( osf::ObjectValue* data, const char* stringIndex, const char* value )
	{
		data->GetConvertAdd<osf::StringValue>(stringIndex)->value = value;
	}

	void properties::SetProperty ( osf::ObjectValue* data, const char* stringIndex, const Vector2f value )
	{
		data->GetConvertAdd<osf::StringValue>(stringIndex)->value = core::utils::string::ToString(value);
	}

	void properties::SetProperty ( osf::ObjectValue* data, const char* stringIndex, const Vector3f value )
	{
		data->GetConvertAdd<osf::StringValue>(stringIndex)->value = core::utils::string::ToString(value);
	}

	void properties::SetProperty ( osf::ObjectValue* data, const char* stringIndex, const Color value )
	{
		ColorRGBA16 intConversion = value.ToRGBA16();
		data->GetConvertAdd<osf::StringValue>(stringIndex)->value = core::utils::string::ToString(value);
	}


	template<>
	float properties::GetProperty ( osf::ObjectValue* data, const char* stringIndex )
	{
		return data->GetConvertAdd<osf::FloatValue>(stringIndex)->value;
	}

	template<>
	int properties::GetProperty ( osf::ObjectValue* data, const char* stringIndex )
	{
		return (int)data->GetConvertAdd<osf::IntegerValue>(stringIndex)->value;
	}

	template<>
	bool properties::GetProperty ( osf::ObjectValue* data, const char* stringIndex )
	{
		return data->GetConvertAdd<osf::BooleanValue>(stringIndex)->value;
	}

	template<>
	const char* properties::GetProperty ( osf::ObjectValue* data, const char* stringIndex )
	{
		return data->GetConvertAdd<osf::StringValue>(stringIndex)->value.c_str();
	}

	template<>
	Vector2f properties::GetProperty ( osf::ObjectValue* data, const char* stringIndex )
	{
		return core::utils::string::ToObject<Vector2f>(data->GetConvertAdd<osf::StringValue>(stringIndex)->value.c_str());
	}

	template<>
	Vector3f properties::GetProperty ( osf::ObjectValue* data, const char* stringIndex )
	{
		return core::utils::string::ToObject<Vector3f>(data->GetConvertAdd<osf::StringValue>(stringIndex)->value.c_str());
	}

	template<>
	Color properties::GetProperty ( osf::ObjectValue* data, const char* stringIndex )
	{
		return core::utils::string::ToObject<ColorRGBA16>(data->GetConvertAdd<osf::StringValue>(stringIndex)->value.c_str()).ToRGBAFloat();
	}

}}}