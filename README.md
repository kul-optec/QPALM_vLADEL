[![Python Wheels](https://github.com/kul-optec/QPALM/actions/workflows/wheel.yml/badge.svg)](https://github.com/kul-optec/QPALM/actions/workflows/wheel.yml)
[![Matlab Package](https://github.com/kul-optec/QPALM/actions/workflows/matlab.yml/badge.svg)](https://github.com/kul-optec/QPALM/actions/workflows/matlab.yml)
[![Test Coverage](https://img.shields.io/endpoint?url=https://kul-optec.github.io/QPALM/Coverage/shield.io.coverage.json)](https://kul-optec.github.io/QPALM/Coverage/index.html)
[![PyPI - Downloads](https://img.shields.io/pypi/dm/qpalm?label=PyPI)](https://pypi.org/project/qpalm)

# Proximal Augmented Lagrangian method for Quadratic Programs

QPALM is a numerical optimization package that finds stationary points of (possibly **nonconvex**) quadratic programs, that is 
```
minimize        ½ xᵀQx + qᵀx

subject to      l ≤ Ax ≤ u
```

## Documentation

The documentation can be found at: <https://kul-optec.github.io/QPALM/Doxygen>  
Examples are included as well: <https://kul-optec.github.io/QPALM/Doxygen/examples.html>

## Installation

### Python

The QPALM Python interface is available from [PyPI](https://pypi.org/project/qpalm),
you can install it using:
```sh
python3 -m pip install qpalm
```

### Julia

In the Julia console, press `]` to enter the Pkg REPL and install QPALM using:
```sh
add QPALM
```

### Matlab

To install the Matlab interface, download
`qpalm-matlab-{linux,windows,macos}.zip` from the 
[releases page](https://github.com/kul-optec/QPALM/releases/latest), and 
extract it into the `~/Documents/MATLAB` folder.  
As a one-liner in the Matlab console:
```matlab
unzip('https://github.com/kul-optec/QPALM/releases/download/1.1.3b2/qpalm-matlab-linux.zip', userpath)
```
```matlab
unzip('https://github.com/kul-optec/QPALM/releases/download/1.1.3b2/qpalm-matlab-windows.zip', userpath)
```
```matlab
unzip('https://github.com/kul-optec/QPALM/releases/download/1.1.3b2/qpalm-matlab-macos.zip', userpath)
```

### C/C++/Fortran

Pre-built C, C++ and Fortran libraries are available from the [releases page](https://github.com/kul-optec/QPALM/releases).

### Building QPALM from source

For detailed instructions on how to build QPALM from source, please see 
<https://kul-optec.github.io/QPALM/Doxygen/installation-md.html>

## Supported platforms

QPALM is written in C, with interfaces for C++, Python, Julia, Matlab and Fortran.  
The code itself is portable across all major platforms. Binaries are available
for Linux on x86-64 and AArch64<sup>*</sup> and for Windows and macOS on x86-64.

<sup>(*) Except Matlab.</sup>

## Benchmarks

Check out the papers below for detailed benchmark tests comparing QPALM with state-of-the-art solvers.

 * [QPALM: A Newton-type Proximal Augmented Lagrangian Method for Quadratic Programs](https://arxiv.org/abs/1911.02934)
 * [QPALM: A Proximal Augmented Lagrangian Method for Nonconvex Quadratic Programs](https://arxiv.org/abs/2010.02653)

## Citing

If you use QPALM in your research, please cite the following paper:
```bibtex
@inproceedings{hermans2019qpalm,
	author      = {Hermans, B. and Themelis, A. and Patrinos, P.},
	booktitle   = {58th IEEE Conference on Decision and Control},
	title       = {{QPALM}: {A} {N}ewton-type {P}roximal {A}ugmented {L}agrangian {M}ethod for {Q}uadratic {P}rograms},
	year        = {2019},
	volume      = {},
	number      = {},
	pages       = {},
	doi         = {},
	issn        = {},
	month       = {Dec.},
}
```

## Previous versions

The original repository by Ben Hermans at <https://github.com/Benny44/QPALM_vLADEL>
will no longer be maintained.

## License

QPALM is licensed under LGPL v3.0. Some modules are used in this software: 
* LADEL: authored by Ben Hermans and licensed under [LGPL-v3](LICENSE).
* LOBPCG: the version of LOBPCG used here was written by Ben Hermans and licensed under the GNU Lesser General Public License v3.0, see [LOBPCG/LICENSE](https://github.com/Benny44/LOBPCG/blob/master/LICENSE).
* LAPACK: authored by The University of Tennessee and The University of Tennessee Research Foundation, The University of California Berkeley, and The University of Colorado Denver, and licensed under BSD-3, see [here](https://github.com/Reference-LAPACK/lapack/blob/master/LICENSE).
