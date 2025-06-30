#include "Decompress.h"

#include "Exception.h"
#include "Error.h"

#include <zstd.h>

namespace Game {

	std::vector<std::byte> Decompress(std::span<const std::byte> data)
	{
		const auto decompressedBufferSize = ZSTD_getFrameContentSize(data.data(), data.size_bytes());
		Expect(decompressedBufferSize != ZSTD_CONTENTSIZE_ERROR, "Not compressed by zstd");
		Expect(decompressedBufferSize != ZSTD_CONTENTSIZE_UNKNOWN, "Cannot get original size");

		auto decompressedBuffer = std::vector<std::byte>(decompressedBufferSize);

		ZSTD_decompress(decompressedBuffer.data(), decompressedBuffer.size(), data.data(), data.size_bytes());

		return decompressedBuffer;
	}

}
