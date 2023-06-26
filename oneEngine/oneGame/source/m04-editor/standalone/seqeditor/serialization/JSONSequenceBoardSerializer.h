#ifndef M04_EDITORS_SEQUENCE_EDITOR_SEQUENCE_BOARD_SERIALIZER_JSON_H_
#define M04_EDITORS_SEQUENCE_EDITOR_SEQUENCE_BOARD_SERIALIZER_JSON_H_

#include "m04-editor/standalone/seqeditor/SequenceSerialization.h"
//#include "nlohmann/json.hpp"

namespace m04 {
namespace editor {
namespace sequence {

	class JSONSequenceBoardSerializer : public ISequenceBoardSerializer
	{
	public:
		EDITOR_API explicit		JSONSequenceBoardSerializer ( const char* filename );
		EDITOR_API				~JSONSequenceBoardSerializer ( void );

		EDITOR_API virtual void	SerializeBoard ( const NodeBoardState* board ) override;
		EDITOR_API virtual void	DeserializeBoard ( NodeBoardState* board ) override;

	protected:
		arstring256			mFilename;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_SEQUENCE_BOARD_SERIALIZER_JSON_H_