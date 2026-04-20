# PIAA — projekt 1: analiza algorytmów sortowania

Zaimplementowane algorytmy (jako szablony C++17):

- **Merge sort** (sortowanie przez scalanie) — `src/merge_sort.hpp`
- **Quicksort z medianą z trzech** — `src/quick_sort.hpp`
- **Introsort** (Musser 1997) — `src/intro_sort.hpp`

Każdy przyjmuje opcjonalny komparator, co pozwala wybrać uporządkowanie rosnące (`std::less<T>`, domyślnie) lub malejące (`std::greater<T>`).

## Struktura

```
src/          kod źródłowy (3 algorytmy + generator + weryfikator + main)
build/        skompilowane pliki wykonywalne
results/      wyniki benchmarków (CSV)
plots/        wykresy PNG + skrypt make_plots.py
report/       sprawozdanie w LaTeX
build.sh      skrypt kompilacji
```

## Budowanie

Wymagane: `g++` (C++17), `python` + `matplotlib` + `pandas`.

```bash
./build.sh
```

Produkuje `build/benchmark.exe` oraz `build/demo.exe`.

## Uruchamianie

```bash
./build/demo.exe                 # demonstracja szablonów (int, double, string, ASC/DESC)
./build/benchmark.exe            # pełny benchmark (5 rozmiarów × 9 scenariuszy × 100 iter × 3 algorytmy)
./build/benchmark.exe --quick    # skrócony (1k/5k/10k × 10 iter) do szybkiego sanity-check
py plots/make_plots.py           # wykresy z results/summary.csv -> plots/*.png
```

## Scenariusze testowe

Zgodne ze specyfikacją zadania:

| Kod            | Opis |
|----------------|------|
| `random`       | Wszystkie elementy losowe |
| `sorted_25`    | 25% początkowych elementów posortowanych |
| `sorted_50`    | 50% początkowych elementów posortowanych |
| `sorted_75`    | 75% początkowych elementów posortowanych |
| `sorted_95`    | 95% początkowych elementów posortowanych |
| `sorted_99`    | 99% początkowych elementów posortowanych |
| `sorted_99_7`  | 99.7% początkowych elementów posortowanych |
| `reverse`      | Wszystkie elementy posortowane w odwrotnej kolejności |

Rozmiary tablic: 10 000, 50 000, 100 000, 500 000, 1 000 000.
Liczba powtórzeń na konfigurację: 100.
