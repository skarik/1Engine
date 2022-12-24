#define QOI_IMPLEMENTATION
#include "qoi/qoi.h"
#undef  QOI_IMPLEMENTATION

#include "qoi.hpp"

namespace core {
namespace texture {
namespace qoi {
	int qoi_write(const char *filename, const void *data, const qoi_desc *desc)
	{
		return ::qoi_write(filename, data, desc);
	}
	void *qoi_read(const char *filename, qoi_desc *desc, int channels)
	{
		return ::qoi_read(filename, desc, channels);
	}
	void *qoi_encode(const void *data, const qoi_desc *desc, int *out_len)
	{
		return ::qoi_encode(data, desc, out_len);
	}
	void *qoi_decode(const void *data, int size, qoi_desc *desc, int channels)
	{
		return ::qoi_decode(data, size, desc, channels);
	}
	void *qoi_decode2(const void *data, int size, qoi_desc *desc, int channels, void *out_pixels)
	{
		return ::qoi_decode2(data, size, desc, channels, out_pixels);
	}
}}}