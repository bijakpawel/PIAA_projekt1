#!/usr/bin/env bash
# Skrypt budujący. Kompiluje benchmark i demo z optymalizacją -O2.
# Uruchamiać z katalogu głównego projektu:
#   ./build.sh
set -euo pipefail

CXX=${CXX:-g++}
CXXFLAGS=${CXXFLAGS:-"-std=c++17 -O2 -Wall -Wextra -pedantic"}

mkdir -p build

echo "[build] $CXX benchmark.cpp -> build/benchmark"
$CXX $CXXFLAGS -Isrc src/benchmark.cpp -o build/benchmark

echo "[build] $CXX demo.cpp -> build/demo"
$CXX $CXXFLAGS -Isrc src/demo.cpp -o build/demo

echo "[build] OK"
