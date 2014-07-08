/********************************************************************
	created:	2013/10/21
	created:	21:10:2013   8:36
	filename: 	DicomImporter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOMIMPORTER__DICOMIMPORTER_H__
#define HEADER_GUARD_DICOMIMPORTER__DICOMIMPORTER_H__

#include "DicomInternalStruct.h"
#include <corelib/Filesystem.h>
#include <boost/tuple/tuple.hpp>

class DcmDirectoryRecord;

namespace dicomImporter {
class DicomImporter
{
public:
    typedef boost::function<void (const std::string&)> refresher;

public:
    DicomImporter(int studyFristId = 1);

	virtual ~DicomImporter() {}
    DicomInternalStructPtr import( const core::Filesystem::Path& from );
    void convertImages(DicomInternalStructPtr inter, const core::Filesystem::Path& from, const core::Filesystem::Path& to );
    std::vector<DicomInternalStructPtr> split(DicomInternalStructPtr);

    int getStudyCurrentIndex() const { return studyCurrentIndex; }
    void setStudyCurrentIndex(int val) { studyCurrentIndex = val; }

    void setCallBack(refresher r);

    static bool testPowerDoppler( const QPixmap &pixmap );

private:
    void handlePatientRecord( DcmDirectoryRecord * patientRecord, internalData::PatientPtr patient, std::string basePath );
    void handleStudyRecord( DcmDirectoryRecord * studyRecord, internalData::StudyPtr study,  std::string basePath );
    void handleSeriesRecord( DcmDirectoryRecord * seriesRecord, internalData::SeriePtr serie, std::string basePath );
    void handleFileRecord( DcmDirectoryRecord * fileRecord, internalData::ImagePtr image, std::string basePath );
    void convertImage( internalData::ImagePtr inter, const core::Filesystem::Path& from, const core::Filesystem::Path& to, const std::string& filenameBase );


    std::string getSessionDir() const;

private:
    int studyCurrentIndex;
    refresher refresh;
};
DEFINE_SMART_POINTERS(DicomImporter);

class DicomSaver
{
public:
    virtual ~DicomSaver() {}
    void save( const core::Filesystem::Path& to, DicomInternalStructPtr inter );
};
DEFINE_SMART_POINTERS(DicomSaver);

class DicomLoader
{
public:
    virtual ~DicomLoader() {}
    DicomInternalStructPtr load( const core::Filesystem::Path& from);
};
DEFINE_SMART_POINTERS(DicomLoader);
}
#endif
