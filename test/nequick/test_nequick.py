import datetime
import gc
import glob
import os
import pytest
import weakref

from nequick import NeQuick, Coefficients


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

            assert stec == pytest.approx(expected_stec, abs=2e-1)


def test__nequick_coefficients():
    """
    Test the NeQuick model coefficients
    """

    # Test the coefficients
    nequick = Coefficients(236.831641, -0.39362878, 0.00402826613)

    assert nequick.a0 == 236.831641
    assert nequick.a1 == -0.39362878
    assert nequick.a2 == 0.00402826613

    coeffs_dict = nequick.to_dict()
    assert coeffs_dict["a0"] == 236.831641
    assert coeffs_dict["a1"] == -0.39362878
    assert coeffs_dict["a2"] == 0.00402826613


def test__compute_stec_receiver_above_transmitter():
    nequick = NeQuick(236.831641, -0.39362878, 0.00402826613)

    stec = nequick.compute_stec(
        datetime.datetime(2025, 3, 21, 12, 0, 0),
        0.0, 0.0, 10000000.0,
        20.0, 0.0, 5000000.0,
    )

    assert stec > 0.0


def test__compute_stec_limb_geometry_supported():
    nequick = NeQuick(236.831641, -0.39362878, 0.00402826613)

    stec = nequick.compute_stec(
        datetime.datetime(2025, 3, 21, 12, 0, 0),
        -20.0, 0.0, 700000.0,
        20.0, 0.0, 700000.0,
    )

    assert stec > 0.0


def test__compute_stec_raises_on_bad_ray():
    nequick = NeQuick(236.831641, -0.39362878, 0.00402826613)

    with pytest.raises(RuntimeError):
        nequick.compute_stec(
            datetime.datetime(2025, 3, 21, 12, 0, 0),
            -30.0, 0.0, 700000.0,
            30.0, 0.0, 700000.0,
        )


def test__compute_ne_profile_vertical_geometry_default_step():
    nequick = NeQuick(236.831641, -0.39362878, 0.00402826613)

    profile = nequick.compute_ne_profile(
        datetime.datetime(2025, 3, 21, 12, 0, 0),
        0.0, 0.0, 100000.0,
        0.0, 0.0, 130000.0,
    )

    assert len(profile) == 4
    assert profile[0][0] == pytest.approx(0.0)
    assert profile[-1][0] == pytest.approx(30.0)
    assert profile[0][1] == pytest.approx(100000.0)
    assert profile[-1][1] == pytest.approx(130000.0)
    assert all(len(sample) == 3 for sample in profile)
    assert all(sample[2] >= 0.0 for sample in profile)


def test__compute_ne_profile_supports_custom_step():
    nequick = NeQuick(236.831641, -0.39362878, 0.00402826613)

    profile = nequick.compute_ne_profile(
        datetime.datetime(2025, 3, 21, 12, 0, 0),
        0.0, 0.0, 100000.0,
        0.0, 0.0, 130000.0,
        7.5,
    )

    distances = [sample[0] for sample in profile]
    assert distances == pytest.approx([0.0, 7.5, 15.0, 22.5, 30.0])


def test__compute_ne_profile_supports_slant_geometry():
    nequick = NeQuick(236.831641, -0.39362878, 0.00402826613)

    profile = nequick.compute_ne_profile(
        datetime.datetime(2025, 3, 21, 12, 0, 0),
        -20.0, 0.0, 700000.0,
        20.0, 0.0, 700000.0,
        250.0,
    )

    distances = [sample[0] for sample in profile]
    heights = [sample[1] for sample in profile]

    assert len(profile) > 2
    assert distances[0] == pytest.approx(0.0)
    assert distances[-1] > 0.0
    assert heights[0] == pytest.approx(700000.0)
    assert heights[-1] == pytest.approx(700000.0)
    assert min(heights) < 700000.0
    assert all(sample[2] >= 0.0 for sample in profile)


def test__compute_ne_profile_rejects_non_positive_step():
    nequick = NeQuick(236.831641, -0.39362878, 0.00402826613)

    with pytest.raises(ValueError):
        nequick.compute_ne_profile(
            datetime.datetime(2025, 3, 21, 12, 0, 0),
            0.0, 0.0, 100000.0,
            0.0, 0.0, 130000.0,
            0.0,
        )


def test__nequick_dealloc_releases_python_object():
    obj = NeQuick(236.831641, -0.39362878, 0.00402826613)
    ref = weakref.ref(obj)

    del obj
    gc.collect()

    assert ref() is None