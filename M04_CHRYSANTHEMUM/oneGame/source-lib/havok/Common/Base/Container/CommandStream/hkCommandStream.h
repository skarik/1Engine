/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_COMMAND_STREAM_H
#define HK_COMMAND_STREAM_H

#include <Common/Base/Container/BlockStream/hkBlockStream.h>

class hkOstream;

/// Base class of a command.
/// Currently commands are used so that different parts of Havok can communicate with each other.
/// Each command has a primary and a secondary type. This is done to allow for a more flexible way
/// to configure and custom implement command dispatchers.
/// Commands should stream-able between platforms of the same endianness like between ppu and spu; 
/// therefore they have no vtable.
class hkCommand
{
	public:

		/// Primary types are used
		enum PrimaryType
		{
			TYPE_DEBUG_DISPLAY,

			TYPE_PHYSICS_API,		///< like body::setVelocity
			TYPE_PHYSICS_INTERNAL,
			TYPE_PHYSICS_EVENTS,

			TYPE_DESTRUCTION_API,
			TYPE_DESTRUCTION_INTERNAL,
			TYPE_DESTRUCTION_EVENTS,

			TYPE_MAX
		};

		hkCommand( PrimaryType type, hkUint16 subType, int sizeInBytes ): m_sizeInBytes( hkUint16(HK_NEXT_MULTIPLE_OF(16,sizeInBytes))), m_filterBits(0), m_primaryType(type), m_secondaryType(subType) {}

		int getSizeInBytes() const { return m_sizeInBytes; }
	public:
		hkUint16 m_sizeInBytes;

		hkUint8  m_filterBits;	///< Allows for selectively executing commands

		hkEnum<PrimaryType, hkUint8> m_primaryType;

		hkUint16 m_secondaryType;

		/// Sometimes commands are created by multiple threads. To ensure deterministic order, 
		/// all commands could be resorted using \a m_deterministicSortKey before they are executed
		hkUint16 m_deterministicSortKey;
};



/// Command dispatcher which dispatches commands using the secondary type
class hkSecondaryCommandDispatcher: public hkReferencedObject
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BASE );
		virtual ~hkSecondaryCommandDispatcher(){}
		virtual void exec( const hkCommand& command )= 0;

		/// optional print function for debugging
		virtual void print( const hkCommand& command, hkOstream& stream ) const {}
};

/// Implementation of hkCommandProcessor which streams into a block stream.
class hkBlockStreamCommandWriter HK_ON_CPU( :public hkSecondaryCommandDispatcher )
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(0);

		hkBlockStreamCommandWriter(  )
		{
			m_deterministicSortKey = 0;
		}

		// non virtual function of exec
		HK_ON_CPU(void append( const hkCommand& command ));

		HK_FORCE_INLINE void* allocBuffer( int size );

		void exec( const hkCommand& command );

	public:
		/// deterministic sort which will be automatically stored in the hkCommand
		HK_PAD_ON_SPU(hkUint16) m_deterministicSortKey;		
		hkBlockStream<hkCommand>::Writer m_writer;
};

/// command dispatcher which dispatches using the primary type
class hkPrimaryCommandDispatcher
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BASE, hkPrimaryCommandDispatcher);
		hkPrimaryCommandDispatcher();

		~hkPrimaryCommandDispatcher();

		void registerDispatcher( hkCommand::PrimaryType type, hkSecondaryCommandDispatcher* dispatcher );

		void exec( const hkCommand& command );

		void print( const hkCommand& command, hkOstream& stream );

	public:

		hkRefPtr<hkSecondaryCommandDispatcher> m_commandDispatcher[ hkCommand::TYPE_MAX ];
};


#include <Common/Base/Container/CommandStream/hkCommandStream.inl>

#endif //HK_COMMAND_STREAM_H

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
