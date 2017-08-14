[![Build Status](https://travis-ci.org/qtdcm/qtdcm.svg?branch=master)](https://travis-ci.org/qtdcm/qtdcm)

## Presentation

QtDCM implements several Qt widgets for manipulating Dicom data. With QtDCM, it is possible
to explore a Dicom CD-ROM, to search and download from a PACS (Pictures Archive and Communications System) and
to convert series of images to the nifti format.
Thanks to its simple design, QtDcm is very easy to integrate in other application (see the example directory
in the code).

## Installation

QtDCM requires :
* CMake (version 3.1 or later), see https://cmake.org/
* Qt (version 5.5 or later), see http://www.qt.io/
* Dcmtk (version 3.6.2 or later), see http://dicom.offis.de/dcmtk
* ITK (version 4.6 is supported), see http://www.itk.org/

## PACS compatibility

For the moment, QtDCM is only tested with
[DCM4CHEE](https://dcm4che.atlassian.net/wiki/display/ee2/Home) PACS.