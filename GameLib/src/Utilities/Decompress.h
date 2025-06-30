#pragma once

#include <vector>
#include <span>
#include <cstdint>

namespace Game {

	std::vector<std::byte> Decompress(std::span<const std::byte> data);

}
