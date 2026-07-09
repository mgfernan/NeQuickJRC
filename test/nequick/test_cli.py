import datetime
import os
import subprocess
import tempfile
import sys

from nequick import cli, to_gim


def _run_nequick_cli(args):
    return subprocess.run(
        [sys.executable, "-m", "nequick", *args],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

def test__cli():

    proc = _run_nequick_cli(['--coefficients', '236.831641', '-0.39362878', '0.00402826613'])

    assert proc.returncode == 0

    doc = proc.stdout.decode()

    assert '# epoch: ' in doc
    assert '# longitude:' in doc
    assert '# latitude:' in doc

    assert len(doc.splitlines()) == 74


def test__cli_ne_profile_help():
    proc = _run_nequick_cli(["ne-profile", "-h"])

    assert proc.returncode == 0

    doc = proc.stdout.decode()
    assert "Compute electron-density samples (ne profile) for one ray segment" in doc
    assert "--epoch-gps-s" in doc
    assert "--lon-start-deg" in doc
    assert "--h-stop-m" in doc


def test__cli_ne_profile_alias_help():
    proc = _run_nequick_cli(["ne-prof", "-h"])

    assert proc.returncode == 0
    doc = proc.stdout.decode()
    assert "--epoch-gps-s" in doc


def test__cli_ne_profile_stdout():
    proc = _run_nequick_cli(
        [
            "ne-profile",
            "--coefficients", "236.831641", "-0.39362878", "0.00402826613",
            "--epoch-gps-s", "1426910400",
            "--lon-start-deg", "0.0",
            "--lat-start-deg", "0.0",
            "--h-start-m", "100000.0",
            "--lon-stop-deg", "0.0",
            "--lat-stop-deg", "0.0",
            "--h-stop-m", "130000.0",
            "--step-m", "10000",
        ]
    )

    assert proc.returncode == 0

    lines = proc.stdout.decode().strip().splitlines()
    assert lines[0] == "distance_m,height_m,electron_density_e_m3"
    assert len(lines) == 5


def test__cli_ne_profile_output_file():
    with tempfile.TemporaryDirectory() as tmp_dir:
        output_file = os.path.join(tmp_dir, "profile.csv")

        proc = _run_nequick_cli(
            [
                "ne-profile",
                "--coefficients", "236.831641", "-0.39362878", "0.00402826613",
                "--epoch-gps-s", "1426910400",
                "--lon-start-deg", "0.0",
                "--lat-start-deg", "0.0",
                "--h-start-m", "100000.0",
                "--lon-stop-deg", "0.0",
                "--lat-stop-deg", "0.0",
                "--h-stop-m", "130000.0",
                "--output", output_file,
            ]
        )

        assert proc.returncode == 0
        assert os.path.exists(output_file)

        with open(output_file, "rt", encoding="utf-8") as f:
            rows = f.read().strip().splitlines()

        assert rows[0] == "distance_m,height_m,electron_density_e_m3"
        assert len(rows) >= 2

