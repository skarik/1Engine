#ifndef CORE_ASSETS_TEXLOADER_QOI_HPP_
#define CORE_ASSETS_TEXLOADER_QOI_HPP_

#include "core/types.h"

#include "qoi/qoi.h"

namespace core {
namespace texture {
namespace qoi {
	CORE_API int qoi_write(const char *filename, const void *data, const qoi_desc *desc);
	CORE_API void *qoi_read(const char *filename, qoi_desc *desc, int channels);
	CORE_API void *qoi_encode(const void *data, const qoi_desc *desc, int *out_len);
	CORE_API void *qoi_decode(const void *data, int size, qoi_desc *desc, int channels);
	CORE_API void *qoi_decode2(const void *data, int size, qoi_desc *desc, int channels, void *out_pixels);
}}}

#endif//CORE_ASSETS_TEXLOADER_QOI_HPP_
