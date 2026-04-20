// Główny program benchmarkujący. Uruchamia każdy z trzech algorytmów
// (merge sort, quicksort z medianą z trzech, introsort) na 100 tablicach
// intów w 5 rozmiarach i 9 scenariuszach rozkładu danych.
//
// Wyniki są zapisywane do dwóch plików CSV:
//   results/raw.csv     — czas pojedynczego uruchomienia [ms]
//   results/summary.csv — średnia, odchylenie std, min, max po 100 powtórzeniach
//
// Użycie:
//   benchmark [--quick]
//     --quick : skrócony tryb (mniejsze rozmiary, 10 powtórzeń) do testów

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "merge_sort.hpp"
#include "quick_sort.hpp"
#include "intro_sort.hpp"
#include "generator.hpp"
#include "verify.hpp"

namespace {

struct Scenario {
    const char* name;
    // -1 => random, -2 => reverse sorted, inne => fraction*1000 (np. 250 = 25%)
    int code;
};

const Scenario kScenarios[] = {
    {"random",        -1},
    {"sorted_25",     250},
    {"sorted_50",     500},
    {"sorted_75",     750},
    {"sorted_95",     950},
    {"sorted_99",     990},
    {"sorted_99_7",   997},
    {"reverse",       -2},
};

enum class Algo { Merge, Quick, Intro };

const char* algoName(Algo a) {
    switch (a) {
        case Algo::Merge: return "merge";
        case Algo::Quick: return "quick_med3";
        case Algo::Intro: return "intro";
    }
    return "?";
}

// Generuje tablicę zgodnie ze scenariuszem i ziarnem RNG.
std::vector<int> makeArray(std::size_t n, const Scenario& s, std::uint64_t seed) {
    std::mt19937_64 rng(seed);
    if (s.code == -1) {
        return piaa::randomArray(n, rng);
    }
    if (s.code == -2) {
        return piaa::reverseSortedArray<std::less<int>>(n, rng);
    }
    double fraction = static_cast<double>(s.code) / 1000.0;
    return piaa::partiallySortedArray<std::less<int>>(n, fraction, rng);
}

// Uruchamia pojedynczy algorytm na kopii tablicy i zwraca czas w ms.
double timeSort(Algo a, std::vector<int> data) {
    auto t0 = std::chrono::steady_clock::now();
    switch (a) {
        case Algo::Merge: piaa::mergeSort(data.data(), data.size()); break;
        case Algo::Quick: piaa::quickSort(data.data(), data.size()); break;
        case Algo::Intro: piaa::introSort(data.data(), data.size()); break;
    }
    auto t1 = std::chrono::steady_clock::now();
    if (!piaa::isSorted(data.data(), data.size())) {
        std::fprintf(stderr, "!!! %s produced unsorted output\n", algoName(a));
        std::exit(2);
    }
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

struct Stats {
    double mean = 0.0;
    double stddev = 0.0;
    double minV = 0.0;
    double maxV = 0.0;
};

Stats computeStats(const std::vector<double>& xs) {
    Stats s;
    if (xs.empty()) return s;
    double sum = 0.0;
    s.minV = xs[0];
    s.maxV = xs[0];
    for (double x : xs) {
        sum += x;
        if (x < s.minV) s.minV = x;
        if (x > s.maxV) s.maxV = x;
    }
    s.mean = sum / static_cast<double>(xs.size());
    double var = 0.0;
    for (double x : xs) {
        double d = x - s.mean;
        var += d * d;
    }
    var /= static_cast<double>(xs.size());
    s.stddev = std::sqrt(var);
    return s;
}

} // namespace

int main(int argc, char** argv) {
    bool quick = false;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--quick") == 0) quick = true;
    }

    std::vector<std::size_t> sizes;
    int repetitions;
    if (quick) {
        sizes = {1000, 5000, 10000};
        repetitions = 10;
    } else {
        sizes = {10000, 50000, 100000, 500000, 1000000};
        repetitions = 100;
    }

    const Algo algos[] = {Algo::Merge, Algo::Quick, Algo::Intro};

    std::ofstream raw("results/raw.csv");
    std::ofstream summary("results/summary.csv");
    if (!raw.is_open() || !summary.is_open()) {
        std::fprintf(stderr, "Nie mozna otworzyc results/*.csv - czy folder istnieje?\n");
        return 1;
    }
    raw << "algorithm,size,scenario,iteration,time_ms\n";
    summary << "algorithm,size,scenario,repetitions,mean_ms,stddev_ms,min_ms,max_ms\n";

    // Deterministyczne ziarno bazowe, dla powtarzalności.
    const std::uint64_t kBaseSeed = 0xC0FFEEULL;

    for (std::size_t n : sizes) {
        for (const auto& sc : kScenarios) {
            // Pre-generujemy 100 tablic, aby wszystkie algorytmy sortowały dokładnie te same dane.
            std::vector<std::vector<int>> arrays(repetitions);
            for (int i = 0; i < repetitions; ++i) {
                std::uint64_t seed = kBaseSeed ^ (static_cast<std::uint64_t>(n) * 1315423911ULL)
                                              ^ (static_cast<std::uint64_t>(sc.code + 10) * 2654435761ULL)
                                              ^ (static_cast<std::uint64_t>(i) * 97531ULL);
                arrays[i] = makeArray(n, sc, seed);
            }

            for (Algo a : algos) {
                std::vector<double> times;
                times.reserve(repetitions);
                for (int i = 0; i < repetitions; ++i) {
                    double ms = timeSort(a, arrays[i]); // kopia wewnątrz timeSort
                    times.push_back(ms);
                    raw << algoName(a) << ',' << n << ',' << sc.name << ','
                        << i << ',' << ms << '\n';
                }
                Stats st = computeStats(times);
                summary << algoName(a) << ',' << n << ',' << sc.name << ','
                        << repetitions << ',' << st.mean << ',' << st.stddev << ','
                        << st.minV << ',' << st.maxV << '\n';
                std::fprintf(stderr,
                    "[done] algo=%-10s n=%-8zu scenario=%-12s mean=%.3f ms\n",
                    algoName(a), n, sc.name, st.mean);
            }
            raw.flush();
            summary.flush();
        }
    }

    std::fprintf(stderr, "\nZapisano: results/raw.csv, results/summary.csv\n");

    return 0;
}
