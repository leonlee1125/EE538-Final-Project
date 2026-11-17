#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace docsearch {

// Split text on non-ASCII-alphanumeric characters and lowercase each token.
std::vector<std::string> tokenize(std::string_view text);

}  // namespace docsearch