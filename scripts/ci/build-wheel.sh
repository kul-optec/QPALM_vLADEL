#!/usr/bin/env bash
cd "$( dirname "${BASH_SOURCE[0]}" )"/../..

case $(uname -m) in
    x86_64)
        export CXXFLAGS="-march=skylake -static-libstdc++ -static-libgcc"
        export LDFLAGS="-static-libstdc++ -static-libgcc"
        export CFLAGS="-march=skylake -static-libgcc";;
    *)
        export CXXFLAGS="-static-libstdc++ -static-libgcc"
        export LDFLAGS="-static-libstdc++ -static-libgcc"
        export CFLAGS="-static-libgcc";;
esac
python3 -m build .
auditwheel repair --plat manylinux_2_27_$(uname -m) dist/*$(uname -m).whl
