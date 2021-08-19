#ifndef CORE_EXT_CONTAINER_TIMELINE_H_
#define CORE_EXT_CONTAINER_TIMELINE_H_

#include "core/types.h"
#include <vector>
#include <algorithm>

// Timeline class for sampling keyframes.
// Currently untested.
template <typename Type>
class arTimeline
{
private:
	struct TypeKey
	{
		float			time;
		Type			value;
	};

	float				m_length;
	float				m_time;
	// All the keys in the timeline
	std::vector<TypeKey>
						m_keys;
	// Keys last sampled in the last advance
	std::vector<Type>	m_keysInLastAdvance;

public:
	void					SetLength (float length)
	{
		m_length = length;
	}
	void					AddKey ( const Type&& key, float time )
	{
		InsertSorted({time, key});
	}

	void					SeekTime (float time)
	{
		m_time = time;
	}

	void					AdvanceToTime (float time, bool loop)
	{
		const float start_time = m_time;
		const float end_time = loop ? time : std::min<float>(time, m_length);
		const bool bSplitBounds = loop ? (end_time < start_time) : false;

		m_keysInLastAdvance.clear();

		// First the first time that time >= start time
		int start_key_index = 0;
		for (; start_key_index < m_keys.size(); ++start_key_index)
		{
			if (m_keys[start_key_index].time >= start_time)
			{
				break;
			}
		}
		
		// Loop forward until we hit the final time
		if (!bSplitBounds)
		{
			for (int key_index = start_key_index; key_index < m_keys.size(); ++key_index)
			{
				if (m_keys[key_index].time < end_time)
				{
					m_keysInLastAdvance.push_back(m_keys[key_index].value);
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			for (int key_index = start_key_index; ; key_index = (key_index + 1) % m_keys.size())
			{
				if (key_index < start_key_index && m_keys[key_index].time >= end_time)
				{
					break;
				}
				else
				{
					m_keysInLastAdvance.push_back(m_keys[key_index].value);
				}
			}
		}

		// We're done.
	}

	void					AdvanceTime (float time, bool loop)
	{
		AdvanceToTime(m_time + time, loop);
	}

	const std::vector<Type>&
							GetKeysInLastAdvance()
		{ return m_keysInLastAdvance; }

private:
	void					InsertSorted ( const TypeKey&& value )
	{
		m_keys.insert(
			std::upper_bound(m_keys.begin(), m_keys.end(), value, [](const TypeKey& A, const TypeKey& B)
			{
				return A.time < B.time;
			}),
			value);
	}
};

#endif//CORE_EXT_CONTAINER_TIMELINE_H_