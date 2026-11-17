#include "docsearch/SearchEngine.hpp"
#include "docsearch/MinHeap.hpp"

#include <cstddef>
#include <string_view>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace docsearch {

std::vector<DocId> searchAnd(
    const IndexView& index,
    const std::vector<std::string>& terms) {
    if (terms.empty()) {
        return {};
    }

    std::vector<DocId> matches;
    const auto& firstPostings = index.postings(terms.front());
    matches.reserve(firstPostings.size());

    for (const auto& posting : firstPostings) {
        matches.push_back(posting.doc_id);
    }

    std::unordered_set<std::string_view> seen{terms.front()};
    for (std::size_t termIndex = 1;
         termIndex < terms.size() && !matches.empty();
         ++termIndex) {
        const auto& term = terms[termIndex];
        if (!seen.insert(term).second) {
            continue;
        }

        const auto& postings = index.postings(term);
        std::vector<DocId> intersection;
        std::size_t left = 0;
        std::size_t right = 0;

        while (left < matches.size() && right < postings.size()) {
            const DocId leftId = matches[left];
            const DocId rightId = postings[right].doc_id;

            if (leftId < rightId) {
                ++left;
            } else if (leftId > rightId) {
                ++right;
            } else {
                intersection.push_back(leftId);
                ++left;
                ++right;
            }
        }

        matches.swap(intersection);
    }

    return matches;
}


std::vector<SearchResult> searchTopK(
    const IndexView& index,
    const std::vector<std::string>& terms,
    std::size_t k,
    TopKMethod method) {
    if (k == 0 || terms.empty()) {
        return {};
    }

    const double documentCount = static_cast<double>(index.documentCount());
    const double averageLength = index.averageDocumentLength();
    if (documentCount == 0 || averageLength <= 0.0) {
        return {};
    }

    constexpr double k1 = 1.2;
    constexpr double b = 0.75;

    std::vector<std::string_view> uniqueTerms(terms.begin(), terms.end());
    std::sort(uniqueTerms.begin(), uniqueTerms.end());
    uniqueTerms.erase(
        std::unique(uniqueTerms.begin(), uniqueTerms.end()), uniqueTerms.end());

    std::unordered_map<DocId, double> scores;
    for (const auto term : uniqueTerms) {
        const auto& postings = index.postings(term);
        const double df = static_cast<double>(postings.size());
        const double idf = std::log1p((documentCount - df + 0.5) / (df + 0.5));

        for (const auto& posting : postings) {
            const double tf = posting.term_frequency;
            const double length = index.documentLength(posting.doc_id);
            const double normalization = k1 * (1.0 - b + b * length / averageLength);
            scores[posting.doc_id] += idf * (tf * (k1 + 1.0)) / (tf + normalization);
        }
    }

    struct RanksWorse {
        bool operator()(const SearchResult& left, const SearchResult& right) const {
            if (left.score != right.score) {
                return left.score < right.score;
            }
            return left.doc_id > right.doc_id;
        }
    };

    const RanksWorse ranksWorse;
    if (method == TopKMethod::Heap) {
        MinHeap<SearchResult, RanksWorse> heap;

        for (const auto& [docId, score] : scores) {
            const SearchResult candidate{docId, score};

            if (heap.size() < k) {
                heap.push(candidate);
            } else if (ranksWorse(heap.top(), candidate)) {
                heap.pop();
                heap.push(candidate);
            }
        }

        std::vector<SearchResult> results;
        results.reserve(heap.size());
        while (!heap.empty()) {
            results.push_back(heap.top());
            heap.pop();
        }

        std::reverse(results.begin(), results.end());
        return results;
    }

    std::vector<SearchResult> results;
    results.reserve(scores.size());
    for (const auto& [docId, score] : scores) {
        results.push_back({docId, score});
    }

    std::sort(results.begin(), results.end(),
              [ranksWorse](const SearchResult& left, const SearchResult& right) {
                  return ranksWorse(right, left);
              });

    if (results.size() > k) {
        results.resize(k);
    }
    return results;
}


}  // namespace docsearch