#pragma once

#include <vector>
#include <span>
#include <cstdint>

namespace Game {

	std::vector<std::byte> Compress(std::span<const std::byte> data);

}
