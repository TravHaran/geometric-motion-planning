#!/bin/bash

set -e

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
THEORY_DIR="$PROJECT_ROOT/theory"

cd "$THEORY_DIR"

echo "Compiling theory notes..."

pdflatex -interaction=nonstopmode -halt-on-error theory.tex
pdflatex -interaction=nonstopmode -halt-on-error theory.tex

echo "Theory compiled successfully."

open theory.pdf