#!/usr/bin/env bash

python_version="${1:-3.10}"
host="${2:-x86_64-centos7-linux-gnu}"
staging="$HOME/GitHub/cross-python/staging/$host"
cross_config=$(mktemp --suffix .cross.toml)
cat > $cross_config <<- EOF
[cmake]
config = ["Debug", "Release"]
generator = "Ninja Multi-Config"
[cmake.options]
CMAKE_FIND_ROOT_PATH = "$staging/pybind11-2.13.6;$staging/eigen"
USE_GLOBAL_PYBIND11 = "On"
EOF
export CFLAGS="-static-libgcc"
export CXXFLAGS="-static-libgcc -static-libstdc++"
python$python_version -m build -w . \
    -C--cross="$cross_config" \
    -C--cross="$staging/cmake/$host.py-build-cmake.cross.toml"
python$python_version -m build -w QPALM/interfaces/python/qpalm-debug \
    -C--cross="$cross_config" \
    -C--cross="$staging/cmake/$host.py-build-cmake.cross.toml" \
    -o dist-debug
