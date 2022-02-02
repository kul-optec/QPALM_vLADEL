# Installation instructions {#installation-md}

## Prerequisites

You'll need a C compiler, CMake, and Make or Ninja. 
To compile the Matlab interface, you need Matlab and MEX as well.

## Download

Download QPALM from <https://github.com/tttapa/QPALM-minimal>:

```sh
git clone https://github.com/tttapa/QPALM-minimal.git --single-branch --depth 1 --recursive
cd QPALM-minimal
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

Work in progress ...

## C/C++ installation

QPALM currently offers no install target for the C/C++ libraries and headers,
but you can easily add it to your own CMake projects using e.g.
`add_subdirectory`. If you do so, include the `QPALM` subfolder (the one with
the `include`, `mex` and `src` directories in it), not the root of the 
directory, which is the developer project (which includes extra warnings, tests,
code coverage, etc.).
