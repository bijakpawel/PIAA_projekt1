// Program demonstracyjny pokazujacy, ze zaimplementowane algorytmy
// (merge sort, quicksort z mediana z trzech, introsort) dzialaja poprawnie
// dla tablic roznych typow dzieki uzyciu szablonow (templates).
//
// Demonstruje tez parametr sposobu uporzadkowania (rosnaco / malejaco)
// realizowany przez komparator (std::less / std::greater).

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

#include "merge_sort.hpp"
#include "quick_sort.hpp"
#include "intro_sort.hpp"
#include "verify.hpp"

namespace {

template <typename T>
void printArray(const char* label, const std::vector<T>& v) {
    std::printf("%-16s [", label);
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i) std::printf(", ");
        if constexpr (std::is_same_v<T, std::string>) {
            std::printf("\"%s\"", v[i].c_str());
        } else if constexpr (std::is_floating_point_v<T>) {
            std::printf("%.2f", static_cast<double>(v[i]));
        } else {
            std::printf("%lld", static_cast<long long>(v[i]));
        }
    }
    std::printf("]\n");
}

} // namespace

int main() {
    std::printf("=== Demo: sortowanie szablonowe na roznych typach ===\n\n");

    // ---- int, rosnaco (std::less) ----
    {
        std::vector<int> v = {5, 2, -7, 11, 0, 3, -1, 8, 4, 6};
        printArray("int (wejscie)", v);

        auto a = v, b = v, c = v;
        piaa::mergeSort(a.data(), a.size());
        piaa::quickSort(b.data(), b.size());
        piaa::introSort(c.data(), c.size());

        printArray("int merge ASC", a);
        printArray("int quick ASC", b);
        printArray("int intro ASC", c);
        std::printf("  posortowane: merge=%d quick=%d intro=%d\n\n",
                    piaa::isSorted(a.data(), a.size()),
                    piaa::isSorted(b.data(), b.size()),
                    piaa::isSorted(c.data(), c.size()));
    }

    // ---- int, malejaco (std::greater) ----
    {
        std::vector<int> v = {5, 2, -7, 11, 0, 3, -1, 8, 4, 6};
        auto a = v, b = v, c = v;
        piaa::mergeSort(a.data(), a.size(), std::greater<int>());
        piaa::quickSort(b.data(), b.size(), std::greater<int>());
        piaa::introSort(c.data(), c.size(), std::greater<int>());
        printArray("int merge DESC", a);
        printArray("int quick DESC", b);
        printArray("int intro DESC", c);
        std::printf("  posortowane (malejaco): merge=%d quick=%d intro=%d\n\n",
                    piaa::isSorted(a.data(), a.size(), std::greater<int>()),
                    piaa::isSorted(b.data(), b.size(), std::greater<int>()),
                    piaa::isSorted(c.data(), c.size(), std::greater<int>()));
    }

    // ---- double ----
    {
        std::vector<double> v = {3.14, -1.5, 2.71, 0.0, 1.41, -2.2, 9.9, 4.5};
        printArray("double (wejscie)", v);
        auto a = v, b = v, c = v;
        piaa::mergeSort(a.data(), a.size());
        piaa::quickSort(b.data(), b.size());
        piaa::introSort(c.data(), c.size());
        printArray("double merge ASC", a);
        printArray("double quick ASC", b);
        printArray("double intro ASC", c);
        std::printf("\n");
    }

    // ---- std::string (sortowanie leksykograficzne) ----
    {
        std::vector<std::string> v = {"banana", "apple", "lemon", "grape", "kiwi"};
        printArray("string (wejscie)", v);
        auto a = v, b = v, c = v;
        piaa::mergeSort(a.data(), a.size());
        piaa::quickSort(b.data(), b.size());
        piaa::introSort(c.data(), c.size());
        printArray("string merge ASC", a);
        printArray("string quick ASC", b);
        printArray("string intro ASC", c);
        std::printf("\n");
    }

    std::printf("=== Wszystkie algorytmy dzialaja na typach: int, double, std::string ===\n");
    return 0;
}
