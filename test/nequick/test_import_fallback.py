import os
from pathlib import Path
import shutil
import subprocess
import sys

import pytest


def test__init_import_falls_back_when_shadowed_by_namespace_package(tmp_path):
    repo_root = Path(__file__).resolve().parents[2]
    src_pkg = repo_root / "nequick"

    extension_candidates = sorted(src_pkg.glob("_c_ext*.so"))
    if not extension_candidates:
        pytest.skip("No compiled nequick extension found in source tree")

    source_root = tmp_path / "source"
    installed_root = tmp_path / "installed"
    shadow_pkg = source_root / "nequick"
    installed_pkg = installed_root / "nequick"
    shadow_pkg.mkdir(parents=True)
    installed_pkg.mkdir(parents=True)

    shutil.copy2(src_pkg / "__init__.py", shadow_pkg / "__init__.py")
    shutil.copy2(src_pkg / "gim.py", shadow_pkg / "gim.py")

    # Create a namespace package that shadows `nequick._c_ext` on the first import.
    (shadow_pkg / "_c_ext").mkdir()

    extension_binary = extension_candidates[0]
    shutil.copy2(extension_binary, installed_pkg / extension_binary.name)

    env = os.environ.copy()
    env["PYTHONPATH"] = (
        str(source_root)
        + os.pathsep
        + str(installed_root)
        + os.pathsep
        + env.get("PYTHONPATH", "")
    )

    proc = subprocess.run(
        [sys.executable, "-c", "import nequick; print(nequick.NeQuick.__name__)"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env=env,
        text=True,
    )

    assert proc.returncode == 0, proc.stderr
    assert proc.stdout.strip() == "NeQuick"