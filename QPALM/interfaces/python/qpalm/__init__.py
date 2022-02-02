"""Proximal Augmented Lagrangian method for Quadratic Programs"""

__version__ = '0.0.0a2'

try:
    from qpalm._qpalm import *
    from qpalm._qpalm import __version__ as c_version
    assert __version__ == c_version
except ImportError as e:
    import warnings
    warnings.warn("Failed to import the QPALM C++ extension")