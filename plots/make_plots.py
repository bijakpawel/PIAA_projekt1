"""Generuje wykresy z results/summary.csv.

Wymaga: matplotlib, pandas.
Uruchomienie (z katalogu głównego projektu):
    py plots/make_plots.py
"""
from __future__ import annotations

import os
import sys
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

ROOT = Path(__file__).resolve().parent.parent
SUMMARY = ROOT / "results" / "summary.csv"
OUT_DIR = ROOT / "plots"

ALGO_ORDER = ["merge", "quick_med3", "intro"]
ALGO_LABEL = {
    "merge": "Merge sort",
    "quick_med3": "Quicksort (med-of-3)",
    "intro": "Introsort",
}
ALGO_COLOR = {
    "merge": "#1f77b4",
    "quick_med3": "#d62728",
    "intro": "#2ca02c",
}
ALGO_MARKER = {
    "merge": "o",
    "quick_med3": "s",
    "intro": "^",
}

SCENARIO_ORDER = [
    "random",
    "sorted_25",
    "sorted_50",
    "sorted_75",
    "sorted_95",
    "sorted_99",
    "sorted_99_7",
    "reverse",
]
SCENARIO_LABEL = {
    "random": "losowa",
    "sorted_25": "25% posortowane",
    "sorted_50": "50% posortowane",
    "sorted_75": "75% posortowane",
    "sorted_95": "95% posortowane",
    "sorted_99": "99% posortowane",
    "sorted_99_7": "99.7% posortowane",
    "reverse": "odwrocona",
}


def load() -> pd.DataFrame:
    if not SUMMARY.exists():
        sys.exit(f"Brak {SUMMARY}. Najpierw uruchom benchmark.")
    df = pd.read_csv(SUMMARY)
    return df


def plot_time_vs_size(df: pd.DataFrame) -> None:
    """Dla każdego scenariusza: czas vs rozmiar (log-log), 3 linie (algorytmy)."""
    for scenario in SCENARIO_ORDER:
        sub = df[df["scenario"] == scenario]
        if sub.empty:
            continue
        fig, ax = plt.subplots(figsize=(7, 4.5))
        for algo in ALGO_ORDER:
            s = sub[sub["algorithm"] == algo].sort_values("size")
            if s.empty:
                continue
            ax.plot(
                s["size"], s["mean_ms"],
                marker=ALGO_MARKER[algo], color=ALGO_COLOR[algo],
                label=ALGO_LABEL[algo], linewidth=1.6,
            )
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_xlabel("Rozmiar tablicy n")
        ax.set_ylabel("Sredni czas [ms]")
        ax.set_title(f"Czas sortowania - scenariusz: {SCENARIO_LABEL[scenario]}")
        ax.grid(True, which="both", linestyle=":", linewidth=0.5)
        ax.legend()
        fig.tight_layout()
        out = OUT_DIR / f"time_vs_size__{scenario}.png"
        fig.savefig(out, dpi=150)
        plt.close(fig)
        print(f"  -> {out.relative_to(ROOT)}")


def plot_bars_per_size(df: pd.DataFrame) -> None:
    """Dla każdego rozmiaru: słupki pogrupowane per scenariusz, 3 algorytmy."""
    sizes = sorted(df["size"].unique())
    for n in sizes:
        sub = df[df["size"] == n]
        if sub.empty:
            continue
        fig, ax = plt.subplots(figsize=(9, 4.8))
        import numpy as np
        x = np.arange(len(SCENARIO_ORDER))
        width = 0.26
        for k, algo in enumerate(ALGO_ORDER):
            vals = []
            for sc in SCENARIO_ORDER:
                row = sub[(sub["algorithm"] == algo) & (sub["scenario"] == sc)]
                vals.append(float(row["mean_ms"].iloc[0]) if not row.empty else 0.0)
            ax.bar(x + (k - 1) * width, vals, width,
                   color=ALGO_COLOR[algo], label=ALGO_LABEL[algo])
        ax.set_xticks(x)
        ax.set_xticklabels([SCENARIO_LABEL[s] for s in SCENARIO_ORDER],
                           rotation=30, ha="right")
        ax.set_ylabel("Sredni czas [ms]")
        ax.set_title(f"Porownanie algorytmow dla n = {n:,}".replace(",", " "))
        ax.grid(True, axis="y", linestyle=":", linewidth=0.5)
        ax.legend()
        fig.tight_layout()
        out = OUT_DIR / f"bars_n_{n}.png"
        fig.savefig(out, dpi=150)
        plt.close(fig)
        print(f"  -> {out.relative_to(ROOT)}")


def plot_normalized(df: pd.DataFrame) -> None:
    """Czas / (n log n) vs n — pokazuje, czy empiryka pasuje do O(n log n)."""
    import numpy as np
    scenarios_to_plot = ["random", "sorted_99_7", "reverse"]
    for scenario in scenarios_to_plot:
        sub = df[df["scenario"] == scenario]
        if sub.empty:
            continue
        fig, ax = plt.subplots(figsize=(7, 4.5))
        for algo in ALGO_ORDER:
            s = sub[sub["algorithm"] == algo].sort_values("size").copy()
            if s.empty:
                continue
            s["norm"] = s["mean_ms"] / (s["size"] * np.log2(s["size"]))
            ax.plot(
                s["size"], s["norm"] * 1e6,
                marker=ALGO_MARKER[algo], color=ALGO_COLOR[algo],
                label=ALGO_LABEL[algo], linewidth=1.6,
            )
        ax.set_xscale("log")
        ax.set_xlabel("Rozmiar tablicy n")
        ax.set_ylabel("Czas / (n * log2 n)  [ns]")
        ax.set_title(f"Znormalizowany czas - scenariusz: {SCENARIO_LABEL[scenario]}")
        ax.grid(True, which="both", linestyle=":", linewidth=0.5)
        ax.legend()
        fig.tight_layout()
        out = OUT_DIR / f"normalized__{scenario}.png"
        fig.savefig(out, dpi=150)
        plt.close(fig)
        print(f"  -> {out.relative_to(ROOT)}")


def main() -> None:
    OUT_DIR.mkdir(exist_ok=True)
    df = load()
    print(f"Wczytano {len(df)} wierszy z {SUMMARY.relative_to(ROOT)}")
    print("\n[1/3] Czas vs rozmiar (per scenariusz)")
    plot_time_vs_size(df)
    print("\n[2/3] Slupki (per rozmiar)")
    plot_bars_per_size(df)
    print("\n[3/3] Znormalizowane: czas / (n log n)")
    plot_normalized(df)
    print("\nGotowe.")


if __name__ == "__main__":
    main()
