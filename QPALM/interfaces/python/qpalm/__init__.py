"""Proximal Augmented Lagrangian method for Quadratic Programs"""

__version__ = '1.1.0'

import os
import typing

if not typing.TYPE_CHECKING and os.getenv('QPALM_PYTHON_DEBUG'):
    from qpalm._qpalmd import *
    from qpalm._qpalmd import __version__ as c_version
else:
    from qpalm._qpalm import *
    from qpalm._qpalm import __version__ as c_version
assert __version__ == c_version
