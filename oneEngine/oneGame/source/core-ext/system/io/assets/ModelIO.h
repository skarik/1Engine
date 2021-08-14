//===============================================================================================//
//
//	ModelIO.h
//
// Definitions for the Model Part Data (MPD) format. 
//
//===============================================================================================//
#ifndef CORE_ASSETS_MODEL_IO_H_
#define CORE_ASSETS_MODEL_IO_H_

#include <string>
#include <stdint.h>
#include <vector>

#include "core/containers/arstring.h"
#include "core/utils/string.h"
#include "core/gfx/pixelFormat.h"
#include "core/types/ModelData.h"

namespace core
{
	static const char*	kModelFormat_Header			= "MPD\0";

	static const int	kModelFormat_VersionMajor	= 0;
	static const int	kModelFormat_VersionMinor	= 3;

	static const char*	kModelFormat_HeadSegmentInfo	= "INF\0";

	static const char*	kModelFormat_HeadGeometryInfo	= "GEO\0";

	enum class ModelFmtSegmentType : uint8
	{
		kUnknown					= 0,
		
		kGeometryInfo				= 1,
		kGeometryIndices			= 2,
		kGeometryVertexData			= 3,

		kMAX,
	};

	// see renderer/types/shaders/vattribs.h
	enum class ModelFmtVertexAttribute : uint8
	{
		kPosition		= 0,
		kUV0			= 1,
		kColor			= 2,
		kNormal			= 3,
		kTangent		= 4,
		kBinormal		= 5,
		kUV1			= 6,
		kBoneWeight		= 7,
		kBoneIndices	= 8,
	};

	struct modelFmtHeader
	{
	public:
		// Always "MPD\0"
		char		head[4] = {kModelFormat_Header[0], kModelFormat_Header[1], kModelFormat_Header[2], kModelFormat_Header[3]};
		// version
		uint16		version[2] = {kModelFormat_VersionMajor, kModelFormat_VersionMinor};
		//	CONVERSION TIME
		// Date of creation/conversion
		uint64		datetime = 0;

		//	FLAGS
		union
		{
			struct
			{
				// Does this file include renderable geometry
				uint flag_hasGeometry : 1;
				// Does this file include a skeleton
				uint flag_hasSkeleton : 1;
				// Does this file include physics geometry
				uint flag_hasPhysics : 1;
				// Does this file include animation
				uint flag_hasAnimation : 1;
			};
			uint32	flags = 0;
		};

		//	CONTENT INFO
		// Number of segments.
		uint32_t	segmentInfoCount = 0;

		// A list of segments immediately follows the header.
	};

	struct modelFmtSegmentInfoHeader
	{
	public:
		// Always "INF\0"
		char		head[4] = {kModelFormat_HeadSegmentInfo[0], kModelFormat_HeadSegmentInfo[1], kModelFormat_HeadSegmentInfo[2], kModelFormat_HeadSegmentInfo[3]};
		ModelFmtSegmentType
					type = ModelFmtSegmentType::kUnknown;
		union
		{
			struct
			{
				// Is this segment compressed?
				uint flag_isCompressed : 1;
			};
			uint8	flags = 0;
		};

		// sub-group index. Can be used to identify mesh index
		uint8		subindex = 0;
		// sub-group identifier. Can be used to identify attribute types, for instance
		uint8		subtype = 0;

		// byte offset to the start of the data
		uint32		dataOffset = 0;
		// byte size of the data segment
		uint32		dataSize = 0;
		// byte size of the data segment uncompressed
		uint32		dataSizeDeflated = 0;
		// hash for making sure the data isnt corrupted (unused)
		uint64		tinyHash = 0;
	};

	struct modelFmtSegmentGeometry_Info
	{
	public:
		// Always "GEO\0"
		char		head[4] = {kModelFormat_HeadGeometryInfo[0], kModelFormat_HeadGeometryInfo[1], kModelFormat_HeadGeometryInfo[2], kModelFormat_HeadGeometryInfo[3]};
		// Indicies in the mesh
		uint32		index_count = 0;
		// Vertices in the mesh
		uint32		vertex_count = 0;
		// if true, is made of triangle strips. If false, is a list of triangles.
		bool		is_strips = false;
		// bounding box minimum
		Vector3f	bbox_min;
		// bounding box maximum
		Vector3f	bbox_max;
		// 256-character string of the name
		arstring256	name; // This is limited in length and constant in order to simplify the IO.

	public:
		const bool		IsValid ( void )
		{
			return head[0] == kModelFormat_HeadGeometryInfo[0]
				&& head[1] == kModelFormat_HeadGeometryInfo[1]
				&& head[2] == kModelFormat_HeadGeometryInfo[2]
				&& head[3] == kModelFormat_HeadGeometryInfo[3];
		}
	};
	//static const int sz = sizeof(modelFmtSegmentGeometryInfo);

	/*struct modelFmtSegmentGeometry_VertexDataHeader
	{
	public:
		char		head[4];	// Always "GEO\0"
	};*/

	//	class MpdInterface : IO for Models.
	// Because Mpd's need to only have certain aspects readable or writable, the interface for input and output is the same.
	class MpdInterface
	{
	public:
		CORE_API explicit		MpdInterface ( void );
		CORE_API				~MpdInterface ( void );

		//	Open ( resource name, convert ) : Attempts to open given MPD.
		// If any issues are detected with the file, will return false.
		CORE_API bool			Open ( const char* n_resource_name, const bool n_convert = true );

		//	OpenFile ( file name, create on missing ) : Attemps to open given MPD.
		// If any issues are detected with the file, will return false.
		CORE_API bool			OpenFile ( const char* n_file_name, const bool n_create_on_missing = false );

		//	Save ( ) : Attemps to resave given MPD.
		// This may spike memory usage as the entire file is loaded into memory.
		CORE_API bool			Save ( void );

		//	GetSegmentCount(type) : Returns number of given segments found.
		// Segments are counted on open.
		CORE_API uint16			GetSegmentCount ( const ModelFmtSegmentType segment_type )
		{
			return (uint16)m_segments[(uint)segment_type].size();
		}

		//	GetSegment(type, index) : Returns segment with type and index.
		CORE_API modelFmtSegmentInfoHeader&
								GetSegment ( const ModelFmtSegmentType segment_type, int index );

		//	GetSegmentData(type, index) : Returns the segment data with given type and index.
		CORE_API const void*	GetSegmentData ( const ModelFmtSegmentType segment_type, int index );

		//	ReleaseSegmentData(type, index) : Releases the decompressed/allocated segment data
		CORE_API void			ReleaseSegmentData ( const ModelFmtSegmentType segment_type, int index );

		//	RemoveSegment(type, index) : Removes segment with type and index.
		CORE_API void			RemoveSegment ( const ModelFmtSegmentType segment_type, int index );

		//	UpdateSegmentData(type, index, data) : Updates segment with the given data
		CORE_API void			UpdateSegmentData ( const ModelFmtSegmentType segment_type, int index, const uint32 dataSize, const void* data );

		//	AddSegment(type, header, data) : Adds segment with the type and index, and stores the data.
		CORE_API void			AddSegment ( const modelFmtSegmentInfoHeader& header, const void* data );

	protected:
		bool					LoadMpdCommon ( void );

	protected:
		std::vector<modelFmtSegmentInfoHeader>
							m_segments [(uint)ModelFmtSegmentType::kMAX];
		std::vector<void*>	m_segmentsData [(uint)ModelFmtSegmentType::kMAX];

		FILE*				m_liveFile = nullptr;

		uint64				m_datetime = 0;
	};
}

#endif//CORE_ASSETS_MODEL_IO_H_
