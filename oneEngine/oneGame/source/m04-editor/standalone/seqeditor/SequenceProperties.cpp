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
		data->GetConvertAdd<osf::StringValue>(stringIndex)->value = std::to_string(value.x) + "\t" + std::to_string(value.y);
	}

	void properties::SetProperty ( osf::ObjectValue* data, const char* stringIndex, const Vector3f value )
	{
		data->GetConvertAdd<osf::StringValue>(stringIndex)->value = std::to_string(value.x) + "\t" + std::to_string(value.y) + "\t" + std::to_string(value.z);
	}

	void properties::SetProperty ( osf::ObjectValue* data, const char* stringIndex, const Color value )
	{
		ColorRGBA16 intConversion = value.ToRGBA16();
		data->GetConvertAdd<osf::StringValue>(stringIndex)->value =
			std::to_string(intConversion.r) + "\t"
			+ std::to_string(intConversion.g) + "\t"
			+ std::to_string(intConversion.b) + "\t"
			+ std::to_string(intConversion.a);
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
		// Need the string tuplet from the values as well.
		std::string positionTripletString = data->GetConvertAdd<osf::StringValue>(stringIndex)->value.c_str();
		// Split the poisition into three string values
		auto tripletValues = core::utils::string::Split(positionTripletString, core::utils::string::kWhitespace);
		ARCORE_ASSERT(tripletValues.size() == 2);
		// Set the node's position.
		return Vector2f(std::stof(tripletValues[0]), std::stof(tripletValues[1]));
	}

	template<>
	Vector3f properties::GetProperty ( osf::ObjectValue* data, const char* stringIndex )
	{
		// Need the string tuplet from the values as well.
		std::string positionTripletString = data->GetConvertAdd<osf::StringValue>(stringIndex)->value.c_str();
		// Split the poisition into three string values
		auto tripletValues = core::utils::string::Split(positionTripletString, core::utils::string::kWhitespace);
		ARCORE_ASSERT(tripletValues.size() == 3);
		// Set the node's position.
		return Vector3f(std::stof(tripletValues[0]), std::stof(tripletValues[1]), std::stof(tripletValues[2]));
	}

	template<>
	Color properties::GetProperty ( osf::ObjectValue* data, const char* stringIndex )
	{
		// Need the string tuplet from the values as well.
		std::string colorQuadString = data->GetConvertAdd<osf::StringValue>(stringIndex)->value.c_str();
		// Split the poisition into three string values
		auto quadValues = core::utils::string::Split(colorQuadString, core::utils::string::kWhitespace);
		ARCORE_ASSERT(quadValues.size() == 3 || quadValues.size() == 4);
		// Set the node's position.
		return ColorRGBA16(std::stoi(quadValues[0]), std::stoi(quadValues[0]), std::stoi(quadValues[0]), (quadValues.size() == 4) ? std::stoi(quadValues[0]) : 255).ToRGBAFloat();
	}

}}}