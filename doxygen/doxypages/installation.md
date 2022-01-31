# Installation instructions {#installation-md}

## Prerequisites

You'll need a C compiler, CMake, and Make or Ninja. 
To compile the Matlab interface, you need Matlab and MEX as well.

## Download

Download QPALM from <https://github.com/tttapa/QPALM.git> ([direct link](https://github.com/tttapa/QPALM/archive/refs/heads/main.zip)):

```sh
git clone https://github.com/tttapa/QPALM.git --single-branch --depth 1
```
Alternatively, without git
```sh
wget https://github.com/tttapa/QPALM/archive/refs/heads/main.tar.gz -O- | tar xzf -
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

## C/C++ installation

QPALM currently offers no install target for the C/C++ libraries and headers,
but you can easily add it to your own CMake projects using e.g.
`add_subdirectory`. If you do so, include the `QPALM` subfolder (the one with
the `include`, `mex` and `src` directories in it), not the root of the 
directory, which is the developer project (which includes extra warnings, tests,
code coverage, etc.).
