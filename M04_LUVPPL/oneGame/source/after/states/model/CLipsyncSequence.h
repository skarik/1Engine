
#ifndef _C_LIPSYNC_SEQUENCE_H_
#define _C_LIPSYNC_SEQUENCE_H_

#include <vector>
#include <string>

#include "core/types/float.h"
#include "core/math/Vector3d.h"
#include "core/math/Quaternion.h"
#include "core/math/matrix/CMatrix.h"

class CCamera;
class CAudioSource;
class CMccCharacterModel;
namespace NPC {
	class CNpcBase;
}

class CLipsyncSequence
{

public:
	void UpdateMorphs ( void );

	void Load ( const std::string& );
	void Save ( const std::string& );

private:
	void CreatePhonetic ( const char*, Matrix4x4& );

public:
	//	struct keyframe_t
	// Keyframe structure used to store the phonetic information at a given time.
	struct keyframe_t
	{
		Real	time;
		Real	strength;
		char	phonetic [16];

		bool operator < (const keyframe_t& right) {
			return time < right.time;
		}
	};
	// List of all the keyframes for this sequence
	std::vector<keyframe_t>	m_keyframes;

	// Tracker of the current time of the sequence
	Real	m_position;
	Real	m_sequence_length;

	// Target mode to work on
	CMccCharacterModel*	m_model;
};

#endif//_C_LIPSYNC_SEQUENCE_H_