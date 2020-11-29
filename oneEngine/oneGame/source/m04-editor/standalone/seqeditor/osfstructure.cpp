#include "osfstructure.h"

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
SpecializeOSFCastTemplate(osf::LabelValue, kLabel);
SpecializeOSFCastTemplate(osf::StringValue, kString);
SpecializeOSFCastTemplate(osf::IntegerValue, kInteger);
SpecializeOSFCastTemplate(osf::FloatValue, kFloat);
SpecializeOSFCastTemplate(osf::BooleanValue, kBoolean);

#undef SpecializeOSFCastTemplate