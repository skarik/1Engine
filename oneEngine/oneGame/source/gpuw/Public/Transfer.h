#ifndef GPU_WRAPPER_TRANSFER_H_
#define GPU_WRAPPER_TRANSFER_H_

#include "core/types/types.h"
#include "renderer/types/types.h"

namespace gpu
{
	enum TransferStyle
	{
		// Discard the previous data in the buffer.
		kTransferWriteDiscardPrevious,

		// Keep the previous data in the buffer, but not for reading.
		kTransferWrite,

		// Transfer mode specifically for reading from the GPU on readback buffers.
		kTransferRead,
	};
}

#endif//GPU_WRAPPER_TRANSFER_H_