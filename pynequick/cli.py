"""
Module that contains the client interface for the NeQuick model, which is
a wrapper around the NeQuick JRC executable.
"""
from dataclasses import dataclass
import argparse
import datetime
import io
import itertools
import tempfile
import subprocess
import sys
from typing import List

import numpy as np

_LONGITUDES = np.linspace(-180, 180, 73)
_LATITUDES = np.linspace(-87.5, 87.5, 71)

@dataclass
class NequickCoeffs():
    a0: float
    a1: float
    a2: float

    @staticmethod
    def from_array(array: List[float]) -> 'NequickCoeffs':
        return NequickCoeffs(array[0], array[1], array[2])


def to_gim_file(cofficients: NequickCoeffs, epoch:datetime.datetime, fout: io.TextIOWrapper):
    """
    """

    with tempfile.NamedTemporaryFile(mode='wt') as temp_file, \
        tempfile.NamedTemporaryFile(mode='wt') as temp_file_output:

        for lat_deg, lon_deg in itertools.product(_LATITUDES, _LONGITUDES):

            month = epoch.month
            ut = epoch.hour + epoch.minute / 60 + epoch.second / 3600

            temp_file.write(f'{month} {ut} {lon_deg} {lat_deg} 0 {lon_deg} {lat_deg} 25000000\n')

        temp_file.flush()
        temp_file.seek(0)

        process = subprocess.run(
            ['NeQuickJRC', '-f', str(cofficients.a0), str(cofficients.a1), str(cofficients.a2), temp_file.name, temp_file_output.name],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )

        if process.returncode != 0:
            raise subprocess.CalledProcessError(process.returncode, 'NeQuickJRC')

        temp_file_output.seek(0)

        # Read the VTEC values from the output files (last column)
        with open(temp_file_output.name, 'rt') as f:
            vtec_values = np.loadtxt(f, usecols=(9,))
            vtec_values = np.reshape(vtec_values, (len(_LATITUDES), len(_LONGITUDES)))

        # Parse the output file and write the GIM file
        lon_start = _LONGITUDES[0]
        lon_end = _LONGITUDES[-1]
        lon_n = len(_LONGITUDES)

        lat_start = _LATITUDES[0]
        lat_end = _LATITUDES[-1]
        lat_n = len(_LATITUDES)

        header = f'# epoch: {epoch.isoformat()}\n' + \
                 f'# longitude:: start: {lon_start:7.2f} end: {lon_end:7.2f} n: {lon_n:3d}\n' + \
                 f'# latitude:: start: {lat_start:7.2f} end: {lat_end:7.2f} n: {lat_n:3d}\n'

        fout.write(header)

        np.savetxt(fout, vtec_values, "%7.3f")


def main():
    """
    Entry point for the client interface
    """

    parser = argparse.ArgumentParser(description='NeQuick JRC client interface')
    parser.add_argument(
        '--coefficients',
        type=float,
        nargs=3,
        required=True,
        metavar=('a0', 'a1', 'a2'),
        help='The three NeQuick model coefficients (a0, a1, a2)'
    )
    parser.add_argument(
        '--epoch',
        type=_parse_datetime,
        required=False, default=datetime.datetime.now(),
        help="The epoch (date and time) in ISO 8601 format (e.g., '2025-03-20T12:34:56')."
    )
    parser.add_argument('--output_file', type=str, default=None, required=False,
                        help='Output file. If not provided, the output will be written to stdout')
    args = parser.parse_args()

    coefficients = NequickCoeffs.from_array(args.coefficients)

    fout = sys.stdout if args.output_file is None else args.output_file
    to_gim_file(coefficients, args.epoch, fout)


def _parse_datetime(datetime_str):
    """Parse a datetime string in ISO 8601 format."""
    try:
        return datetime.datetime.fromisoformat(datetime_str)
    except ValueError:
        raise argparse.ArgumentTypeError(f"Invalid datetime format: '{datetime_str}'. Use ISO 8601 format (e.g., '2025-03-20T12:34:56').")
