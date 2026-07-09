# NeQuickG JRC C library and Python package

NeQuick is an ionospheric electron-density model used by Galileo single-frequency users to estimate ionospheric delay.
This repository contains:

- The core C implementation (`lib`)
- A C test application (`app`)
- A Python package (`nequick`) with a compiled extension and a CLI

## Install

```bash
pip install nequick
```

Current package version: `1.1.0`.

## Python API quick start

```python
from datetime import datetime
from nequick import NeQuick

model = NeQuick(236.831641, -0.39362878, 0.00402826613)
epoch = datetime(2025, 3, 21, 12, 0, 0)

# Vertical TEC at one ionospheric pierce point
vtec = model.compute_vtec(epoch, 40.0, -3.0)
print(f"VTEC: {vtec}")

# Slant TEC between receiver and satellite
stec = model.compute_stec(
    epoch,
    40.0, -3.0, 0.0,
    45.0, -2.0, 20_000_000.0,
)
print(f"STEC: {stec}")

# Electron-density profile along a ray (distance_km, height_m, Ne[m^-3])
profile = model.compute_ne_profile(
    epoch,
    0.0, 0.0, 100_000.0,
    0.0, 0.0, 130_000.0,
    10.0,  # step in km
)
print(profile[:2])
```

## CLI

The package installs the `nequick` command.

### 1) GIM generation (`gim`)

Generate a Global Ionospheric Map (VTEC grid) to stdout:

```bash
nequick gim \
    --coefficients 236.831641,-0.39362878,0.00402826613 \
    --epoch 2025-03-21T12:00:00
```

Write to a file:

```bash
nequick gim \
    --coefficients 236.831641,-0.39362878,0.00402826613 \
    --epoch 2025-03-21T12:00:00 \
    --output-file gim.txt
```

Backward compatibility is preserved: calling `nequick --coefficients ...` without a subcommand still routes to the GIM workflow.

### 2) Electron-density profile (`ne-profile`, alias `ne-prof`)

Compute samples along a ray and print CSV to stdout:

```bash
nequick ne-profile \
    --coefficients 236.831641,-0.39362878,0.00402826613 \
    --epoch-gps-s 1426910400 \
    --lon-start-deg 0.0 --lat-start-deg 0.0 --h-start-m 100000.0 \
    --lon-stop-deg 0.0 --lat-stop-deg 0.0 --h-stop-m 130000.0 \
    --step-m 10000
```

Key options:

- `--step-m`: sampling step in meters (must be > 0)
- `--min-iono-height-m` and `--max-iono-height-m`: output filtering by height
- `--output`: write CSV to file (`-` means stdout)

## Build from source (C/CMake)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Optional install:

```bash
cmake --install build
```

The build uses `FTR_MODIP_CCIR_AS_CONSTANTS`, so CCIR and MODIP data are embedded as constants.

## Run tests

```bash
cmake --build build -j
ctest --test-dir build -E unit_test
pytest -v
```

## References

- European GNSS (Galileo) Open Service. Ionospheric Correction Algorithm for Galileo Single Frequency Users, 1.2 (September 2016)
- C standard ISO/IEC 9899:2011

## Acknowledgements

The NeQuick electron-density model was developed by ICTP and the University of Graz. The adaptation to the Galileo single-frequency algorithm (NeQuick G) was performed by ESA with the original authors and other European ionospheric scientists under ESA contracts. The step-by-step NeQuick for Galileo algorithm description was a collaborative effort of ICTP, ESA and the European Commission (including JRC).

This code is based on a fork from code published by `odrisci`.
