#include "ModelIO.h"

#include "core/debug/console.h"

#include "core-ext/system/io/Resources.h"
#include "core-ext/system/io/assets/Conversion.h"
#include "core-ext/system/io/Files.h"

#include <chrono>

core::MpdInterface::MpdInterface ( void )
{
	;
}

core::MpdInterface::~MpdInterface ( void )
{
	// free the loaded data
	for (uint type_index = 0; type_index < (uint)ModelFmtSegmentType::kMAX; ++type_index)
	{
		for (uint segment_index = 0; segment_index < (uint)m_segments[segment_index].size(); ++segment_index)
		{
			delete[] (char*)(m_segmentsData[type_index][segment_index]);
		}
	}

	// Close any live file
	if (m_liveFile != NULL)
	{
		fclose(m_liveFile);
		m_liveFile = NULL;
	}
}

bool core::MpdInterface::Open ( const char* n_resource_name, const bool n_convert )
{
	// Close any live file
	if (m_liveFile != NULL)
	{
		fclose(m_liveFile);
		m_liveFile = NULL;
	}

	// Create the resource name
	std::string mpd_filename;
	{
		bool bRequiresConvert = false;
		bool bMpdFound = false;
		bool bGltfFound = false;
		bool bGlbFound = false;

		arstring256 model_rezname (n_resource_name);

		// Find a MPD file.
		mpd_filename = model_rezname + ".mpd";
		if (!core::Resources::MakePathTo(mpd_filename.c_str(), mpd_filename))
		{
			bRequiresConvert = true;
		}
		else
		{
			bMpdFound = true;
		}

		// Find a GLTF file.
		std::string gltf_filename = model_rezname + ".gltf";
		if (core::Resources::MakePathTo(gltf_filename.c_str(), gltf_filename))
		{
			bGltfFound = true;
		}
		std::string glb_filename = model_rezname + ".glb";
		if (core::Resources::MakePathTo(glb_filename.c_str(), glb_filename))
		{
			bGlbFound = true;
		}
		if (!bGltfFound && !bGlbFound)
		{
			if (n_convert && bRequiresConvert)
			{
				// If we require a convert at this point, we can't open the model.
				return false;
			}
		}

		// If MPD is found, and GLTF found, compare the times in them to check
		if (bMpdFound && (bGltfFound || bGlbFound))
		{
			bool bMpdCorrupted = false;
			bool bMpdOutOfDate = false;

			// Read in the MPD header
			FILE* l_fileMpd = fopen(mpd_filename.c_str(), "rb");
			modelFmtHeader mpd_header;
			fread(&mpd_header, sizeof(modelFmtHeader), 1, l_fileMpd);
			fclose(l_fileMpd);

			// Check for correct MPD format & version
			bMpdCorrupted = strcmp(mpd_header.head, kModelFormat_Header) != 0;
			bMpdOutOfDate = mpd_header.version[0] < kModelFormat_VersionMajor || (mpd_header.version[0] == kModelFormat_VersionMajor && mpd_header.version[1] < kModelFormat_VersionMinor);

			// Grab the MPD's age
			uint64 mpd_datetime = mpd_header.datetime;

			// Grab the GLTF's age
			uint64 gltf_datetime = bGltfFound ? io::file::GetLastWriteTime(gltf_filename.c_str()) : io::file::GetLastWriteTime(glb_filename.c_str());

			// If the GLTF is newer, then we need to convert before we continue
			if (bMpdCorrupted || bMpdOutOfDate
				|| gltf_datetime > mpd_datetime)
			{
				bRequiresConvert = true;
			}

			// Pull the older datetime (if we have it)
			m_datetime = std::max<uint64>(mpd_datetime, gltf_datetime);
		}

		// Let's convert if we need a convert
		if (bRequiresConvert && n_convert)
		{
			ARCORE_ASSERT(bGltfFound || bGlbFound);
			if (core::Converter::ConvertFile(bGltfFound ? gltf_filename.c_str() : glb_filename.c_str()) == false)
			{
				debug::Console->PrintError( "MpdLoader::Open : Error occurred in core::Converter::ConvertFile call\n" );
			}
		}

		// Find the MPD post-convert
		if (!bMpdFound && n_convert)
		{
			if (!core::Resources::MakePathTo(mpd_filename.c_str(), mpd_filename))
			{
				debug::Console->PrintError( "MpdLoader::Open : Could not find model file in the resources.\n" );
				return false;
			}
		}
	}

	// Open the new file
	m_liveFile = fopen(mpd_filename.c_str(), "rb+");
	if (m_liveFile != NULL)
	{
		return LoadMpdCommon();
	}
	// If not converting files, then we want to open a new file.
	else if (!n_convert)
	{
		m_liveFile = fopen(mpd_filename.c_str(), "wb+");
		if (m_liveFile != NULL)
		{
			// Reset local information
			for (uint type_index = 0; type_index < (uint)ModelFmtSegmentType::kMAX; ++type_index)
			{
				m_segments[type_index].clear();
				m_segmentsData[type_index].clear();
			}

			// Set time to now
			m_datetime = (uint64_t)std::chrono::system_clock::now().time_since_epoch().count();

			return true;
		}
	}

	return false;
}

bool core::MpdInterface::OpenFile ( const char* n_file_name, const bool n_create_on_missing )
{
	// Close any live file
	if (m_liveFile != NULL)
	{
		fclose(m_liveFile);
		m_liveFile = NULL;
	}

	// Open the new file
	m_liveFile = fopen(n_file_name, "rb+");
	if (m_liveFile != NULL)
	{
		return LoadMpdCommon();
	}
	else if (n_create_on_missing)
	{
		m_liveFile = fopen(n_file_name, "wb+");
		if (m_liveFile != NULL)
		{
			// Reset local information
			for (uint type_index = 0; type_index < (uint)ModelFmtSegmentType::kMAX; ++type_index)
			{
				m_segments[type_index].clear();
				m_segmentsData[type_index].clear();
			}

			// Set time to now
			m_datetime = (uint64_t)std::chrono::system_clock::now().time_since_epoch().count();

			return true;
		}
	}
	return false;
}

bool core::MpdInterface::LoadMpdCommon ( void )
{
	// Reset local information
	for (uint type_index = 0; type_index < (uint)ModelFmtSegmentType::kMAX; ++type_index)
	{
		m_segments[type_index].clear();
		m_segmentsData[type_index].clear();
	}

	// Read in the header
	modelFmtHeader header;
	fread(&header, sizeof(modelFmtHeader), 1, m_liveFile);
	if (strcmp(header.head, kModelFormat_Header) != 0)
	{
		return false;
	}

	m_datetime = header.datetime; // Need the datetime in case we edit later.
	
	// Read in the segments
	std::vector<modelFmtSegmentInfoHeader> l_allSegments;
	l_allSegments.resize(header.segmentInfoCount);
	fread(l_allSegments.data(), sizeof(modelFmtSegmentInfoHeader), l_allSegments.size(), m_liveFile);

	// Verify segments are valid & count data
	for (auto& segment : l_allSegments)
	{
		if (strcmp(segment.head, kModelFormat_HeadSegmentInfo) != 0)
		{
			ARCORE_ERROR("Invalid data.");
			return false;
		}

		m_segments[(int)segment.type].push_back(segment);
		m_segmentsData[(int)segment.type].push_back(NULL);
	}

	return true;
}

core::modelFmtSegmentInfoHeader& core::MpdInterface::GetSegment ( const ModelFmtSegmentType segment_type, int index )
{
	const int list_index = (int)segment_type;
	ARCORE_ASSERT(index >= 0 && index < m_segments[list_index].size());
	return m_segments[list_index][index];
}

const void* core::MpdInterface::GetSegmentData ( const ModelFmtSegmentType segment_type, int index )
{
	const int list_index = (int)segment_type;
	ARCORE_ASSERT(index >= 0 && index < m_segments[list_index].size());

	if (m_segmentsData[list_index][index] != NULL)
	{
		return m_segmentsData[list_index][index];
	}
	else
	{
		// We need to load up the data from the disk.
		m_segmentsData[list_index][index] = new char [m_segments[list_index][index].dataSizeDeflated];

		// Cannot decompress right now
		if (m_segments[list_index][index].flag_isCompressed)
		{
			ARCORE_ERROR("Decompression not currently supported.");
			return nullptr;
		}
		else
		{
			ARCORE_ASSERT(m_segments[list_index][index].dataSize == m_segments[list_index][index].dataSizeDeflated);

			fseek(m_liveFile, m_segments[list_index][index].dataOffset, SEEK_SET);
			fread(m_segmentsData[list_index][index], m_segments[list_index][index].dataSize, 1, m_liveFile);

			return m_segmentsData[list_index][index];
		}
	}
}

void core::MpdInterface::ReleaseSegmentData ( const ModelFmtSegmentType segment_type, int index )
{
	const int list_index = (int)segment_type;
	ARCORE_ASSERT(index >= 0 && index < m_segments[list_index].size());

	// Delete any local data we're currently storing.
	if (m_segmentsData[list_index][index] != NULL)
	{
		delete[] ((char*)m_segmentsData[list_index][index]);
		m_segmentsData[list_index][index] = NULL;
	}
}

void core::MpdInterface::RemoveSegment ( const ModelFmtSegmentType segment_type, int index )
{
	const int list_index = (int)segment_type;
	ARCORE_ASSERT(index >= 0 && index < m_segments[list_index].size());

	ReleaseSegmentData(segment_type, index);

	m_segments[list_index].erase(m_segments[list_index].begin() + index);
	m_segmentsData[list_index].erase(m_segmentsData[list_index].begin() + index);
}

void core::MpdInterface::UpdateSegmentData ( const ModelFmtSegmentType segment_type, int index, const uint32 dataSize, const void* data )
{
	const int list_index = (int)segment_type;
	ARCORE_ASSERT(data != NULL);
	ARCORE_ASSERT(index >= 0 && index < m_segments[list_index].size());

	// Get current header & update data
	modelFmtSegmentInfoHeader& segmentInfo = m_segments[list_index][index];
	memcpy(segmentInfo.head, kModelFormat_HeadSegmentInfo, 4);
	segmentInfo.dataSizeDeflated = dataSize;
	segmentInfo.dataSize = dataSize;
	segmentInfo.flag_isCompressed = false; // Do not compress

	// Free old data
	ReleaseSegmentData(segment_type, index);

	// Create new data
	void* data_section = new char [dataSize];
	memcpy(data_section, data, dataSize);
	// TODO: do not compress, just add
	m_segmentsData[list_index][index] = data_section;
}

void core::MpdInterface::AddSegment ( const modelFmtSegmentInfoHeader& header, const void* data )
{
	const int list_index = (int)header.type;
	ARCORE_ASSERT(data != NULL);

	// Add new header & data slot
	m_segments[list_index].push_back(header);
	m_segmentsData[list_index].push_back(NULL);

	// Update the data
	UpdateSegmentData( header.type, (int)(m_segments[list_index].size() - 1), header.dataSize, data);
}

bool core::MpdInterface::Save ( void )
{
	ARCORE_ASSERT(m_liveFile != NULL);

	// Set time to now
	m_datetime = (uint64_t)std::chrono::system_clock::now().time_since_epoch().count();

	// Count all segments & data
	int segment_count = 0;
	bool bHasGeometry = false;
	bool bHasSkeleton = false;
	bool bHasPhysics = false;
	bool bHasAnimation = false;
	for (uint type_index = 0; type_index < (uint)ModelFmtSegmentType::kMAX; ++type_index)
	{
		segment_count += (int)m_segments[type_index].size();
		if (!m_segments[type_index].empty())
		{
			if (type_index == (uint)ModelFmtSegmentType::kGeometryInfo
				|| type_index == (uint)ModelFmtSegmentType::kGeometryIndices
				|| type_index == (uint)ModelFmtSegmentType::kGeometryVertexData)
			{
				bHasGeometry = true;
			}
		}

		// Read in all the data - it could end up being deleted
		for (uint segment_index = 0; segment_index < m_segments[type_index].size(); ++segment_index)
		{
			// GetSegmentData() call forces the data to be loaded into memory, decompressed, and stored.
			GetSegmentData((core::ModelFmtSegmentType)type_index, segment_index);
			ARCORE_ASSERT(m_segmentsData[type_index][segment_index] != NULL);
		}
	}
	
	// Generate the base offset for data
	intptr_t start_segment_offset = sizeof(modelFmtHeader) + sizeof(modelFmtSegmentInfoHeader) * segment_count;

	// Create header and write
	{
		modelFmtHeader header;
		header.datetime = m_datetime;
		header.flag_hasGeometry = bHasGeometry;
		header.flag_hasSkeleton = bHasSkeleton;
		header.flag_hasPhysics = bHasPhysics;
		header.flag_hasAnimation = bHasAnimation;
		header.segmentInfoCount = segment_count;
	
		fseek(m_liveFile, 0, SEEK_SET);
		fwrite(&header, sizeof(modelFmtHeader), 1, m_liveFile);
	}

	// Write the segment info
	{
		// Collected segment info
		std::vector<modelFmtSegmentInfoHeader> all_segments;

		// Keep track of current size
		intptr_t current_segment_offset = start_segment_offset;

		// Patch the segment info pointers
		for (uint type_index = 0; type_index < (uint)ModelFmtSegmentType::kMAX; ++type_index)
		{
			for (uint segment_index = 0; segment_index < (uint)m_segments[type_index].size(); ++segment_index)
			{
				ARCORE_ASSERT(!m_segments[type_index][segment_index].flag_isCompressed || (m_segments[type_index][segment_index].dataSize == m_segments[type_index][segment_index].dataSizeDeflated));

				m_segments[type_index][segment_index].dataOffset = (uint32)current_segment_offset;
				current_segment_offset += m_segments[type_index][segment_index].dataSize;

				all_segments.push_back(m_segments[type_index][segment_index]);
			}
		}

		// Write out all the segments
		fwrite(all_segments.data(), sizeof(modelFmtSegmentInfoHeader), all_segments.size(), m_liveFile);
	}

	// Write out all the segments now
	for (uint type_index = 0; type_index < (uint)ModelFmtSegmentType::kMAX; ++type_index)
	{
		for (uint segment_index = 0; segment_index < (uint)m_segments[type_index].size(); ++segment_index)
		{
			ARCORE_ASSERT(ftell(m_liveFile) == (long)m_segments[type_index][segment_index].dataOffset);
			fwrite(m_segmentsData[type_index][segment_index], m_segments[type_index][segment_index].dataSize, 1, m_liveFile);
		}
	}

	return true;
}