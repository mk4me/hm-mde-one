/********************************************************************
	created:	2013/10/06
	created:	6:10:2013   14:34
	filename: 	DicomSource.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DICOMSOURCE_H__
#define HEADER_GUARD_DICOM__DICOMSOURCE_H__


#include <corelib/ISource.h>
#include <corelib/Filesystem.h>
#include <corelib/IHierarchyItem.h>

class DcmDirectoryRecord;

namespace dicom {

class DicomSource : public plugin::ISource
{
    UNIQUE_ID("{CB848CFE-5A1F-4D99-B06A-0249EA38B706}")
    CLASS_DESCRIPTION("Dicom Source", "Dicom Source")
public:
    DicomSource();
	virtual ~DicomSource() {}


public:
    virtual void init( core::IMemoryDataManager * memoryDM, core::IStreamDataManager * streamDM, core::IFileDataManager * fileDM );

    virtual bool lateInit();

    virtual void finalize();

    virtual void update( double deltaTime );

    virtual QWidget* getWidget();

    virtual QWidget* getControlWidget();

    virtual QWidget* getSettingsWidget();

    virtual void getOfferedTypes( core::TypeInfoList & offeredTypes ) const;
    void addFile( const core::Filesystem::Path& path );

    

    void loadDirFile(const core::Filesystem::Path& dirPath );

private:
    void handlePatientRecord( DcmDirectoryRecord * patientRecord, core::IHierarchyItemPtr root, std::string basePath );
    void handleStudyRecord( DcmDirectoryRecord * studyRecord, core::IHierarchyItemPtr root, std::string basePath );
    void handleSeriesRecord( DcmDirectoryRecord * seriesRecord, core::IHierarchyItemPtr root, std::string basePath );
    void handleFileRecord( DcmDirectoryRecord * fileRecord, core::IHierarchyItemPtr root, std::string basePath );
    core::IHierarchyItemPtr transactionPart( const core::Filesystem::Path &path );

private:
    core::IMemoryDataManager* memoryDM;
    core::IFileDataManager* fileDM;
    core::IStreamDataManager* streamDM;
};
DEFINE_SMART_POINTERS(DicomSource);

}
#endif
