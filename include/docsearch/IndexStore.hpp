#pragma once

#include "Types.hpp"

#include <filesystem>

namespace docsearch {

void saveIndex(const std::filesystem::path& file, const IndexSnapshot& snapshot);
IndexSnapshot loadIndex(const std::filesystem::path& file);

}  // namespace docsearch