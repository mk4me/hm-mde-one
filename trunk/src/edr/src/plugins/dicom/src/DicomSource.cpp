#include "DicomSource.h"
#include <corelib/Filesystem.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include "DicomSourceWidget.h"
#include "dcmtk/dcmdata/dcdicdir.h"
#include "dcmtk/dcmdata/dcdirrec.h"
#include "dcmtk/dcmdata/dcdeftag.h"

using namespace dicom;

DicomSource::DicomSource() :
    fileDM(nullptr),
    memoryDM(nullptr)
{

}


void DicomSource::init( core::IMemoryDataManager * memoryDM, core::IStreamDataManager * streamDM, core::IFileDataManager * fileDM )
{
    this->fileDM = fileDM;
    this->memoryDM = memoryDM;
    this->streamDM = streamDM;
}

bool DicomSource::lateInit()
{
    return true;
}

void DicomSource::finalize()
{
    
}

void DicomSource::update( double deltaTime )
{
    
}

QWidget* DicomSource::getWidget()
{
    return new DicomSourceWidget(this);
}

QWidget* DicomSource::getControlWidget()
{
    return nullptr;
}

QWidget* DicomSource::getSettingsWidget()
{
    return nullptr;
}

void DicomSource::getOfferedTypes( core::TypeInfoList & offeredTypes ) const
{
    
}

void DicomSource::addFile( const core::Filesystem::Path& path )
{
    auto root = transactionPart(path);
    auto hierarchyTransaction = memoryDM->hierarchyTransaction();
    hierarchyTransaction->addRoot(root);
}

void createHierarchy(core::IHierarchyItemPtr root, DcmDirectoryRecord* record)
{
    auto count = record->cardSub();
    auto elemCount = record->card();
    for (auto i = 0; i < elemCount; ++i) {
        DcmElement* element = record->getElement(i);
        
        if (element) {
            auto tag = element->getTag();
            QString desc(tag.getTagName());
            core::IHierarchyItemPtr itm = utils::make_shared<core::HierarchyItem>(QString("Element %1").arg(i + 1), desc, QIcon());
            root->appendChild(itm);
        }
    }

    for (auto i = 0; i < count; ++i) {
        DcmDirectoryRecord* child = record->getSub(i);
        core::IHierarchyItemPtr itm = utils::make_shared<core::HierarchyItem>(QString("childDir"), QString("desc"), QIcon());
        root->appendChild(itm);
        createHierarchy(itm, child);
    }
}

void DicomSource::loadDirFile( const core::Filesystem::Path& dirPath )
{
    core::Filesystem::Path dirfile = dirPath / "DICOMDIR";
    if (core::Filesystem::pathExists(dirfile)) {
        /*DcmDicomDir* dir = new DcmDicomDir(dirfile.string().c_str());
        DcmDirectoryRecord *rec = &(dir->getRootRecord());

        core::IHierarchyItemPtr root = utils::make_shared<core::HierarchyItem>(QString("DICOMDIR"), QString("main dicom directory"), QIcon());
        createHierarchy(root, rec);

        auto hierarchyTransaction = memoryDM->hierarchyTransaction();
        hierarchyTransaction->addRoot(root);
        int test = 0;*/

        DcmDicomDir dicomdir(dirfile.string().c_str());
        DcmDirectoryRecord* root = &(dicomdir.getRootRecord());
                
        std::string basePath = dirPath.string();
        
        if (root) {
            core::IHierarchyItemPtr rootItm = utils::make_shared<core::HierarchyItem>(QString("DICOMDIR"), QString("main dicom directory"), QIcon());
            DcmDirectoryRecord *patientRecord = nullptr;
            while ((patientRecord = root->nextSub(patientRecord)) != nullptr) {
                handlePatientRecord(patientRecord, rootItm, basePath);
            }

            auto hierarchyTransaction = memoryDM->hierarchyTransaction();
            hierarchyTransaction->addRoot(rootItm);
        } 

    } else {
        throw std::runtime_error("file not found");
    }
}

void DicomSource::handleFileRecord( DcmDirectoryRecord * fileRecord, core::IHierarchyItemPtr root, std::string basePath )
{
    OFString tmpString;
    if(fileRecord->findAndGetOFStringArray(DCM_DirectoryRecordType,tmpString).good() && (tmpString == "IMAGE" || tmpString == "ENCAP DOC"))
    {
        if(fileRecord->findAndGetOFStringArray(DCM_ReferencedFileID,tmpString).good())
        {
            OFString uidImagen,instanceNumber;
            fileRecord->findAndGetOFString(DCM_ReferencedSOPInstanceUIDInFile, uidImagen);
            fileRecord->findAndGetOFString(DCM_InstanceNumber, instanceNumber);

            core::Filesystem::Path currentPath(basePath);
            core::Filesystem::Path suffix(tmpString.c_str());
            currentPath /= suffix;
            if (core::Filesystem::pathExists(currentPath)) {
                //utils::ObjectWrapperPtr wrapper;
                ////HierarchyDataItemPtr dataItem = utils::make_shared<HierarchyDataItem>(wrapper, QString());                
                //core::HierarchyItemPtr dataItem = utils::make_shared<core::HierarchyItem>(QString("image"), QString());                
                auto dataItem = transactionPart(currentPath);
                root->appendChild(dataItem);
            } 
        }
    }
}

void DicomSource::handleSeriesRecord( DcmDirectoryRecord * seriesRecord, core::IHierarchyItemPtr root, std::string basePath )
{
    OFString serieId, modality, serieDate, serieTime, serieDesc, serieNumber, physicianName;
    seriesRecord->findAndGetOFString(DCM_SeriesInstanceUID, serieId);
    seriesRecord->findAndGetOFString(DCM_Modality, modality);
    seriesRecord->findAndGetOFString(DCM_SeriesDate, serieDate);
    seriesRecord->findAndGetOFString(DCM_SeriesTime, serieTime);
    seriesRecord->findAndGetOFString(DCM_SeriesDescription, serieDesc);
    seriesRecord->findAndGetOFString(DCM_SeriesNumber, serieNumber);
    seriesRecord->findAndGetOFString(DCM_ReferringPhysicianName, physicianName);
      

    QString name = QString("Serie: %1").arg(serieNumber.c_str());
    core::IHierarchyItemPtr serieItem = utils::make_shared<core::HierarchyItem>(name, QString());

    DcmDirectoryRecord * fileRecord = nullptr;
    while((fileRecord = seriesRecord->nextSub(fileRecord)) != nullptr) {
        handleFileRecord(fileRecord, serieItem, basePath);
    }	

    if (serieItem->getNumChildren() > 0) {
        root->appendChild(serieItem);
    }


}

void DicomSource::handleStudyRecord( DcmDirectoryRecord * studyRecord, core::IHierarchyItemPtr root, std::string basePath )
{
    OFString studyId, studyDesc, studyDate, studyTime, physicianName, accesionNumber;
    studyRecord->findAndGetOFString(DCM_StudyInstanceUID, studyId);
    studyRecord->findAndGetOFString(DCM_StudyDescription, studyDesc);
    studyRecord->findAndGetOFString(DCM_StudyDate, studyDate);
    studyRecord->findAndGetOFString(DCM_StudyTime, studyTime);
    studyRecord->findAndGetOFString(DCM_ReferringPhysicianName, physicianName);
    studyRecord->findAndGetOFString(DCM_AccessionNumber, accesionNumber);

    QString name = QString("Study");
    if (studyDate.length() == 8) {
        name += QString(" %1.%2.%3")
            .arg(studyDate.substr(0, 4).c_str())
            .arg(studyDate.substr(4, 2).c_str())
            .arg(studyDate.substr(6, 2).c_str());
    }
    core::IHierarchyItemPtr studyItem = utils::make_shared<core::HierarchyItem>(name, QString());

    DcmDirectoryRecord* seriesRecord = nullptr;    
    while (((seriesRecord = studyRecord->nextSub(seriesRecord)) != nullptr)) {
        handleSeriesRecord(seriesRecord, studyItem, basePath);
    }

    if (studyItem->getNumChildren() > 0) {
        root->appendChild(studyItem);
    }
}

void DicomSource::handlePatientRecord( DcmDirectoryRecord * patientRecord, core::IHierarchyItemPtr root, std::string basePath )
{
    OFString patientID, patientName, patientBirthdate, patientSex;
    patientRecord->findAndGetOFString(DCM_PatientID,patientID);
    patientRecord->findAndGetOFString(DCM_PatientName,patientName);
    patientRecord->findAndGetOFString(DCM_PatientBirthDate, patientBirthdate);
    patientRecord->findAndGetOFString(DCM_PatientSex, patientSex);
    
    QString name = QString("Patient: %1 %2 %3 %4").arg(patientID.c_str()).arg(patientName.c_str()).arg(patientBirthdate.c_str()).arg(patientSex.c_str());
    core::IHierarchyItemPtr patientItem = utils::make_shared<core::HierarchyItem>(name, QString());

    DcmDirectoryRecord* studyRecord = nullptr;
    while ((studyRecord = patientRecord->nextSub(studyRecord)) != nullptr)  {
        handleStudyRecord(studyRecord, patientItem, basePath);
    }

    if (patientItem->getNumChildren() > 0) {
        root->appendChild(patientItem);
    }
}

core::IHierarchyItemPtr DicomSource::transactionPart( const core::Filesystem::Path &path )
{
    auto transaction = fileDM->transaction();
    transaction->addFile(path);
    core::ConstObjectsList oList;
    transaction->getObjects(path, oList);

    QString filename = QString::fromStdString(path.filename().string());
    QString desc = QString::fromStdString(path.string());

    if (oList.size() == 1) {
        return utils::make_shared<core::HierarchyDataItem>(*oList.begin(), QIcon(), filename, desc);
    }

    core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>(filename, desc, QIcon());
    for (auto it = oList.begin(); it != oList.end(); ++it) {
        core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(*it, desc);
        root->appendChild(item);
    }

    return root;
}
