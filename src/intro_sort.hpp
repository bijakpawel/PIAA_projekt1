#ifndef PIAA_INTRO_SORT_HPP
#define PIAA_INTRO_SORT_HPP

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <functional>
#include <utility>

#include "quick_sort.hpp"

namespace piaa {

// Sortowanie przez wstawianie — używane jako finisher dla małych podtablic.
template <typename T, typename Compare>
void insertionSort(T* arr, std::size_t n, Compare cmp) {
    for (std::size_t i = 1; i < n; ++i) {
        T key = std::move(arr[i]);
        std::size_t j = i;
        while (j > 0 && cmp(key, arr[j - 1])) {
            arr[j] = std::move(arr[j - 1]);
            --j;
        }
        arr[j] = std::move(key);
    }
}

// Przesiewanie w dół w kopcu — cmp wyznacza typ kopca.
// Dla cmp = std::less<T> budujemy max-kopiec -> sortowanie rosnące.
template <typename T, typename Compare>
void siftDown(T* arr, std::size_t start, std::size_t end, Compare cmp) {
    std::size_t root = start;
    while (2 * root + 1 < end) {
        std::size_t child = 2 * root + 1;
        if (child + 1 < end && cmp(arr[child], arr[child + 1])) {
            ++child;
        }
        if (!cmp(arr[root], arr[child])) return;
        std::swap(arr[root], arr[child]);
        root = child;
    }
}

// Sortowanie przez kopcowanie — fallback w introsort przy zbyt głębokiej rekurencji.
// Złożoność O(n log n) w najgorszym przypadku.
template <typename T, typename Compare>
void heapSort(T* arr, std::size_t n, Compare cmp) {
    if (n < 2) return;
    for (std::ptrdiff_t start = static_cast<std::ptrdiff_t>(n) / 2 - 1; start >= 0; --start) {
        siftDown(arr, static_cast<std::size_t>(start), n, cmp);
    }
    for (std::size_t end = n - 1; end > 0; --end) {
        std::swap(arr[0], arr[end]);
        siftDown(arr, 0, end, cmp);
    }
}

template <typename T, typename Compare>
void introSortImpl(T* arr, std::ptrdiff_t lo, std::ptrdiff_t hi,
                   std::ptrdiff_t depthLimit, Compare cmp) {
    const std::ptrdiff_t INSERTION_THRESHOLD = 16;
    while (hi - lo > INSERTION_THRESHOLD) {
        if (depthLimit <= 0) {
            heapSort(arr + lo, static_cast<std::size_t>(hi - lo + 1), cmp);
            return;
        }
        --depthLimit;
        std::ptrdiff_t p = partitionMedianOfThree(arr, lo, hi, cmp);
        // Rekurencja w mniejszą część, iteracja w większą.
        if (p - lo < hi - p) {
            introSortImpl(arr, lo, p - 1, depthLimit, cmp);
            lo = p + 1;
        } else {
            introSortImpl(arr, p + 1, hi, depthLimit, cmp);
            hi = p - 1;
        }
    }
    // Finiszer: insertion sort na małym odcinku.
    if (hi > lo) {
        insertionSort(arr + lo, static_cast<std::size_t>(hi - lo + 1), cmp);
    }
}

// Introsort (Musser 1997): quicksort z medianą z trzech, przełączający się
// na heapsort po przekroczeniu głębokości 2*log2(n), z insertion-sortem na końcu.
// Gwarantuje O(n log n) w najgorszym przypadku.
template <typename T, typename Compare = std::less<T>>
void introSort(T* arr, std::size_t n, Compare cmp = Compare()) {
    if (n < 2) return;
    std::ptrdiff_t depthLimit =
        2 * static_cast<std::ptrdiff_t>(std::log2(static_cast<double>(n)));
    introSortImpl(arr, 0, static_cast<std::ptrdiff_t>(n) - 1, depthLimit, cmp);
}

} // namespace piaa

#endif
