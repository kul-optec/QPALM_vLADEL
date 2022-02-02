# Proximal Augmented Lagrangian method for Quadratic Programs


QPALM is a numerical optimization package that finds stationary points of (possibly **nonconvex**) quadratic programs, that is 
```
minimize        0.5 x' Q x + q' x

subject to      l <= A x <= u
```

## Documentation

You can now find the the documentation [here](https://tttapa.github.io/QPALM-minimal/). This includes all information you need to get started using QPALM.

## Benchmarks

Check out the paper below for detailed benchmark tests comparing QPALM with state-of-the-art solvers.

 * [QPALM: A Newton-type Proximal Augmented Lagrangian Method for Quadratic Programs](https://arxiv.org/abs/1911.02934)
 * [QPALM: A Proximal Augmented Lagrangian Method for Nonconvex Quadratic Programs](https://arxiv.org/abs/2010.02653)

## Citing

If you use QPALM in your research, please cite the following paper
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

## License

QPALM is licensed under LGPL v3.0. Some modules are used in this software: 
* LADEL: authored by Ben Hermans and licensed under [LGPL-v3](https://github.com/Benny44/LADEL/blob/master/LICENSE).
* LOBPCG: the version of LOBPCG used here was written by Ben Hermans and licensed under the GNU Lesser General Public License v3.0, see [LOBPCG/LICENSE](https://github.com/Benny44/LOBPCG/blob/master/LICENSE).
* LAPACK: authored by The University of Tennessee and The University of Tennessee Research Foundation, The University of California Berkeley, and The University of Colorado Denver, and licensed under BSD-3, see [here](https://github.com/Reference-LAPACK/lapack/blob/master/LICENSE).
* Minunit: a minimal unit testing framework for C, modified from the version by David Siñuela Pastor and licensed under MIT, see [here](https://github.com/siu/minunit/blob/master/MIT-LICENSE.txt).

