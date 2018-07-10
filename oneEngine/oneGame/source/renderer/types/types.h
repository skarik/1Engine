#ifndef RENDERER_TYPES_H_
#define RENDERER_TYPES_H_

#include <stdint.h>

typedef uint64_t gpuHandle;
typedef unsigned int gpuEnum;

static_assert(sizeof(gpuHandle) == sizeof(uintptr_t), "Invalid type size.");

#endif//RENDERER_TYPES_H_