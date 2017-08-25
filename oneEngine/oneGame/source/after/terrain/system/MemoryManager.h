
#ifndef _AFTER_TERRAIN_SYSTEM_MEMORY_MANAGER_
#define _AFTER_TERRAIN_SYSTEM_MEMORY_MANAGER_

#include "core/common.h"
#include "core/types/types.h"

#include "after/types/terrain/BlockData.h"

#include <mutex>

namespace Terrain
{

	class MemoryManager
	{

	public:
		explicit			MemoryManager ( void );
							~MemoryManager( void );

		//===============================================================================================//
		// Memory Requests
		//===============================================================================================//

		terra_b*			NewDataBlock  ( void );
		void				FreeDataBlock ( terra_b* block );

		//===============================================================================================//
		// State queries
		//===============================================================================================//

		Real				GetMemoryUsage ( void );

	private:
		bool Init ( void );
		void Free ( void );

		static uint32_t	MemorySize_Sidebuffer;
		static uint32_t	MemorySize_Payload;
		static uint32_t	MemorySize_Node;

			uint32_t	m_memoryMaxBlocks;
			char*		m_memoryData;
			char*		m_memoryUsage;

			std::mutex	m_memoryLock;
	};
};

#endif//_AFTER_TERRAIN_SYSTEM_MEMORY_MANAGER_