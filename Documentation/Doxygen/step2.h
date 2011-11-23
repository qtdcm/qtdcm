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
 * $ CFLAGS="-fPIC" CXXFLAGS="-fPIC" AR="gcc" ARFLAGS="-shared -o" LIBEXT="so" RANLIB=":" ./configure -disable-threads --without-openssl --without-libtiff --without-libpng<br>
 * $ make && sudo make install-all
 * </div>
 *
 * \subsection itk_dep_sec The Insight Toolkit
 *
 * Download the 3.20 version of the Insight Toolkit from <a href="http://voxel.dl.sourceforge.net/sourceforge/itk/InsightToolkit-3.20.0.tar.gz">here</a>.
 *
 * \section config_sec Configuring QtDcm with CMake
 * Make sure to have CMake 2.8.5 minimum and then go to your local QtDcm source directory. We will perform an "out of source" build, so you have to create
 * a "build" directory and launch cmake from this place:
 * <div id="code">
 * $ cd $qtdcm_source_dir<br>
 * $ mkdir build<br>
 * $ cd build<br>
 * $ cmake ..<br>
 * </div>
 *
 * Now you have to configure some CMake variables before compiling the project. Use ccmake like this:
 * <div id="code">
 * $ cd $qtdcm_source_dir/build<br>
 * $ ccmake .
 * </div>
 *
 * Then set the variables:
 * <div id="code">
 * ITK_DIR : /path/to/your/itk/build/directory<br>
 * DCMTK_DIR : /usr/local<br>
 * CMAKE_BUILD_TYPE : release<br>
 * BUILD_SHARED_LIBS : ON<br>
 * </div>
 *
 * If you want to play with the example or build this documentation, you can also set the following variables:
 * <div id="code">
 * BUILD_EXAMPLE : ON<br>
 * BUILD_DOCUMENTATION : ON<br>
 * </div>
 *
 * Then you are ready to compile the library.
 *
 * Go to the \ref step3page step<br>
 * Back to \ref step1page step
 * 
 */