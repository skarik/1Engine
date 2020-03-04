#ifndef GPU_WRAPPER_TRANSFER_H_
#define GPU_WRAPPER_TRANSFER_H_

#include "core/types/types.h"
#include "renderer/types/types.h"

namespace gpu
{
	enum TransferStyle
	{
		kTransferStatic,
		kTransferStream,
	};
}

#endif//GPU_WRAPPER_TRANSFER_H_