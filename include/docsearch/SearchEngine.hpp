#pragma once

#include "IndexView.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace docsearch {

enum class TopKMethod { Heap, FullSort };

std::vector<DocId> searchAnd(
    const IndexView& index,
    const std::vector<std::string>& terms);

std::vector<SearchResult> searchTopK(
    const IndexView& index,
    const std::vector<std::string>& terms,
    std::size_t k,
    TopKMethod method = TopKMethod::Heap);

}  // namespace docsearch