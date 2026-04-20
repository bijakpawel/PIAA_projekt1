#ifndef PIAA_VERIFY_HPP
#define PIAA_VERIFY_HPP

#include <cstddef>
#include <functional>

namespace piaa {

// Sprawdza, czy tablica jest uporządkowana wg komparatora cmp.
// Dla cmp = std::less<T> zwraca true jeżeli elementy są niemalejące (rosnące słabo).
template <typename T, typename Compare = std::less<T>>
bool isSorted(const T* arr, std::size_t n, Compare cmp = Compare()) {
    for (std::size_t i = 1; i < n; ++i) {
        if (cmp(arr[i], arr[i - 1])) return false;
    }
    return true;
}

} // namespace piaa

#endif
