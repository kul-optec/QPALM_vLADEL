#!/usr/bin/env bash
cd "$( dirname "${BASH_SOURCE[0]}" )"/..

if [ -z "${VIRTUAL_ENV+x}" ]; then
    echo "No active virtual environment, refusing to install."
    exit 1
fi

build_type="${1:-RelWithDebInfo}"

set -ex
export CMAKE_PREFIX_PATH="$VIRTUAL_ENV:$CMAKE_PREFIX_PATH"
export PKG_CONFIG_PATH="$VIRTUAL_ENV/lib/pkgconfig:$PKG_CONFIG_PATH"

pushd /tmp

# GoogleTest framework
rm -rf googletest
git clone --single-branch --depth=1 --branch main \
    https://github.com/google/googletest.git
pushd googletest
cmake -Bbuild -S. \
    -D CMAKE_INSTALL_PREFIX="$VIRTUAL_ENV" \
    -D CMAKE_BUILD_TYPE="${build_type}"
cmake --build build -j
cmake --install build
popd

popd
