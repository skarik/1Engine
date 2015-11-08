/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_BASE_FILESERVER_STREAMREADER_H
#define HK_BASE_FILESERVER_STREAMREADER_H

class hkSocket;
class hkFileServerStreamReader : public hkStreamReader
{

public:

	hkFileServerStreamReader( hkSocket* connection, hkUint32 id );
	void close();
	virtual ~hkFileServerStreamReader();
	virtual int read( void* buf, int nbytes);
	virtual hkBool isOk() const;

	virtual hkBool seekTellSupported() const;
	virtual hkResult seek(int offset, SeekWhence whence);
	virtual int tell() const;

	enum OutCommands
	{
		READ_CHUNK = 0x11,
		CLOSE = 0x12,
		// Use Seek and Tell sparingly..
		SEEK = 0x13,
		TELL = 0x14,
	};

	enum InCommands
	{
		SEND_CHUNK = 0x11,
		EOF_OR_ERROR = 0x12,
		SEND_TELL = 0x13
	};

	hkSocket* m_socket;
	hkUint32 m_id;
};

#endif //HK_BASE_FILESERVER_STREAMREADER_H

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
