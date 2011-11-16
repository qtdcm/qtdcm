/*
    QtDcm is a C++ Qt based library for communication and conversion of Dicom images.
    Copyright (C) 2011  Alexandre Abadie <Alexandre.Abadie@univ-rennes1.fr>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "QtDcmConvert.h"

#include <QtDcmManager.h>
#include <QtDcmPreferences.h>

#include <itkOrientedImage.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>
#include <itkMetaDataDictionary.h>
#include <itkObjectFactoryBase.h>
#include <itkMetaDataObject.h>
#include <itkImageFileWriter.h>

class QtDcmConvertPrivate
{

public:
    QString tempDirectory;
    QString serieUID;
    QString inputDirectory;
    QString outputDirectory;
    QString outputFilename;
};

QtDcmConvert::QtDcmConvert ( QObject * parent ) : d ( new QtDcmConvertPrivate )
{
    d->inputDirectory = "";
    d->outputFilename = "";
}

QtDcmConvert::~QtDcmConvert()
{
  delete d;
  d = NULL;
}


void QtDcmConvert::convert()
{
    if (QtDcmPreferences::instance()->useDcm2nii())
    {
        QString program = QtDcmPreferences::instance()->getDcm2niiPath();
        QStringList arguments;
        arguments << "-x" << "N";
        arguments << "-r" << "N";
        arguments << "-g" << "N";
        arguments << "-o" << d->outputDirectory << d->inputDirectory;
        
        QProcess * process = new QProcess(this);
        process->setStandardOutputFile(d->tempDirectory + QDir::separator() + "logs" + QDir::separator() + d->serieUID + ".txt");
        process->start(program, arguments);
        process->waitForFinished();

        delete process;
    }
    else
    {
        typedef signed short                                PixelType;
        const unsigned int Dimension = 3;
        typedef itk::OrientedImage< PixelType, Dimension >  ImageType;
        typedef itk::ImageSeriesReader< ImageType >         ReaderType;
        typedef ImageType::RegionType                       RegionType;
        typedef ImageType::SpacingType                      SpacingType;
        typedef ImageType::PointType                        PointType;
        typedef ImageType::DirectionType                    DirectionType;
        typedef itk::ImageRegionIterator<ImageType>         IteratorType;
        typedef IteratorType::IndexType                     IndexType;
        typedef itk::ImageFileWriter<ImageType>             WriterType;
        typedef itk::GDCMImageIO                            ImageIOType;
        typedef itk::GDCMSeriesFileNames                    NamesGeneratorType;
        typedef std::vector< std::string >                  FileNamesContainer;
        typedef std::vector< std::string >                  SeriesIdContainer;

//     ImageType::Pointer image = 0;

        ReaderType::Pointer reader = ReaderType::New();
        ImageIOType::Pointer dicomIO = ImageIOType::New();

        NamesGeneratorType::Pointer inputNames = NamesGeneratorType::New();
        inputNames->SetUseSeriesDetails ( true );
        inputNames->AddSeriesRestriction ( "0008|0021" );
        inputNames->AddSeriesRestriction ( "0020,0037" );
        inputNames->LoadSequencesOn();
        inputNames->LoadPrivateTagsOn();
        inputNames->SetInputDirectory ( d->inputDirectory.toStdString() );
        try
        {
            const SeriesIdContainer & seriesUID = inputNames->GetSeriesUIDs();
            std::string seriesIdentifier = seriesUID.begin()->c_str();
            FileNamesContainer filenames = inputNames->GetFileNames ( seriesIdentifier );

            dicomIO->SetFileName ( filenames.begin()->c_str() );
            try
            {
                dicomIO->ReadImageInformation();
            }
            catch ( itk::ExceptionObject &e )
            {
                qDebug() << e.GetDescription();
                return;
            }

            reader->UseStreamingOn();
            reader->SetFileNames ( filenames );
            reader->SetImageIO ( dicomIO );

            try
            {
                reader->Update();
            }
            catch ( itk::ExceptionObject &excp )
            {
                std::cerr << excp << std::endl;
                return;
            }

//         IteratorType itOut;
//
//         image = reader->GetOutput();
//
//         RegionType region;
//         region.SetSize ( 0, image->GetLargestPossibleRegion().GetSize() [0] );
//         region.SetSize ( 1, image->GetLargestPossibleRegion().GetSize() [1] );
//         region.SetSize ( 2, image->GetLargestPossibleRegion().GetSize() [2] );
//         image->SetRegions ( region );
//         image->Allocate();
//         SpacingType spacing;
//         spacing[0] = image->GetSpacing() [0];
//         spacing[1] = image->GetSpacing() [1];
//         spacing[2] = image->GetSpacing() [2];
//         spacing[3] = 1;
//         image->SetSpacing ( spacing );
//         PointType origin;
//         origin[0] = image->GetOrigin() [0];
//         origin[1] = image->GetOrigin() [1];
//         origin[2] = image->GetOrigin() [2];
//         origin[3] = 0;
//         image->SetOrigin ( origin );
//         DirectionType direction;
//         for ( unsigned int i=0; i<4; i++ )
//             for ( unsigned int j=0; j<4; j++ )
//             {
//                 if ( ( i < 3 ) && ( j < 3 ) )
//                     direction[i][j] = image->GetDirection() [i][j];
//                 else
//                     direction[i][j] = ( i == j ) ? 1 : 0;
//             }
//         image->SetDirection ( direction );
//         itOut = IteratorType ( image, region );
//
//         image->SetMetaDataDictionary ( dicomIO->GetMetaDataDictionary() );
//
//
//         itk::ImageRegionIterator<ImageType> itIn ( image, image->GetLargestPossibleRegion() );
//         while ( !itIn.IsAtEnd() )
//         {
//             itOut.Set ( itIn.Get() );
//             ++itIn;
//             ++itOut;
//         }


            WriterType::Pointer writer = WriterType::New();

            QString completeFilename = d->outputDirectory + QDir::separator() + d->outputFilename;

            writer->SetFileName ( completeFilename.toStdString() );
            writer->SetInput ( reader->GetOutput() );
//         writer->SetInput ( image );

            try
            {
                writer->Update();
            }
            catch ( itk::ExceptionObject &ex )
            {
                std::cout << ex << std::endl;
                return;
            }
        }
        catch ( itk::ExceptionObject &ex )
        {
            std::cout << ex << std::endl;
            return;
        }
    }
}

void QtDcmConvert::setInputDirectory ( QString dir )
{
    d->inputDirectory = dir;
}

void QtDcmConvert::setOutputDirectory ( QString dir )
{
    d->outputDirectory = dir;
}

void QtDcmConvert::setOutputFilename ( QString fname )
{
    d->outputFilename = fname;
}

void QtDcmConvert::setSerieUID(QString uid)
{
  d->serieUID = uid;
}

void QtDcmConvert::setTempDirectory(QString dir)
{
  d->tempDirectory = dir;
}

