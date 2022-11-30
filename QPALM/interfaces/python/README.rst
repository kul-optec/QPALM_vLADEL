Proximal Augmented Lagrangian method for Quadratic Programs
===========================================================
| QPALM is a numerical optimization package that finds stationary points of (possibly **nonconvex**) quadratic programs, that is

.. math::

        \begin{aligned}
            & \underset{x}{\textbf{minimize}}
            & & \tfrac12 x^\top Q x + q^\top x\\
            & \textbf{subject to}
            & & b_\mathrm{min} \le Ax \le b_\mathrm{max} \\
        \end{aligned}

Documentation
-------------
| The documentation can be found at: `<https://kul-optec.github.io/QPALM/Doxygen>`_  
| Examples are included as well: `<https://kul-optec.github.io/QPALM/Doxygen/examples.html>`_

Installation
------------

Python
^^^^^^
| The QPALM Python interface is available from `PyPI <https://pypi.org/project/qpalm>`_, you can install it using:

.. code-block:: sh

    python3 -m pip install qpalm


Julia, Matlab, C/C++/Fortran
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
| Installation instructions for the Julia, Matlab, C, C++ and Fortran interfaces, as well as instructions for building QPALM from source, can be found on `GitHub <https://github.com/kul-optec/QPALM/>`_.

Supported platforms
-------------------
| QPALM is written in C, with interfaces for C++, Python, Julia, Matlab and Fortran. The code itself is portable across all major platforms. Binaries are available for Linux on x86-64, AArch64, ARMv7 and ARMv6, for macOS on x86-64 and ARM64, and for Windows on x86-64.

Benchmarks
----------
| Check out the papers below for detailed benchmark tests comparing QPALM with state-of-the-art solvers.

* `QPALM: A Newton-type Proximal Augmented Lagrangian Method for Quadratic Programs <https://arxiv.org/abs/1911.02934>`_.
* `QPALM: A Proximal Augmented Lagrangian Method for Nonconvex Quadratic Programs <https://arxiv.org/abs/2010.02653>`_.

Citing
------
| If you use QPALM in your research, please cite the following paper:

.. code-block:: bib

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
