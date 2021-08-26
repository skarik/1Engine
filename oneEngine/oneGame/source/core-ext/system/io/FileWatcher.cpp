#include "FileWatcher.h"

#include "core/os.h"
#include "core/debug.h"
#include "core-ext/threads/naming.h"

#include <fileapi.h>
#include <thread>
#include <mutex>
#include <atomic>

#include <map>
#include <array>
#include <vector>

#include <algorithm>
#include <regex>

#if PLATFORM_WINDOWS

// TODO: make this a utlity. Seriously do this so much
static void convert_wstring(const std::wstring& wstr, std::string& out)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	out.resize(size_needed, '\0');
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &out[0], size_needed, NULL, NULL);
}

// Filewatcher adapted from https://github.com/ThomasMonkman/filewatch

struct arFileWatchLocks
{
	std::atomic_bool
					m_running;
	std::mutex		m_editEventMutex;
	std::condition_variable
					m_editEventCv;

	arFileWatchLocks()
		: m_running(true)
	{}
};

struct core::io::file::arFileWatch
{
	HANDLE			m_directory = NULL;
	bool			m_watchSingleFile = true;

	HANDLE			m_eventReadDir = NULL;
	HANDLE			m_eventClosing = NULL;

	arFileWatchCallback
					m_callback = nullptr;

	std::thread		m_monitorThread;
	std::thread		m_callbackThread;

	std::string		m_pathDirectory;
	std::string		m_pathFilename;

	std::vector<std::pair<std::string, core::io::file::Event>>
					m_editEvents;

	arFileWatchLocks*
					m_locks = nullptr;
};

//	SplitDirectoryAndFile(path, out file, out dir) : Splits input path to filename & directory. TODO: Probably could be done better with std::fs?
const void SplitDirectoryAndFile(const char* path, std::string& out_filename, std::string& out_directory) 
{
	const auto IsDirectoryDividor = [](char character)
	{
		return character == '\\' || character == '/';
	};

	std::string path_string = path;
	const auto pivot = std::find_if(path_string.rbegin(), path_string.rend(), IsDirectoryDividor).base();
	// If the path is filename only, we still need a directory part. We insert local dir "./"
	const auto extracted_directory = std::string(path_string.begin(), pivot);
	out_directory = (extracted_directory.size() > 0) ? extracted_directory : "./";
	// Filename is after the final '\'
	out_filename = std::string(pivot, path_string.end());
}

//	FilePassesFilter(path, regex, filwatch) : Does the given file match the regex & filewatch?
static bool FilePassesFilter(const std::string& file_path, const char* regex_pattern, core::io::file::arFileWatch* fileWatch)
{ 
	if (fileWatch->m_watchSingleFile)
	{
		std::string extracted_filename;
		std::string extracted_directory;
		SplitDirectoryAndFile(file_path.c_str(), extracted_filename, extracted_directory);

		// if we are watching a single file, only that file should trigger action
		return extracted_filename == fileWatch->m_pathFilename;
	}
	return std::regex_match(file_path, std::regex(regex_pattern));
}

//	GetDirectory(path, filewatch) : Initialize filewatch to watch the given path.
// If there is any error, then fileWatch->m_directory will be NULL.
static void GetDirectory (const char* path, core::io::file::arFileWatch* fileWatch)
{
	// Get target file attribs to see if we're watching a directory
	DWORD file_info = GetFileAttributesA(path);
	if (file_info == INVALID_FILE_ATTRIBUTES)
	{
		fileWatch->m_directory = NULL;
		return;
	}
	fileWatch->m_watchSingleFile = (file_info & FILE_ATTRIBUTE_DIRECTORY) == 0;

	// Get the path we want to actually watch.
	std::string watch_path;
	if (fileWatch->m_watchSingleFile)
	{
		std::string parsed_filename, parsed_path;
		SplitDirectoryAndFile(path, parsed_filename, parsed_path);
		fileWatch->m_pathFilename = parsed_filename;
		watch_path = parsed_path;
	}
	else 
	{
		watch_path = path;
	}

	HANDLE directory = CreateFileA(
		watch_path.c_str(),		// pointer to the file name
		FILE_LIST_DIRECTORY,	// access (read/write) mode
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,	// share mode
		nullptr,				// security descriptor
		OPEN_EXISTING,			// how to create
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,	// file attributes
		HANDLE(0));				// file with attributes to copy

	if (directory == INVALID_HANDLE_VALUE)
	{
		fileWatch->m_directory = NULL;
		return;
	}
	
	// Have successful file we can now watch on.
	fileWatch->m_directory = directory;
}

static const DWORD kListenFilters =
	FILE_NOTIFY_CHANGE_SECURITY |
	FILE_NOTIFY_CHANGE_CREATION |
	FILE_NOTIFY_CHANGE_LAST_ACCESS |
	FILE_NOTIFY_CHANGE_LAST_WRITE |
	FILE_NOTIFY_CHANGE_SIZE |
	FILE_NOTIFY_CHANGE_ATTRIBUTES |
	FILE_NOTIFY_CHANGE_DIR_NAME |
	FILE_NOTIFY_CHANGE_FILE_NAME;

static const std::map<DWORD, core::io::file::Event> kEventMapping = {
		{ FILE_ACTION_ADDED,			core::io::file::Event::kAdded },
		{ FILE_ACTION_REMOVED,			core::io::file::Event::kRemoved },
		{ FILE_ACTION_MODIFIED,			core::io::file::Event::kModified },
		{ FILE_ACTION_RENAMED_OLD_NAME,	core::io::file::Event::kRenamedOld },
		{ FILE_ACTION_RENAMED_NEW_NAME,	core::io::file::Event::kRenamedNew }
	};

//	RoutineMonitorDirectory(filewatch) : Thread routine for watching the directory.
// Uses Windows Events to sleep without a spinloop.
static void RoutineMonitorDirectory ( core::io::file::arFileWatch* filewatch )
{
	constexpr size_t kBufferSize = 1024 * 256;
	std::vector<BYTE> buffer(kBufferSize);

	DWORD bytes_returned = 0;
	OVERLAPPED overlapped_buffer{ 0 };

	overlapped_buffer.hEvent = filewatch->m_eventReadDir;

	std::array<HANDLE, 2> handles = { 
		filewatch->m_eventReadDir,
		filewatch->m_eventClosing
	};

	bool bWaitingOnDirectoryRead = false;

	do
	{
		std::vector<std::pair<std::string, core::io::file::Event>> parsed_information;

		ReadDirectoryChangesW(
			filewatch->m_directory,
			buffer.data(),
			static_cast<DWORD>(buffer.size()),
			TRUE,
			kListenFilters,
			&bytes_returned,
			&overlapped_buffer,
			NULL);

		bWaitingOnDirectoryRead = true;

		switch (WaitForMultipleObjects(2, handles.data(), FALSE, INFINITE))
		{
		case WAIT_OBJECT_0:
			{
				if (!GetOverlappedResult(filewatch->m_directory, &overlapped_buffer, &bytes_returned, TRUE))
				{
					ARCORE_ERROR("Failure with GetOverlappedResult: %d\n", GetLastError());
					break;
				}
				bWaitingOnDirectoryRead = false;

				// Skip if there was nothing in the read.
				if (bytes_returned == 0)
				{
					break;
				}

				FILE_NOTIFY_INFORMATION *file_information = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&buffer[0]);
				do
				{
					// Get the changed filename in the buffer
					std::wstring changed_file_w { file_information->FileName, file_information->FileNameLength / sizeof(file_information->FileName[0]) };
					std::string changed_file;
					convert_wstring(changed_file_w, changed_file);

					// If the file passes the filter
					if (FilePassesFilter(changed_file, ".*", filewatch))
					{
						parsed_information.emplace_back(std::string{ changed_file }, kEventMapping.at(file_information->Action));
					}

					// Nothing in the next entry? Then break out of the loop
					if (file_information->NextEntryOffset == 0)
					{
						break;
					}
					// Otherwise continue to the next entry in the buffer
					else
					{
						file_information = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(file_information) + file_information->NextEntryOffset);
					}
				}
				while (true);
				break;
			}
			break;

		case WAIT_OBJECT_0 + 1:
			// quit
			break;

		case WAIT_FAILED:
			break;
		}

		// Queue up the events to be handled by the callback thread.
		{
			std::lock_guard<std::mutex> lock(filewatch->m_locks->m_editEventMutex);
			filewatch->m_editEvents.insert(filewatch->m_editEvents.end(), parsed_information.begin(), parsed_information.end());
		}
		filewatch->m_locks->m_editEventCv.notify_all();
	}
	while (filewatch->m_locks->m_running);

	if (bWaitingOnDirectoryRead)
	{
		// Clean up running async io if we were previously waiting
		CancelIo(filewatch->m_directory);
		GetOverlappedResult(filewatch->m_directory, &overlapped_buffer, &bytes_returned, TRUE);
	}
}

//	RoutineCallbacks(filewatch) : Thread routine for calling callbacks.
// Separate thread assumes that callbacks are going to take a hot minute.
void RoutineCallbacks ( core::io::file::arFileWatch* filewatch )
{
	while ( filewatch->m_locks->m_running )
	{
		std::unique_lock<std::mutex> lock(filewatch->m_locks->m_editEventMutex);

		// Wait for the CV to be notified, when the other thread fills it.
		if ( filewatch->m_editEvents.empty() && filewatch->m_locks->m_running )
		{
			filewatch->m_locks->m_editEventCv.wait(lock, 
				[filewatch]()
				{ 
					return (filewatch->m_editEvents.size() > 0) || (filewatch->m_locks->m_running == false);
				});
		}

		// Grab the events from the other thread and steal them.
		decltype(filewatch->m_editEvents) callback_information = {};
		std::swap(callback_information, filewatch->m_editEvents);
		lock.unlock();

		// Loop through all events and call the callback with the given changes.
		for (const auto& file : callback_information)
		{
			if ( filewatch->m_callback != nullptr )
			{
				filewatch->m_callback(file.first, file.second);
			}
		}
	}
}

core::io::file::arFileWatch* core::io::file::BeginFileWatch ( const char* filepath, arFileWatchCallback callback )
{
	// First create watcher on stack
	core::io::file::arFileWatch filewatch;

	// If there's no callback, there's no point to watch files.
	if (callback == NULL)
	{
		return NULL;
	}

	// Create file to watch
	GetDirectory(filepath, &filewatch);
	if (filewatch.m_directory == NULL)
	{
		return NULL;
	}

	// Create needed events
	filewatch.m_eventReadDir = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!filewatch.m_eventReadDir)
	{
		return NULL;
	}
	filewatch.m_eventClosing = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!filewatch.m_eventClosing)
	{
		CloseHandle(filewatch.m_eventReadDir);
		return NULL;
	}

	// Create the lock structure
	filewatch.m_locks = new arFileWatchLocks;

	// Set up the callback
	filewatch.m_callback = callback;

	// If everything is valid, copy the filewatch to the heap
	core::io::file::arFileWatch* output_filewatch = new core::io::file::arFileWatch(std::move(filewatch));

	// Spin up the threads
	output_filewatch->m_callbackThread = std::thread(RoutineCallbacks, output_filewatch);
	core::threads::SetThreadName(output_filewatch->m_callbackThread, ("FW_Cb_" + output_filewatch->m_pathFilename).c_str());

	output_filewatch->m_monitorThread = std::thread(RoutineMonitorDirectory, output_filewatch);
	core::threads::SetThreadName(output_filewatch->m_monitorThread, ("FW_Mon_" + output_filewatch->m_pathFilename).c_str());

	return output_filewatch;
}

void core::io::file::EndFileWatch ( arFileWatch* fileWatch )
{
	if (fileWatch != nullptr)
	{
		fileWatch->m_locks->m_running = false;
		
		// Trigger close event to kill the watch thread
		SetEvent(fileWatch->m_eventClosing);
		fileWatch->m_monitorThread.join();

		// Trigger CV to kill the callback thread
		fileWatch->m_locks->m_editEventCv.notify_all();
		fileWatch->m_callbackThread.join();

		// Close the directory watch
		CloseHandle(fileWatch->m_directory);

		// Destroy events
		CloseHandle(fileWatch->m_eventReadDir);
		CloseHandle(fileWatch->m_eventClosing);

		delete fileWatch->m_locks;
		delete fileWatch;
	}
}

#endif