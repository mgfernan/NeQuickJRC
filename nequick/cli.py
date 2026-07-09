"""CLI entrypoint for the NeQuick Python package."""

import argparse
import datetime
import sys

from . import Coefficients, NeQuick, to_gim
from .gim import GimFileHandler

GPS_EPOCH = datetime.datetime(1980, 1, 6, tzinfo=datetime.timezone.utc)


def main(argv=None):
    """Entry point for the NeQuick client interface."""
    if argv is None:
        argv = sys.argv[1:]

    parser = _build_parser()
    argv = _normalize_coefficients_args(argv)

    # Backward compatibility: if users call `nequick --coefficients ...`,
    # route to the historical GIM workflow.
    if argv and argv[0] not in ("gim", "ne-profile", "ne-prof", "-h", "--help"):
        argv = ["gim", *argv]

    args = parser.parse_args(argv)
    args.handler(args)


def _build_parser():
    parser = argparse.ArgumentParser(description="NeQuick JRC client interface")
    subparsers = parser.add_subparsers(dest="command", required=True)

    parser_gim = subparsers.add_parser(
        "gim",
        help="Compute a Global Ionospheric Map (VTEC grid)",
    )
    parser_gim.add_argument(
        "--coefficients",
        type=_parse_coefficients,
        required=True,
        metavar="a0,a1,a2",
        help="The three NeQuick model coefficients (a0, a1, a2)",
    )
    parser_gim.add_argument(
        "--epoch",
        type=_parse_datetime,
        required=False,
        default=datetime.datetime.now(),
        help="The epoch in ISO 8601 format (e.g., 2025-03-20T12:34:56).",
    )
    parser_gim.add_argument(
        "--output-file",
        type=str,
        default="-",
        required=False,
        help="Output file path. Use '-' for stdout.",
    )
    parser_gim.set_defaults(handler=_run_gim)

    parser_ne_profile = subparsers.add_parser(
        "ne-profile",
        aliases=["ne-prof"],
        description="Compute electron-density samples (ne profile) for one ray segment",
        help="Compute electron-density samples (ne profile) for one ray segment",
    )
    parser_ne_profile.add_argument(
        "--coefficients",
        type=_parse_coefficients,
        required=True,
        metavar="a0,a1,a2",
        help="The three NeQuick model coefficients (a0, a1, a2)",
    )
    parser_ne_profile.add_argument(
        "--step-m",
        type=float,
        default=10000.0,
        help="Sampling step along the ray in meters",
    )
    parser_ne_profile.add_argument(
        "--min-iono-height-m",
        type=float,
        default=60000.0,
        help="Minimum ellipsoidal height to keep in output (meters)",
    )
    parser_ne_profile.add_argument(
        "--max-iono-height-m",
        type=float,
        default=2000000.0,
        help="Maximum ellipsoidal height to keep in output (meters)",
    )
    parser_ne_profile.add_argument(
        "--extrapolation-tolerance-ns",
        type=float,
        default=0.0,
        help="Compatibility option retained for parity with other clients",
    )
    parser_ne_profile.add_argument(
        "--epoch-gps-s",
        type=float,
        required=True,
        help="GPS epoch seconds",
    )
    parser_ne_profile.add_argument(
        "--lon-start-deg",
        type=float,
        required=True,
        help="Start longitude in degrees",
    )
    parser_ne_profile.add_argument(
        "--lat-start-deg",
        type=float,
        required=True,
        help="Start latitude in degrees",
    )
    parser_ne_profile.add_argument(
        "--h-start-m",
        type=float,
        required=True,
        help="Start ellipsoidal height in meters",
    )
    parser_ne_profile.add_argument(
        "--lon-stop-deg",
        type=float,
        required=True,
        help="Stop longitude in degrees",
    )
    parser_ne_profile.add_argument(
        "--lat-stop-deg",
        type=float,
        required=True,
        help="Stop latitude in degrees",
    )
    parser_ne_profile.add_argument(
        "--h-stop-m",
        type=float,
        required=True,
        help="Stop ellipsoidal height in meters",
    )
    parser_ne_profile.add_argument(
        "--output",
        type=str,
        default="-",
        help="Output file path. Use '-' for stdout.",
    )
    parser_ne_profile.set_defaults(handler=_run_ne_profile)

    return parser


def _run_gim(args):
    coefficients = Coefficients.from_array(args.coefficients)

    if args.output_file == "-":
        to_gim(coefficients, args.epoch, gim_handler=GimFileHandler(sys.stdout))
        return

    with open(args.output_file, "wt", encoding="utf-8") as output_stream:
        to_gim(coefficients, args.epoch, gim_handler=GimFileHandler(output_stream))


def _run_ne_profile(args):
    if args.step_m <= 0.0:
        raise argparse.ArgumentTypeError("--step-m must be greater than zero")
    if args.min_iono_height_m > args.max_iono_height_m:
        raise argparse.ArgumentTypeError("--min-iono-height-m cannot be greater than --max-iono-height-m")

    coefficients = Coefficients.from_array(args.coefficients)
    nequick = NeQuick(coefficients.a0, coefficients.a1, coefficients.a2)

    epoch = _gps_seconds_to_datetime(args.epoch_gps_s)
    profile = nequick.compute_ne_profile(
        epoch,
        args.lon_start_deg,
        args.lat_start_deg,
        args.h_start_m,
        args.lon_stop_deg,
        args.lat_stop_deg,
        args.h_stop_m,
        args.step_m / 1000.0,
    )

    filtered_profile = [
        sample
        for sample in profile
        if args.min_iono_height_m <= sample[1] <= args.max_iono_height_m
    ]

    if args.output == "-":
        _write_ne_profile(sys.stdout, filtered_profile)
        return

    with open(args.output, "wt", encoding="utf-8") as output_stream:
        _write_ne_profile(output_stream, filtered_profile)


def _write_ne_profile(stream, profile):
    stream.write("distance_m,height_m,electron_density_e_m3\n")
    for distance_km, height_m, electron_density in profile:
        stream.write(f"{distance_km * 1000.0:.3f},{height_m:.3f},{electron_density:.9e}\n")


def _gps_seconds_to_datetime(epoch_gps_s):
    return (GPS_EPOCH + datetime.timedelta(seconds=epoch_gps_s)).replace(tzinfo=None)


def _normalize_coefficients_args(argv):
    normalized = []
    i = 0

    while i < len(argv):
        token = argv[i]

        if token == "--coefficients" and i + 3 < len(argv):
            v1 = argv[i + 1]
            v2 = argv[i + 2]
            v3 = argv[i + 3]

            if not v1.startswith("--") and not v2.startswith("--") and not v3.startswith("--"):
                normalized.extend(["--coefficients", f"{v1},{v2},{v3}"])
                i += 4
                continue

        normalized.append(token)
        i += 1

    return normalized


def _parse_coefficients(value):
    if isinstance(value, (list, tuple)) and len(value) == 3:
        return [float(value[0]), float(value[1]), float(value[2])]

    parts = [p.strip() for p in str(value).split(",") if p.strip()]
    if len(parts) != 3:
        raise argparse.ArgumentTypeError(
            "--coefficients must contain exactly 3 values: a0,a1,a2"
        )

    try:
        return [float(parts[0]), float(parts[1]), float(parts[2])]
    except ValueError as exc:
        raise argparse.ArgumentTypeError(
            "--coefficients values must be valid floating-point numbers"
        ) from exc


def _parse_datetime(datetime_str):
    """Parse a datetime string in ISO 8601 format."""
    try:
        return datetime.datetime.fromisoformat(datetime_str)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(
            f"Invalid datetime format: '{datetime_str}'. Use ISO 8601 format (e.g., '2025-03-20T12:34:56')."
        ) from exc


if __name__ == "__main__":
    main()
