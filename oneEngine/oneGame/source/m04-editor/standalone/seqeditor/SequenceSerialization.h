#ifndef M04_EDITORS_SEQUENCE_EDITOR_SEQUENCE_SEQUENCE_SERIALIZER_H_
#define M04_EDITORS_SEQUENCE_EDITOR_SEQUENCE_SEQUENCE_SERIALIZER_H_

#include "./SequenceNode.h"

#include "core-ext/system/io/osf.h"

namespace m04 {
namespace editor {
namespace sequence {

	// Node serialization interface
	class ISequenceSerializer
	{
	public:
		EDITOR_API virtual bool IsValid ( void ) = 0;

		EDITOR_API virtual void	SerializeFileBegin ( void ) = 0;
		EDITOR_API virtual void	SerializeListBegin ( void ) = 0;
		EDITOR_API virtual void	SerializeStartpoint ( const m04::editor::SequenceNode* node ) = 0;
		EDITOR_API virtual void	SerializeJumptarget ( const m04::editor::SequenceNode* node ) = 0;
		EDITOR_API virtual void	SerializeNode ( const m04::editor::SequenceNode* node ) = 0;
		EDITOR_API virtual void	SerializeListEnd ( const m04::editor::SequenceNode* lastNode ) = 0;
		EDITOR_API virtual void	SerializeFileEnd ( void ) = 0;
	};

	// Node deserialization interface
	class ISequenceDeserializer
	{
	public:
		EDITOR_API virtual bool	IsValid ( void ) = 0;

		EDITOR_API virtual bool DeserializeFileBegin(void) = 0;
	};

	class OsfSerializer : public ISequenceSerializer
	{
	public:
		EDITOR_API explicit		OsfSerializer ( const char* filename );
		EDITOR_API				~OsfSerializer ( void );

		EDITOR_API virtual bool IsValid ( void ) override;

		EDITOR_API virtual void	SerializeFileBegin ( void ) override;
		EDITOR_API virtual void	SerializeListBegin ( void ) override;
		EDITOR_API virtual void	SerializeStartpoint ( const m04::editor::SequenceNode* node ) override;
		EDITOR_API virtual void	SerializeJumptarget ( const m04::editor::SequenceNode* node ) override;
		EDITOR_API virtual void	SerializeNode ( const m04::editor::SequenceNode* node ) override;
		EDITOR_API virtual void	SerializeListEnd ( const m04::editor::SequenceNode* lastNode ) override;
		EDITOR_API virtual void	SerializeFileEnd ( void ) override;

	protected:

		FILE*				osf_fileHandle = NULL;
		io::OSFWriter*		osf_writer = NULL;
	};

	class JsonSerializer : public ISequenceSerializer
	{
	};

	class OsfDeserializer : ISequenceDeserializer
	{
	public:

		EDITOR_API explicit OsfDeserializer ( const char* filename);
		EDITOR_API			~OsfDeserializer ( void );

		EDITOR_API virtual bool DeserializeFileBegin ( void ) override;
		EDITOR_API virtual m04::editor::SequenceNode* DeserializeNode ( void );

	protected:

		FILE * osf_fileHandle = NULL;
		io::OSFReader*		osf_reader = NULL;
		int buffer_size = 256;
		char * buffer = NULL;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_SEQUENCE_SEQUENCE_SERIALIZER_H_