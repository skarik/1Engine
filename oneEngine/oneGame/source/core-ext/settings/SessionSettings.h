#ifndef CORE_EXT_SESSION_SETTINGS_H_
#define CORE_EXT_SESSION_SETTINGS_H_

#include "core/common.h"

#include <vector>
#include <map>
#include <string>

namespace osf {
	class KeyValueTree;
}

namespace core {
namespace settings {

	class Session;
	class BaseSessionSetting;

	class Session
	{
	public:
		CORE_API const BaseSessionSetting*
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

		static const Session*	Get ( void )
		{
			return g_Session;
		}
		
		static const std::map<std::string, BaseSessionSetting*>&
								GetSettingsReadOnly ( void )
		{
			return g_Session->m_data;
		}
		static std::map<std::string, BaseSessionSetting*>&
								GetSettings ( void )
		{
			return g_Session->m_data;
		}

	private:
		friend BaseSessionSetting;

		Session ( void )
		{
			// Add the default invalid 
			m_data[""] = nullptr;
		}

		void AddSetting ( BaseSessionSetting* value, const char* key )
		{
			ARCORE_ASSERT_MSG(m_data.find(key) == m_data.end(), "Setting with duplicate name found.");
			m_data[key] = value;
		}

		static void PrepareForSettings ( void )
		{
			if (g_Session == nullptr)
			{
				g_Session = new Session();
			}
		}

	private:
		CORE_API static Session*
							g_Session;

		std::map<std::string, BaseSessionSetting*>
							m_data;
	};

	enum class SessionSettingGroup
	{
		kClient,
		kClientRenderer,
		kServer,
	};

	// Base persistent setting class;
	class BaseSessionSetting 
	{
	protected:
		explicit BaseSessionSetting ( const char* identifier, const SessionSettingGroup group = SessionSettingGroup::kClient )
		{
			Session::PrepareForSettings();
			Session::g_Session->AddSetting(this, identifier);
		}

	public:
		virtual void			SetFromString (const char* str) = 0;
		virtual std::string		ToString (void) const = 0;
	};

	//	class SessionSetting<Type> : Provides a type-safe and locally scoped persistent setting.
	// Can be edited directly, or queried from the Persistent class's storage.
	template <typename Type>
	class SessionSetting : public BaseSessionSetting
	{
	public:
		explicit SessionSetting ( const char* identifier, const Type& defaultValue, const SessionSettingGroup group = SessionSettingGroup::kClient )
			: BaseSessionSetting(identifier, group)
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

#	define SESSION_SETTING_SPECIALIZE_SET(TYPE, CONVERSION) \
	template<> \
	void SessionSetting<TYPE>::SetFromString (const char* str) \
	{ \
		m_value = (TYPE)CONVERSION(str); \
	}

	SESSION_SETTING_SPECIALIZE_SET(float, std::atof);
	SESSION_SETTING_SPECIALIZE_SET(double, std::atof);
	SESSION_SETTING_SPECIALIZE_SET(int8, std::atoi);
	SESSION_SETTING_SPECIALIZE_SET(uint8, std::atoi);
	SESSION_SETTING_SPECIALIZE_SET(int16, std::atoi);
	SESSION_SETTING_SPECIALIZE_SET(uint16, std::atoi);
	SESSION_SETTING_SPECIALIZE_SET(int32, std::atoi);
	SESSION_SETTING_SPECIALIZE_SET(uint32, std::atoll);
	SESSION_SETTING_SPECIALIZE_SET(int64, std::atoll);
	SESSION_SETTING_SPECIALIZE_SET(uint64, std::atoll);

#	undef SESSION_SETTING_SPECIALIZE_SET

	template<>
	std::string SessionSetting<std::string>::ToString (void) const
	{ 
		return m_value;
	}

}}

#endif//CORE_EXT_PERSISTENT_SETTINGS_H_