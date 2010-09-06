QtDcm - version 0.1
(c) Alexandre Abadie
=============================================
contact : alexandre.abadie@univ-rennes1.fr
          romain.carpentier@irisa.fr
=============================================

1 - Présentation
2 - Installation


1 - Présentation
================

La bibliothèque QtDcm implémente un widget Qt pour manipuler des données Dicom.
Avec QtDcm, il est possible d'explorer un CD Dicom, d'interroger un PACS (Pictures
Archive and Communications System) et de convertir ces données en format Nifti. Il
est également possible d'afficher une prévisualisation des images dicom.

2 - Installation
================

QtDcm nécessite l'installation de CMake (version 2.6 et supérieures), Qt (version 4.5
et supérieures, Dcmtk (version 3.5.4 et supérieures).
La conversion des données Dicom en format Nifti se base sur l'outil en ligne de commande
<< dcm2nii >> téléchargeable sur le site de Mricron.
La communications avec un PACS utilise la distribution des outils Dcm4che2.
