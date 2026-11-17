#pragma once

#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

namespace docsearch {

template <typename T, typename Compare = std::less<T>>
class MinHeap {
public:
    bool empty() const {
        return values_.empty();
    }

    std::size_t size() const {
        return values_.size();
    }

    // Call top() and pop() only when the heap is nonempty.
    const T& top() const {
        return values_.front();
    }

    void push(const T& value) {
        values_.push_back(value);
        std::size_t child = values_.size() - 1;

        while (child > 0) {
            const std::size_t parent = (child - 1) / 2;
            if (!compare_(values_[child], values_[parent])) {
                break;
            }

            std::swap(values_[child], values_[parent]);
            child = parent;
        }
    }

    void pop() {
        if (values_.size() == 1) {
            values_.pop_back();
            return;
        }

        values_.front() = std::move(values_.back());
        values_.pop_back();
        std::size_t parent = 0;

        while (parent < values_.size() / 2) {
            std::size_t child = 2 * parent + 1;
            const std::size_t right = child + 1;

            if (right < values_.size() && compare_(values_[right], values_[child])) {
                child = right;
            }
            if (!compare_(values_[child], values_[parent])) {
                break;
            }

            std::swap(values_[parent], values_[child]);
            parent = child;
        }
    }

private:
    std::vector<T> values_;
    Compare compare_{};
};

}  // namespace docsearch