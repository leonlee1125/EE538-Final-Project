#pragma once

#include "Types.hpp"

#include <cstddef>
#include <string_view>
#include <vector>

namespace docsearch {

class IndexView {
public:
    virtual ~IndexView() = default;

    virtual const std::vector<Posting>& postings(std::string_view term) const = 0;
    virtual std::size_t documentLength(DocId id) const = 0;
    virtual std::size_t documentCount() const = 0;
    virtual double averageDocumentLength() const = 0;
};

}  // namespace docsearch