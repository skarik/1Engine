#include "Volumes.h"
#include "core/os.h"
#include "core/debug.h"

//	EnumerateVolumes() : Returns a list of all the volumes connected to the system.
void core::io::EnumerateVolumes ( std::vector<std::string>& outVolumeList )
{
	ARCORE_ASSERT(outVolumeList.empty());

#if PLATFORM_WINDOWS

	DWORD l_driveBufferSize = GetLogicalDriveStrings(0, NULL) + 1;
	char* l_driveBuffer = new char [l_driveBufferSize];
	GetLogicalDriveStrings(l_driveBufferSize, l_driveBuffer);

	DWORD l_cursor = 0;
	while (l_cursor < l_driveBufferSize - 2)
	{
		outVolumeList.push_back(l_driveBuffer + l_cursor);
		l_cursor += (DWORD)outVolumeList.back().size() + 1;
	}

	delete[] l_driveBuffer;

#elif PLATFORM_APPLE

	ARCORE_ERROR("Unimplemented");

#else

	outVolumeList.push_back("/");

#endif
}