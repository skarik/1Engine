#ifndef GPU_WRAPPER_TRANSFER_H_
#define GPU_WRAPPER_TRANSFER_H_

#include "core/types/types.h"
#include "renderer/types/types.h"

namespace gpu
{
	enum TransferStyle
	{
		// Discard the previous data in the buffer.
		kTransferWriteDiscardPrevious = 0x01,

		// Keep the previous data in the buffer, but not for reading.
		kTransferWrite = 0x02,

		// Transfer mode specifically for reading from the GPU on readback buffers.
		kTransferRead = 0x04,
	};
}

#endif//GPU_WRAPPER_TRANSFER_H_