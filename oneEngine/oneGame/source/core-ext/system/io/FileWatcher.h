#ifndef CORE_EXT_SYSTEM_IO_FILE_WATCHER_H_
#define CORE_EXT_SYSTEM_IO_FILE_WATCHER_H_

#include <core/types.h>
#include <thread>

namespace core {
namespace io {
namespace file {

	struct arFileWatch;

	enum class Event
	{
		kAdded,
		kRemoved,
		kModified,
		kRenamedOld,
		kRenamedNew,
	};

	typedef std::function<void(const std::string& file, const Event event_type)> arFileWatchCallback;

	CORE_API arFileWatch*	BeginFileWatch ( const char* filepath, arFileWatchCallback callback );
	CORE_API void			EndFileWatch ( arFileWatch* fileWatch );

}}}

#endif//CORE_EXT_SYSTEM_IO_FILE_WATCHER_H_