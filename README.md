# NeQuickG and Python package

## Background
Galileo is the European global navigation satellite system providing a highly accurate and global positioning service
under civilian control. Galileo, and in general current GNSS, are based on the broadcasting of electromagnetic ranging
signals in the L frequency band. Those satellite signals suffer from a number of impairments when propagating
through the Earth’s ionosphere. Receivers  operating  in  single  frequency  mode  may  use  the  single  frequency
ionospheric  correction  algorithm  NeQuickG to  estimate  the ionospheric delay on each satellite link.<br>

The implementation has been written in the C programming language with a Python extension and is divided in:

 - The NeQuickG JRC library (lib)
 - The test driver program (app)
 - A Python module that is built using the C code

## Basic usage of the Python module

The Python library provides an interface to work with the NeQuick model. Below is an example of how to use the library to compute VTEC and STEC values.

The Python module can be installed with:

```bash
pip install nequick
```

### Example

The following script will output the computed STEC and VTEC values for the given epoch and coordinates.

This example demonstrates how to initialize the NeQuick model, set the model coefficients, and compute the STEC and VTEC.

```python
from datetime import datetime
from nequick import NeQuick

# Initialize the NeQuick model with coefficients
nequick = NeQuick(236.831641, -0.39362878, 0.00402826613)

# Define the epoch and coordinates
epoch = datetime(2025, 3, 21, 12, 0, 0)
station_lon, station_lat, station_alt = 40.0, -3.0, 0.0
sat_lon, sat_lat, sat_alt = 45.0, -2.0, 20000000.0

# Compute STEC (Slant Total Electron Content) between the station and the satellite
stec = nequick.compute_stec(epoch, station_lat, station_lon, station_alt, sat_lat, sat_lon, sat_alt)
print(f"STEC: {stec}")

# Compute VTEC (Vertical Total Electron Content) at the station location
vtec = nequick.compute_vtec(epoch, station_lat, station_lon)
print(f"VTEC: {vtec}")
```

## References
 - European GNSS (Galileo) Open Service. Ionospheric Correction Algorithm for Galileo Single Frequency Users, 1.2 September 2016
 - C standard ISO/IEC 9899:2011

## Compilation options

The library has been refactored so that CMake is used as the compiler manager.

Note that the `FTR_MODIP_CCIR_AS_CONSTANTS` has been set, therefore the CCIR
grid and the MODIP files are preloaded as constants in the library

To compile the library using CMAKE, follow these instructions)

```bash
cd build
cmake [-DCMAKE_BUILD_TYPE=Debug] ..
make
make install  # optional to make the executables and library system-wide
```

This will create a library file `libnequick.a`, that can be used for other programs.
To use the exeecutable:

```bash
# Execute the program to show a list of available options
NeQuickG_JRC.exe -h
```

## Acknowledgements

The NeQuick electron density model was developed by the Abdus Salam International Center of Theoretical
Physics (ICTP) and the University of Graz. The adaptation of NeQuick for the Galileo single-frequency ionospheric
correction algorithm (NeQuick G) has been performed by the European Space Agency (ESA) involving the original
authors and other European ionospheric scientists under various ESA contracts. The step-by-step algorithmic
description of NeQuick for Galileo has been a collaborative effort of IC TP, ESA and the European Commission, including JRC.

This code is based on a fork from the code published in Github by `odrisci`
