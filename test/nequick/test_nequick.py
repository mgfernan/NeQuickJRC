import datetime
import glob
import os
import pytest

from nequick import NeQuick


# Path to the benchmark folder
BENCHMARK_FOLDER = os.path.join(os.path.dirname(__file__), "../benchmark")

BENCHMARK_FILES = glob.glob(os.path.join(BENCHMARK_FOLDER, "*"))

param_array = [pytest.param(f, id=os.path.basename(f)) for f in BENCHMARK_FILES]

@pytest.mark.parametrize("input_file", param_array)
def test__benchmarks(input_file: str):
    """
    Test the NeQuick model
    """

    with open(input_file, "rt") as f:
        lines = f.readlines()

        # Parse the input file
        coefficients = list(map(float, lines[0].split()))
        assert len(coefficients) == 3

        nequick = NeQuick(*coefficients)

        for test_case in lines[1:]:
            month, ut, sta_lon, sta_lat, sta_hgt, sat_lon, sat_lat, sat_hgt, expected_stec = \
                map(float, test_case.split())

            epoch = datetime.datetime(2025, int(month), 21, int(ut), 0, 0)
            print(epoch)

            stec = nequick.compute_stec(epoch,
                                        sta_lon, sta_lat, sta_hgt,
                                        sat_lon, sat_lat, sat_hgt)

            assert stec == pytest.approx(expected_stec, abs=1e-3)
