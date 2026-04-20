#ifndef PIAA_GENERATOR_HPP
#define PIAA_GENERATOR_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>
#include <functional>

namespace piaa {

// Generator tablic testowych.
// Wszystkie funkcje używają std::mt19937_64 dla powtarzalności przy tym samym seed.

// Losowa tablica intów w pełnym zakresie int32.
inline std::vector<int> randomArray(std::size_t n, std::mt19937_64& rng) {
    std::uniform_int_distribution<int> dist(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max()
    );
    std::vector<int> v(n);
    for (std::size_t i = 0; i < n; ++i) v[i] = dist(rng);
    return v;
}

// Tablica z pierwszymi fraction*n elementami już posortowanymi (rosnąco),
// a resztą losową. Modeluje sytuację, gdy część danych była już uporządkowana.
template <typename Compare = std::less<int>>
std::vector<int> partiallySortedArray(std::size_t n, double fraction,
                                      std::mt19937_64& rng,
                                      Compare cmp = Compare()) {
    auto v = randomArray(n, rng);
    std::size_t k = static_cast<std::size_t>(fraction * static_cast<double>(n));
    if (k > n) k = n;
    std::sort(v.begin(), v.begin() + k, cmp);
    return v;
}

// Tablica posortowana malejąco (gdy cmp = std::less, czyli "rosnąco" jest docelowe).
// Dla dowolnego cmp: elementy w kolejności ODWROTNEJ niż chciane uporządkowanie.
template <typename Compare = std::less<int>>
std::vector<int> reverseSortedArray(std::size_t n, std::mt19937_64& rng,
                                    Compare cmp = Compare()) {
    auto v = randomArray(n, rng);
    // Sortujemy odwrotnym komparatorem -> worst case dla wejściowego cmp.
    std::sort(v.begin(), v.end(),
              [&](const int& a, const int& b) { return cmp(b, a); });
    return v;
}

} // namespace piaa

#endif
