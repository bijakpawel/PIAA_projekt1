"""Generuje tabele LaTeX z results/summary.csv -> report/tables/*.tex.

Dla każdego scenariusza tworzy jedną tabelę: wiersze = rozmiary, kolumny = algorytmy
(średni czas w ms +/- stddev).

Uruchomienie (z katalogu głównego projektu):
    py plots/make_tex_tables.py
"""
from __future__ import annotations

import sys
from pathlib import Path

import pandas as pd

ROOT = Path(__file__).resolve().parent.parent
SUMMARY = ROOT / "results" / "summary.csv"
OUT_DIR = ROOT / "report" / "tables"

ALGO_ORDER = ["merge", "quick_med3", "intro"]
ALGO_HEAD = {
    "merge": "Merge sort",
    "quick_med3": "Quicksort (med-3)",
    "intro": "Introsort",
}

SCENARIO_LABEL = {
    "random": "dane losowe",
    "sorted_25": "25\\,\\% posortowane",
    "sorted_50": "50\\,\\% posortowane",
    "sorted_75": "75\\,\\% posortowane",
    "sorted_95": "95\\,\\% posortowane",
    "sorted_99": "99\\,\\% posortowane",
    "sorted_99_7": "99,7\\,\\% posortowane",
    "reverse": "tablica odwrócona",
}


def fmt_val(mean: float, stddev: float) -> str:
    # np. 12.34 \pm 0.56
    return f"${mean:.3f} \\pm {stddev:.3f}$"


def make_table(df: pd.DataFrame, scenario: str) -> str:
    sub = df[df["scenario"] == scenario].copy()
    if sub.empty:
        return ""
    sizes = sorted(sub["size"].unique())
    lines = []
    lines.append("\\begin{table}[H]")
    lines.append("\\centering")
    lines.append(f"\\caption{{Średni czas sortowania [ms] $\\pm$ odch. std. "
                 f"(100 powtórzeń) — {SCENARIO_LABEL[scenario]}.}}")
    lines.append(f"\\label{{tab:summary_{scenario}}}")
    lines.append("\\begin{tabular}{rccc}")
    lines.append("\\toprule")
    header = "$n$ & " + " & ".join(ALGO_HEAD[a] for a in ALGO_ORDER) + " \\\\"
    lines.append(header)
    lines.append("\\midrule")
    for n in sizes:
        row = [f"\\num{{{n}}}"]
        for algo in ALGO_ORDER:
            r = sub[(sub["algorithm"] == algo) & (sub["size"] == n)]
            if r.empty:
                row.append("---")
            else:
                row.append(fmt_val(float(r["mean_ms"].iloc[0]),
                                   float(r["stddev_ms"].iloc[0])))
        lines.append(" & ".join(row) + " \\\\")
    lines.append("\\bottomrule")
    lines.append("\\end{tabular}")
    lines.append("\\end{table}")
    return "\n".join(lines) + "\n"


def main() -> None:
    if not SUMMARY.exists():
        sys.exit(f"Brak {SUMMARY}. Najpierw uruchom benchmark.")
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    df = pd.read_csv(SUMMARY)
    for scenario in SCENARIO_LABEL.keys():
        tex = make_table(df, scenario)
        if not tex:
            continue
        out = OUT_DIR / f"summary_{scenario}.tex"
        out.write_text(tex, encoding="utf-8")
        print(f"  -> {out.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
