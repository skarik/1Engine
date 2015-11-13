/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_SERVER_FILESYSTEM__H
#define HK_SERVER_FILESYSTEM__H

#include <Common/Base/System/Io/FileSystem/hkNativeFileSystem.h>

#define HK_SERVER_FILE_SYSTEM_DEFAULT_PORT 26002

class hkServerFileSystem : public hkNativeFileSystem
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);

	static hkReferencedObject* create()
	{
		return new hkServerFileSystem(HK_SERVER_FILE_SYSTEM_DEFAULT_PORT);
	}

	
	virtual hkStreamReader* openReader( const char* name );

	virtual hkStreamWriter* openWriter( const char* name );

	virtual hkResult listDirectory(const char* basePath, DirectoryListing& listingOut);

	// Default = VIRTUAL_READWRITE | WAIT_FOR_CONNECT
	enum Mode
	{
		VIRTUAL_READ = 1,
		VIRTUAL_WRITE = 2,
		VIRTUAL_READWRITE = (VIRTUAL_READ | VIRTUAL_WRITE),
		
		VIRTUAL_DIRLIST = 4,
		
		WAIT_FOR_CONNECT = 8,
		
		DEFAULT = (VIRTUAL_READWRITE | VIRTUAL_DIRLIST | WAIT_FOR_CONNECT)
	};

	enum Version
	{
		CUR_VERSION = 0x00010000,
		MIN_COMPAT_VERSION = 0x00010000
	};

	enum OutCommands
	{	
		FILE_READ = 0x01,
		FILE_WRITE = 0x02,
		DIR_LIST = 0x03
	};
	
	enum InCommands
	{	
		NOT_FOUND = 0x01,
		SEND_DIR_LIST = 0x02,
		ACK = 0xAC	
	};

	void setMode( Mode m );
	void shutdown();
	void closeConnection();
	bool waitForConnection();
	bool tryForConnection();

public:

	hkServerFileSystem(int port = HK_SERVER_FILE_SYSTEM_DEFAULT_PORT);
	virtual ~hkServerFileSystem();

protected:

	Mode m_mode;
	int m_connectionPort;
	class hkSocket* m_listenSocket;
	class hkSocket* m_connectSocket;
	
};

#endif // HK_SERVER_FILESYSTEM__H

/*
 * Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20120119)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2012
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available at www.havok.com/tryhavok.
 * 
 */
