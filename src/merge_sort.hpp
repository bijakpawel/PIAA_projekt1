#ifndef PIAA_MERGE_SORT_HPP
#define PIAA_MERGE_SORT_HPP

#include <cstddef>
#include <functional>
#include <vector>
#include <utility>

namespace piaa {

// Stabilny merge in-place na buforze pomocniczym.
// Scala [lo, mid) i [mid, hi) w arr, używając buf jako kopii.
template <typename T, typename Compare>
void merge(T* arr, T* buf, std::size_t lo, std::size_t mid, std::size_t hi, Compare cmp) {
    for (std::size_t i = lo; i < hi; ++i) {
        buf[i] = arr[i];
    }
    std::size_t i = lo;
    std::size_t j = mid;
    std::size_t k = lo;
    while (i < mid && j < hi) {
        // Warunek !cmp(buf[j], buf[i]) daje stabilność: przy równych bierzemy z lewej.
        if (!cmp(buf[j], buf[i])) {
            arr[k++] = buf[i++];
        } else {
            arr[k++] = buf[j++];
        }
    }
    while (i < mid) arr[k++] = buf[i++];
    while (j < hi) arr[k++] = buf[j++];
}

template <typename T, typename Compare>
void mergeSortImpl(T* arr, T* buf, std::size_t lo, std::size_t hi, Compare cmp) {
    if (hi - lo <= 1) return;
    std::size_t mid = lo + (hi - lo) / 2;
    mergeSortImpl(arr, buf, lo, mid, cmp);
    mergeSortImpl(arr, buf, mid, hi, cmp);
    merge(arr, buf, lo, mid, hi, cmp);
}

// Sortowanie przez scalanie (top-down), złożoność O(n log n) we wszystkich przypadkach.
// Alokuje jeden bufor rozmiaru n.
template <typename T, typename Compare = std::less<T>>
void mergeSort(T* arr, std::size_t n, Compare cmp = Compare()) {
    if (n < 2) return;
    std::vector<T> buf(n);
    mergeSortImpl(arr, buf.data(), 0, n, cmp);
}

} // namespace piaa

#endif
