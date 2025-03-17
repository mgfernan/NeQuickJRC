# NeQuickG (NeQuick Galileo) JRC Readme file

## License
@copyright European Union, 2019<br>
 This software has been released as free and open source software
 under the terms of the European Union Public Licence (EUPL), version 1.2,
 https://joinup.ec.europa.eu/sites/default/files/custom-page/attachment/eupl_v1.2_en.pdf <br>
 Questions? Submit your query at https://www.gsc-europa.eu
Version: #NEQUICKG_VERSION<br>

Release date: <b>10/12/2019</b>

## Background
Galileo is the European global navigation satellite system providing a highly accurate and global positioning service
under civilian control. Galileo, and in general current GNSS, are based on the broadcasting of electromagnetic ranging
signals in the L frequency band. Those satellite signals suffer from a number of impairments when propagating
through the Earth’s ionosphere. Receivers  operating  in  single  frequency  mode  may  use  the  single  frequency
ionospheric  correction  algorithm  NeQuickG to  estimate  the ionospheric delay on each satellite link.<br>

The implementation has been written in the C programming language standard 2011 and is divided in:

 - The NeQuickG JRC library (lib)
 - The test driver program (app)

## References
 - European GNSS (Galileo) Open Service. Ionospheric Correction Algorithm for Galileo Single Frequency Users, 1.2 September 2016
 - C standard ISO/IEC 9899:2011

## Requirements
 - One of the toolchains listed in Configuration
 - perl for test execution
 - Doxygen, a documentation generator
 - genhtml, a coverage HTML report generator
 - lcov, a coverage tool

## Compilation options

The library has been refactored so that CMake is used as the compiler manager.

Note that the `FTR_MODIP_CCIR_AS_CONSTANTS` has been set, therefore the CCIR
grid and the MODIP files are preloaded as constants in the library

To compile the library using CMAKE,
 - cd build
 - cmake [-DCMAKE_BUILD_TYPE=Debug] ..
 - make
 - make install (optional, for development)

 This will create a library file libnequick.a

<h3>Usage of the test driver program</h3>
 - open terminal at bin/(selected_compiler)
 - run ./NeQuickG_JRC.exe -h for a list of options.

## Acknowledgements

The NeQuick electron density model was developed by the Abdus Salam International Center of Theoretical
Physics (ICTP) and the University of Graz. The adaptation of NeQuick for the Galileo single-frequency ionospheric
correction algorithm (NeQuick G) has been performed by the European Space Agency (ESA) involving the original
authors and other European ionospheric scientists under various ESA contracts. The step-by-step algorithmic
description of NeQuick for Galileo has been a collaborative effort of IC TP, ESA and the European Commission, including JRC.

This code is based on a fork from the code published in Github by `odrisci`

