#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"
if [ ! -x ./libyang ]; then
  go build -ldflags="-s -w" -o libyang .
fi
./libyang -xml sample.xml -out sample.h
