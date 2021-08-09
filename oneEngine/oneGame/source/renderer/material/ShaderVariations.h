#ifndef SHADER_VARIATIONS_H_
#define SHADER_VARIATIONS_H_

#include <vector>
#include <string>

// Prototype variant info before access
class RrShaderVariantBase;

// Base class for variant macro values.
class ShaderVariantValue
{
public:
	virtual int			GetVariantCount ( void )
	{ return 0; }

	virtual int			GetVariantIndex ( void )
	{ return 0; }

public:
	int&				AccessVariantCount (RrShaderVariantBase* variant);
	std::vector<ShaderVariantValue*>&
						AccessVariantValues (RrShaderVariantBase* variant);
};

// Types of values
class ShaderVariantValueBool;

// Base shader variant
class RrShaderVariantBase
{
protected:
	explicit				RrShaderVariantBase()
		: m_totalVariantCount(0)
		{}

public:
	//	GetTotalVariantCount() : Get total number of available variants in this shader.
	int					GetTotalVariantCount ( void ) const
		{ return m_totalVariantCount; }

	//	GetVariantIndex() : Returns expected index of the current configuration's variant.
	int					GetVariantIndex ( void ) const
	{
		// Loop through each variant to generate the indices.
		int index = 0;
		int multiplier = 1;
		for (ShaderVariantValue* variantValue : m_values)
		{
			index += variantValue->GetVariantIndex() * multiplier;
			// Make sure next element steps over all the current element's indices.
			multiplier *= variantValue->GetVariantCount();
		}
		return index;
	}

	//	GetVariantName() : Returns expected name of the current configuration's variant.
	std::string			GetVariantName ( void ) const
	{
		return std::string(GetShaderName()) + "_" + std::to_string(GetVariantIndex());
	}

	virtual const char*	GetShaderName ( void ) const
		{ return "Invalid Shader"; }

protected:
	// Allow all the values to see the internals 
	friend ShaderVariantValue;
	friend ShaderVariantValueBool;

	int					m_totalVariantCount;
	std::vector<ShaderVariantValue*>
						m_values;
};

int& ShaderVariantValue::AccessVariantCount (RrShaderVariantBase* variant)
{
	return variant->m_totalVariantCount;
}
std::vector<ShaderVariantValue*>& ShaderVariantValue::AccessVariantValues (RrShaderVariantBase* variant)
{
	return variant->m_values;
}

// Boolean variant macro value.
class ShaderVariantValueBool : public ShaderVariantValue
{
public:
	explicit				ShaderVariantValueBool(const RrShaderVariantBase *const const_owningVariant)
		: ShaderVariantValue()
	{
		RrShaderVariantBase* owningVariant = (RrShaderVariantBase*)const_owningVariant;
		owningVariant->m_totalVariantCount = std::max(1, owningVariant->m_totalVariantCount * 2);
		owningVariant->m_values.push_back(this);
	}

	// Variant information
	virtual int			GetVariantCount ( void ) override
		{ return 2; }
	virtual int			GetVariantIndex ( void ) override
		{ return value ? 1 : 0; }

	// Option setting
	bool&					operator= (const bool input)
	{
		value = input; 
		return value;
	}
							operator bool() const
		{ return value; }

public:
	bool				value = false;
};

// Enumerating integer variant macro value.
template <int... Values>
class ShaderVariantValueEnumInt : public ShaderVariantValue
{
public:
	explicit				ShaderVariantValueEnumInt(const RrShaderVariantBase *const const_owningVariant)
		: ShaderVariantValue()
	{
		RrShaderVariantBase* owningVariant = (RrShaderVariantBase*)const_owningVariant;
		const std::size_t ValueCount = sizeof...(Values);
		AccessVariantCount(owningVariant) = std::max<int>(1, AccessVariantCount(owningVariant) * ValueCount);
		AccessVariantValues(owningVariant).push_back(this);
	}

	// Variant information
	virtual int			GetVariantCount ( void ) override
		{ return (int)sizeof...(Values); }
	virtual int			GetVariantIndex ( void ) override
	{
		int index = _GetVariantIndex(0, Values...);
		return index;
	}

private:
	template <typename Arg>
	int _GetVariantIndex ( int current_index, Arg&& arg )
	{
		if (value == std::forward<Arg>(arg))
		{
			return current_index;
		}
		ARCORE_ERROR("Invalid value passed in.");
		return -1;
	}
	template <typename Arg, typename... Args>
	int _GetVariantIndex ( int current_index, Arg&& arg, Args&&... args)
	{
		if (value == std::forward<Arg>(arg))
		{
			return current_index;
		}
		return _GetVariantIndex( current_index + 1, std::forward<Args>(args)... );
	}

public:
	// Option setting
	int&					operator= (const int input)
	{
		value = input;
		ARCORE_ASSERT(GetVariantIndex() != -1);
		return value;
	}
							operator int() const
		{ return value; }

public:
	int					value = 0;
};


#define RR_SHADER_VARIANT_BEGIN(ShaderName) \
	class RrShaderVariant_ShaderType##ShaderName : public RrShaderVariantBase \
	{ \
	public: \
		virtual const char*	GetShaderName ( void ) const override \
			{ return #ShaderName ; }

#define SHADER_BOOL(MacroName) \
		ShaderVariantValueBool MacroName;
#define SHADER_ENUM(MacroName, ...) \
		ShaderVariantValueEnumInt<__VA_ARGS__> MacroName;

#define RR_SHADER_VARIANT_COLLECT_BEGIN(ShaderName) \
	public: \
		explicit RrShaderVariant_ShaderType##ShaderName (void) \
			: RrShaderVariantBase()

#define RR_SHADER_COLLECT(MacroName) \
			, MacroName(this)

#define RR_SHADER_VARIANT_COLLECT_END() \
			{}

#define RR_SHADER_VARIANT_END() \
	};

#define RR_SHADER_VARIANT(ShaderName) \
	RrShaderVariant_ShaderType##ShaderName


#endif//SHADER_VARIATIONS_H_