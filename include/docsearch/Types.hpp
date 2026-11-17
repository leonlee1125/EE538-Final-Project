#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace docsearch {

using DocId = std::uint32_t;

struct Posting {
    DocId doc_id;
    std::size_t term_frequency;
};

struct SearchResult {
    DocId doc_id;
    double score;
};

struct DocumentInfo {
    DocId doc_id;
    std::size_t length;
};

struct TermEntry {
    std::string term;
    std::vector<Posting> postings;
};

struct IndexSnapshot {
    std::vector<DocumentInfo> documents;
    std::vector<TermEntry> terms;
};

}  // namespace docsearch