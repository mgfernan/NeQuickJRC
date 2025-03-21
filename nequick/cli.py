"""
Module that contains the client interface for the NeQuick model, which is
a wrapper around the NeQuick JRC executable.
"""
import argparse
import datetime
import sys

from . import Coefficients, to_gim
from .gim import GimFileHandler

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

    coefficients = Coefficients.from_array(args.coefficients)

    gim_handler = GimFileHandler(sys.stdout)
    if args.output_file is not None:
        with open(args.output_file, 'wt') as f:
            gim_handler = GimFileHandler(f)

    to_gim(coefficients, args.epoch, gim_handler=gim_handler)


def _parse_datetime(datetime_str):
    """Parse a datetime string in ISO 8601 format."""
    try:
        return datetime.datetime.fromisoformat(datetime_str)
    except ValueError:
        raise argparse.ArgumentTypeError(f"Invalid datetime format: '{datetime_str}'. Use ISO 8601 format (e.g., '2025-03-20T12:34:56').")
