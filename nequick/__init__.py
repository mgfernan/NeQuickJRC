from dataclasses import dataclass
import datetime
import importlib.machinery
import importlib.util
import pathlib
import sys
from typing import List

import numpy as np

try:
    from ._c_ext import NeQuick
except ImportError:
    # In source trees, `nequick/_c_ext/` may be treated as a namespace package
    # and shadow the compiled extension module with the same base name.
    _pkg_dir = pathlib.Path(__file__).resolve().parent
    _binary_candidates = sorted(_pkg_dir.glob("_c_ext*"))

    # In CI we may import this source package, while the compiled extension lives
    # in an installed wheel location later on sys.path.
    if not _binary_candidates:
        _pkg_name = __name__.split(".")[0]
        _mod_name = f"{_pkg_name}._c_ext"
        _suffixes = importlib.machinery.EXTENSION_SUFFIXES

        for _path_entry in sys.path:
            if not _path_entry:
                continue

            _candidate_pkg_dir = pathlib.Path(_path_entry) / _pkg_name
            for _suffix in _suffixes:
                _candidate_bin = _candidate_pkg_dir / f"_c_ext{_suffix}"
                if _candidate_bin.exists():
                    _binary_candidates.append(_candidate_bin)
                    break
            if _binary_candidates:
                break

    if not _binary_candidates:
        raise

    _module_name = f"{__name__.split('.')[0]}._c_ext"
    _existing_module = sys.modules.get(_module_name)
    if _existing_module is not None and getattr(_existing_module, "__file__", None) is None:
        # Remove namespace placeholder module so the extension can be loaded.
        del sys.modules[_module_name]

    _spec = importlib.util.spec_from_file_location(
        _module_name, str(_binary_candidates[0])
    )
    if _spec is None or _spec.loader is None:
        raise

    _module = importlib.util.module_from_spec(_spec)
    sys.modules[_module_name] = _module
    _spec.loader.exec_module(_module)
    NeQuick = _module.NeQuick
from .gim import Gim, GimHandler, GimFileHandler

__all__ = ["NeQuick"]

__version__ = "1.1.0"

_LONGITUDES = np.linspace(-180, 180, 73)
_LATITUDES = np.linspace(-87.5, 87.5, 71)

@dataclass
class Coefficients():
    a0: float
    a1: float
    a2: float

    @staticmethod
    def from_array(array: List[float]) -> 'Coefficients':
        return Coefficients(array[0], array[1], array[2])

    def to_dict(self) -> dict:
        """
        Convert the coefficients to a dictionary. Use this method when you want to
        make the contents of the class serializable to JSON or similar formats.

        :return: A dictionary containing the coefficients.
        :rtype: dict
        """
        return {
            "a0": self.a0,
            "a1": self.a1,
            "a2": self.a2
        }


def to_gim(cofficients: Coefficients, epoch: datetime.datetime,
           latitudes: List[float] = _LATITUDES,
           longitudes: List[float] = _LONGITUDES,
           gim_handler: GimHandler = GimFileHandler(sys.stdout)):
    """
    Generate a Global Ionospheric Map (GIM) using the NeQuick model based on the
    given coefficients and epoch.

    This function computes the Vertical Total Electron Content (VTEC) values for a grid of
    latitudes and longitudes using the NeQuick model. The resulting GIM is processed by the
    provided `GimHandler`.

    :param cofficients:
        The NeQuick model coefficients (a0, a1, a2) used for the computation.
        These coefficients define the ionospheric model parameters.
    :type cofficients: Coefficients

    :param epoch:
        The epoch (date and time) for which the GIM is generated. This must be a
        `datetime.datetime` object.
    :type epoch: datetime.datetime

    :param latitudes: List of latitudes for which the GIM shall be computed
    :type latitudes: List[float]

    :param longitudes: List of longitudes for which the GIM shall be computed
    :type longitudes: List[float]

    :param gim_handler:
        An optional handler to process the generated GIM. By default, the GIM is written
        to standard output.
    :type gim_handler: GimHandler

    :return:
        None.

    **Example Usage**::

        from datetime import datetime
        from nequick import Coefficients, to_gim

        coefficients = Coefficients(a0=236.831641, a1=-0.39362878, a2=0.00402826613)
        epoch = datetime(2025, 3, 21, 12, 0, 0)

        to_gim(coefficients, epoch)

    **Notes**:
    - The grid of latitudes and longitudes is predefined as:
        - Latitudes: 71 points from -87.5 to 87.5 degrees.
        - Longitudes: 73 points from -180 to 180 degrees.
    """

    nequick = NeQuick(cofficients.a0, cofficients.a1, cofficients.a2)

    vtec_values = []

    for lat in latitudes:
        lat_row = []
        for lon in longitudes:
            lat_row.append(nequick.compute_vtec(epoch, lon, lat))
        vtec_values.append(lat_row)

    gim = Gim(epoch, longitudes, latitudes, vtec_values)

    gim_handler.process(gim)
