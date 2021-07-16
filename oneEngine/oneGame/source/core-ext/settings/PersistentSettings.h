#ifndef CORE_EXT_PERSISTENT_SETTINGS_H_
#define CORE_EXT_PERSISTENT_SETTINGS_H_

#include "core/common.h"

#include <vector>
#include <map>
#include <string>

namespace osf {
	class KeyValueTree;
}

namespace core {
namespace settings {

	class Persistent;
	class BasePersistentSetting;

	class StringSettingGroup
	{
	public:
		//CORE_API const std::string&
		CORE_API const BasePersistentSetting*
								operator[] ( const char* identifier ) const
		{
			auto value_itr = m_data.find(identifier);
			if (value_itr != m_data.end())
			{
				return value_itr->second;
			}
			else
			{	// Return the default invalid value if it cannot be found.
				return m_data.at("");
			}
		}
		//CORE_API std::string&	operator[] ( const char* identifier )
		/*CORE_API BasePersistentSetting*
								operator[] ( const char* identifier )
		{
			auto value_itr = m_data.find(identifier);
			if (value_itr != m_data.end())
			{
				return value_itr->second;
			}
			// Create a new key-value if it cannot be found.
			return m_data[identifier];
		}*/

		const std::map<std::string, BasePersistentSetting*>&
								GetSettingsReadOnly ( void ) const
		{
			return m_data;
		}
		std::map<std::string, BasePersistentSetting*>&
								GetSettings ( void )
		{
			return m_data;
		}

	private:
		friend Persistent;
		friend BasePersistentSetting;

		StringSettingGroup ( void )
		{
			// Add the default invalid 
			//m_data[""] = "invalid";
			m_data[""] = nullptr;
		}

		void AddSetting ( BasePersistentSetting* value, const char* key )
		{
			ARCORE_ASSERT_MSG(m_data.find(key) == m_data.end(), "Setting with duplicate name found.");
			m_data[key] = value;
		}

	private:
		//std::map<std::string, std::string>
		std::map<std::string, BasePersistentSetting*>
							m_data;
	};

	class Persistent
	{
	public:
		//	PrepareForSettings() : Called before Load() by each persistent setting.
		CORE_API static void	PrepareForSettings ( void );

		CORE_API static void	Load ( void );
		CORE_API static void	Save ( void );

		//	CleanUp() : Frees all internal state. Only call on application end.
		CORE_API static void	CleanUp ( void );

	public:
		CORE_API static StringSettingGroup*
							Global;
		CORE_API static StringSettingGroup*
							User;

	private:
		struct PersistentState
		{
			osf::KeyValueTree*	all_kv_data = nullptr;
		};

		static PersistentState*
							g_state;
	};

	enum class PersistentSettingGroup
	{
		kGlobal,
		kUser,
	};

	// Base persistent setting class;
	class BasePersistentSetting 
	{
	protected:
		explicit BasePersistentSetting ( const char* identifier, const PersistentSettingGroup group = PersistentSettingGroup::kGlobal )
		{
			Persistent::PrepareForSettings();
			if (group == PersistentSettingGroup::kGlobal)
			{
				Persistent::Global->AddSetting(this, identifier);
			}
			else if (group == PersistentSettingGroup::kUser, identifier)
			{
				Persistent::User->AddSetting(this, identifier);
			}
		}

	public:
		virtual void			SetFromString (const char* str) = 0;
		virtual std::string		ToString (void) const = 0;
	};

	//	class PersistentSetting<Type> : Provides a type-safe and locally scoped persistent setting.
	// Can be edited directly, or queried from the Persistent class's storage.
	template <typename Type>
	class PersistentSetting : public BasePersistentSetting
	{
	public:
		explicit PersistentSetting ( const char* identifier, const Type& defaultValue, const PersistentSettingGroup group = PersistentSettingGroup::kGlobal )
			: BasePersistentSetting(identifier, group)
			, m_value(defaultValue)
		{
			
		}

		//	opeartor= : Assignment of the option.
		Type&				operator= (const Type& rhs)
		{
			m_value = rhs;
			return m_value;
		}

		//	implicit cast : Querying of the option.
		/**/				operator const Type(void) const
		{
			return m_value;
		}

		//	implicit cast : Querying of the option.
		/**/				operator const Type&(void) const
		{
			return m_value;
		}

		//	SetFromString(str) : Used by loader to set up initial values.
		void				SetFromString (const char* str) override
		{
			m_value = str;
		}
		//	ToString() : Used by saver to save values back out.
		std::string			ToString (void) const override
		{
			return std::to_string(m_value);
		}

	private:
		Type				m_value;
	};

#	define PERSISTENT_SETTING_SPECIALIZE_SET(TYPE, CONVERSION) \
	template<> \
	void PersistentSetting<TYPE>::SetFromString (const char* str) \
	{ \
		m_value = (TYPE)CONVERSION(str); \
	}

	PERSISTENT_SETTING_SPECIALIZE_SET(float, std::atof);
	PERSISTENT_SETTING_SPECIALIZE_SET(double, std::atof);
	PERSISTENT_SETTING_SPECIALIZE_SET(int8, std::atoi);
	PERSISTENT_SETTING_SPECIALIZE_SET(uint8, std::atoi);
	PERSISTENT_SETTING_SPECIALIZE_SET(int16, std::atoi);
	PERSISTENT_SETTING_SPECIALIZE_SET(uint16, std::atoi);
	PERSISTENT_SETTING_SPECIALIZE_SET(int32, std::atoi);
	PERSISTENT_SETTING_SPECIALIZE_SET(uint32, std::atoll);
	PERSISTENT_SETTING_SPECIALIZE_SET(int64, std::atoll);
	PERSISTENT_SETTING_SPECIALIZE_SET(uint64, std::atoll);

#	undef PERSISTENT_SETTING_SPECIALIZE_SET

	template<>
	std::string PersistentSetting<std::string>::ToString (void) const
	{ 
		return m_value;
	}

}}

#endif//CORE_EXT_PERSISTENT_SETTINGS_H_