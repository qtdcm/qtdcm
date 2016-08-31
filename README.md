[![Build Status](https://travis-ci.org/qtdcm/qtdcm.svg?branch=master)](https://travis-ci.org/qtdcm/qtdcm)

1 - Presentation
----------------

QtDcm implements several Qt widgets which aim at manipulating Dicom data. With QtDcm, it is possible
to explore a Dicom CD-ROM, to search and download from a PACS (Pictures Archive and Communications System) and
to convert series of images to the nifti format.
Due to its simple design, QtDcm is very easy to integrate in other application (see the example directory
in the code).

2 - Installation
----------------

QtDcm requires :
* CMake (version 3.1 or later), see https://cmake.org/
* Qt (version 5.5 or later), see http://www.qt.io/
* Dcmtk (version 3.6.0 or later), see http://dicom.offis.de/dcmtk
* ITK (version 4.6 is supported), see http://www.itk.org/
 
The conversion of Dicom data to Nifti format can also be perfomed with the command line tool
<<dcm2nii>>. This tool can be downloaded from the mricron website
(http://www.cabiatl.com/mricro/mricron/dcm2nii.html).
