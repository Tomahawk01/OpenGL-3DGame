#include "Compress.h"

#include "Exception.h"

#include <zstd.h>

namespace Game {

	std::vector<std::byte> Compress(std::span<const std::byte> data)
	{
		const auto compressedBufferSize = ZSTD_compressBound(data.size_bytes());
		auto compressedBuffer = std::vector<std::byte>(compressedBufferSize);

		const auto compressedSize = ZSTD_compress(compressedBuffer.data(), compressedBuffer.size(), data.data(), data.size_bytes(), 1);

		if (ZSTD_isError(compressedSize))
		{
			throw Exception("Failed to compress data: {}", ZSTD_getErrorName(compressedSize));
		}

		return compressedBuffer;
	}

}
