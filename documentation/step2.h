/**
 * \page step2page Configuration
 *
 * \section supported_builds_sec Supported builds system
 * 
 * QtDcm can be built and run on the following systems:
 * * GCC or CLang on Linux (Ubuntu, CentOS have been tested)
 * * VisualStudio 2013 (2010 and 2012 should work but were not tested)
 * * MacOS X
 * 
 * \section dep_sec The dependencies
 *
 * \subsection cmake_dep_sec CMake
 * 
 * Install <a href="http://qt.io">CMake</a><br> and choose the installer corresponding to your system. The minimum version required is 3.1 so on certain Linux
 * it might not be availble, thus you'll have to build it on your own. 
 * 
 * \subsection qt_dep_sec Qt
 *
 * You can get the Qt development kit from <a href="http://qt.io">the Qt website</a><br>
 * Take the development kit corresponding to your system and at least install Qt 5.5.
 *
 * \subsection dcmtk_dep_sec DCMTK
 *
 * Download the source code from the <a href="ftp://dicom.offis.de/pub/dicom/offis/software/dcmtk/dcmtk360/dcmtk-3.6.0.tar.gz">OFFIS website</a>
 * Use the following CMake options values:
 * <div id="code">
 * BUILD_SHARED_LIBS: OFF
 * CMAKE_BUILD_TYPE: RelWithDebInfo
 * </div>
 * 
 * \subsection itk_dep_sec The Insight Toolkit
 *
 * Download the 4.6 version of the Insight Toolkit from <a href="http://voxel.dl.sourceforge.net/sourceforge/itk/InsightToolkit-4.6.0.tar.gz">here</a>.
 * Use the following CMake options values:
 * <div id="code">
 * BUILD_EXAMPLES: OFF
 * BUILD_SHARED_LIBS: ON
 * BUILD_TESTING: OFF
 * CMAKE_BUILD_TYPE: RelWithDebInfo
 * </div>
 * 
 * \section config_sec Configuring QtDcm with CMake
 * Make sure to have CMake 3.1 minimum and then go to your local QtDcm source directory. I recommend to configure an "out of source" build, so create
 * a "build" directory and launch cmake from there:
 * <div id="code">
 * $ cd $qtdcm_source_dir<br>
 * $ mkdir build<br>
 * $ cd build<br>
 * $ cmake ..<br>
 * </div>
 * 
 * Set the following CMake options values:
 * <div id="code">
 * ITK_DIR : /path/to/your/itk/build/directory<br>
 * DCMTK_DIR : /path/to/your/dcmtk/build/directory<br>
 * DCMTK_SOURCE_DIR : /path/to/your/dcmtk/source/directory<br>
 * CMAKE_BUILD_TYPE : RelWithDebInfo<br>
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
