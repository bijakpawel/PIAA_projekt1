#ifndef PIAA_QUICK_SORT_HPP
#define PIAA_QUICK_SORT_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>

namespace piaa {

// Partycjonowanie Lomuto z pivotem wybranym metodą mediany z trzech
// (pierwszy, środkowy, ostatni). Zwraca końcowy indeks pivota.
template <typename T, typename Compare>
std::ptrdiff_t partitionMedianOfThree(T* arr, std::ptrdiff_t lo, std::ptrdiff_t hi, Compare cmp) {
    std::ptrdiff_t mid = lo + (hi - lo) / 2;
    // Po trzech porównaniach: arr[lo] <= arr[mid] <= arr[hi].
    if (cmp(arr[mid], arr[lo])) std::swap(arr[lo], arr[mid]);
    if (cmp(arr[hi],  arr[lo])) std::swap(arr[lo], arr[hi]);
    if (cmp(arr[hi],  arr[mid])) std::swap(arr[mid], arr[hi]);
    // Mediana jest w arr[mid]. Przenosimy ją na koniec do Lomuto.
    std::swap(arr[mid], arr[hi]);
    T pivot = arr[hi];
    std::ptrdiff_t i = lo - 1;
    for (std::ptrdiff_t j = lo; j < hi; ++j) {
        if (cmp(arr[j], pivot)) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[hi]);
    return i + 1;
}

// Iteracyjna rekurencja z eliminacją ogona: zawsze rekurujemy w mniejszą część,
// a w większej kontynuujemy pętlę. Dzięki temu stos ma głębokość O(log n).
template <typename T, typename Compare>
void quickSortImpl(T* arr, std::ptrdiff_t lo, std::ptrdiff_t hi, Compare cmp) {
    while (lo < hi) {
        std::ptrdiff_t p = partitionMedianOfThree(arr, lo, hi, cmp);
        if (p - lo < hi - p) {
            quickSortImpl(arr, lo, p - 1, cmp);
            lo = p + 1;
        } else {
            quickSortImpl(arr, p + 1, hi, cmp);
            hi = p - 1;
        }
    }
}

// Szybkie sortowanie z pivotem wybieranym metodą mediany z trzech.
// Średnio O(n log n). Przy złośliwym wyborze pivota pesymistycznie O(n^2),
// ale mediana z trzech unika O(n^2) dla tablic posortowanych/odwróconych.
template <typename T, typename Compare = std::less<T>>
void quickSort(T* arr, std::size_t n, Compare cmp = Compare()) {
    if (n < 2) return;
    quickSortImpl(arr, 0, static_cast<std::ptrdiff_t>(n) - 1, cmp);
}

} // namespace piaa

#endif
