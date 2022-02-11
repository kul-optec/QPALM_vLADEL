# Installation instructions {#installation-md}

These are the instructions for building QPALM from source. If you just want to
install the pre-built binaries, please see the [main page](index.html).

## Prerequisites

You'll need a C compiler, CMake, and Make or Ninja. 
To compile the Matlab interface, you need Matlab and MEX as well. For the Python
interface, you need the Python development files, which are not installed by
default on some systems. For example, on Ubuntu, install the `python3-dev`
package.

## Download

Download QPALM from <https://github.com/kul-optec/QPALM>:

```sh
git clone https://github.com/kul-optec/QPALM.git --single-branch --depth 1 --recursive
cd QPALM
```
If you already cloned it without the `--recursive` argument, use:
```sh
git submodule update --init --recursive
```

## Matlab installation

### Build

Open a terminal inside of the QPALM repository, and configure and build the 
project:

```sh
cmake -B build -S QPALM -D CMAKE_BUILD_TYPE=Release -D QPALM_WITH_MEX=On -D CMAKE_POSITION_INDEPENDENT_CODE=On
```
```sh
cmake --build build --config Release -j
```

### Install

On Linux, Matlab automatically adds `~/Documents/MATLAB` to the path, so it's easiest install QPALM there:

```sh
cmake --install build --config Release --component mex_interface --prefix ~/Documents/MATLAB
```

### Uninstall

To uninstall QPALM, simply remove the `+qpalm` folder from where you installed it:

```sh
rm -r "~/Documents/MATLAB/+qpalm"
```

## Python installation

QPALM is available on [PyPI](https://pypi.org/project/qpalm/), with binaries
for Windows and Linux (x86_64 and arm64). To install it use:
```sh
python3 -m pip install qpalm
```

To build the Python bindings yourself, create a virtual environment:
```sh
# Linux
python3 -m venv py-venv
. ./py-venv/bin/activate
```
```sh
# Windows
python3 -m venv py-venv
./py-venv/Scripts/Activate.ps1
```
Then install [Eigen](https://eigen.tuxfamily.org):
```sh
# Linux
./scripts/install-eigen.sh
```
```sh
# Windows
./scripts/install-eigen.ps1
```
And finally, install the package:
```sh
pip install .
```

## Julia installation

Usually, you'll use tools like BinaryBuilder.jl instead of compiling it manually,
but if you need to, you can:
```sh
cmake -B build -S QPALM \
    -D CMAKE_BUILD_TYPE=Release \
    -D QPALM_WITH_JULIA=On \
    -D CMAKE_POSITION_INDEPENDENT_CODE=On
cmake --build build --config Release -j
cmake --install build --config Release --component julia_modules --prefix $PWD/staging-julia
```

## C/C++ installation

To install the C and C++ libraries and headers, simply follow the standard 
CMake configure, build, install instructions:

```sh
cmake -B build -S QPALM \
    -D CMAKE_BUILD_TYPE=Release \
    -D QPALM_WITH_CXX=On \
    -D CMAKE_POSITION_INDEPENDENT_CODE=On
cmake --build build --config Release -j
cmake --install build --config Release --prefix /usr/local
cmake --install build/QPALM/LADEL --prefix /usr/local
```

If you just need the shared libraries, you can use:
```sh
cmake -B build -S QPALM \
    -D CMAKE_BUILD_TYPE=Release \
    -D QPALM_WITH_CXX=On \
    -D CMAKE_POSITION_INDEPENDENT_CODE=On \
    -D BUILD_SHARED_LIBS=On
cmake --build build --config Release -j
cmake --install build --config Release --component shlib --prefix /usr/local
cmake --install build/LADEL --component shlib --prefix /usr/local
```