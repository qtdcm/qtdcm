/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Alexandre <email>

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

#include "itkOrientedImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"

class QtDcmConvertPrivate
{

public:
    QString inputDirectory;
    QString outputDirectory;
    QString outputFilename;
};

QtDcmConvert::QtDcmConvert ( QObject * parent ) : d ( new QtDcmConvertPrivate )
{
    d->inputDirectory = "";
    d->outputFilename = "";
}

void QtDcmConvert::convert()
{
    typedef signed short                                PixelType;
    const unsigned int Dimension = 3;
    typedef itk::OrientedImage< PixelType, Dimension >  ImageType;
    typedef itk::ImageSeriesReader< ImageType >         ReaderType;
    typedef itk::ImageFileWriter<ImageType>             WriterType;
    typedef itk::GDCMImageIO                            ImageIOType;
    typedef itk::GDCMSeriesFileNames                    NamesGeneratorType;
    typedef std::vector< std::string >                  FileNamesContainer;
    typedef std::vector< std::string >                  SeriesIdContainer;
    ReaderType::Pointer reader = ReaderType::New();
    ImageIOType::Pointer dicomIO = ImageIOType::New();

    NamesGeneratorType::Pointer inputNames = NamesGeneratorType::New();
    inputNames->SetUseSeriesDetails ( true );
    inputNames->AddSeriesRestriction ( "0008|0021" );
    inputNames->AddSeriesRestriction ( "0020,0037" );
    inputNames->SetDirectory ( d->inputDirectory.toStdString() );

    try
    {
        const SeriesIdContainer & seriesUID = inputNames->GetSeriesUIDs();
        std::string seriesIdentifier = seriesUID.begin()->c_str();
        FileNamesContainer filenames = inputNames->GetFileNames ( seriesIdentifier );

        reader->SetImageIO ( dicomIO );
        reader->SetFileNames ( filenames );

        try
        {
            reader->Update();
        }
        catch ( itk::ExceptionObject &excp )
        {
            std::cerr << excp << std::endl;
            return;
        }

        WriterType::Pointer writer = WriterType::New();

        QString completeFilename = d->outputDirectory + QDir::separator() + d->outputFilename;

        writer->SetFileName ( completeFilename.toStdString() );
        writer->SetInput ( reader->GetOutput() );

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
