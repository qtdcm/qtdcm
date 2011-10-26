/**
 * \page step2page Configuration
 *
 * The project has to be configured using CMake, see the <a href="http://www.cmake.org">CMake website</a> for more information.
 *
 * \section dep_sec The dependencies
 *
 * \subsection qt_dep_sec Qt
 *
 * You can get the Qt development kit from <a href="http://qt.nokia.com/downloads/">the Qt website</a><br>
 * On Ubuntu you can simply use the package manager: <a href="apt://libqt4-dev">libqt4-dev</a>
 * <div id="code">
 * $ sudo apt-get install libqt4-dev
 * </div>
 *
 * \subsection dcmtk_dep_sec DCMTK
 *
 * Download the source code from the <a href="ftp://dicom.offis.de/pub/dicom/offis/software/dcmtk/dcmtk360/dcmtk-3.6.0.tar.gz">OFFIS website</a>
 *
 * On Linux, configure the code, compile and install using the following commands (from the source directory):
 * <div id="code">
 * $ CFLAGS="-fPIC" CXXFLAGS="-fPIC" AR="gcc" ARFLAGS="-shared -o" LIBEXT="so" RANLIB=":" ./configure<br>
 * $ make && sudo make install-all
 * </div>
 *
 * \subsection itk_dep_sec The Insight Toolkit
 *
 * Download the 3.20 version of the Insight Toolkit from <a href="http://voxel.dl.sourceforge.net/sourceforge/itk/InsightToolkit-3.20.0.tar.gz">here</a>.
 *
 *
 * \section config_sec Configuring QtDcm with CMake
 *
 *
 * Go to the \ref step3page step<br>
 * Back to \ref step1page step
 * 
 */