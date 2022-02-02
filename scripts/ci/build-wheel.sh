#!/usr/bin/env bash
cd "$( dirname "${BASH_SOURCE[0]}" )"/../..

export CXXFLAGS="-march=skylake -static-libstdc++ -static-libgcc"
export LDFLAGS="-static-libstdc++ -static-libgcc"
export CFLAGS="-march=skylake -static-libgcc"
python3 -m build .
auditwheel repair --plat manylinux_2_27_x86_64 dist/*.whl
