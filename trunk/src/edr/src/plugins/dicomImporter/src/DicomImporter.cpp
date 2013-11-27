#include "DicomImporterPCH.h"
#include "DicomImporter.h"


#include "DicomImporterSource.h"
#include <corelib/Filesystem.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include "DicomImporterSourceWidget.h"
#include "dcmtk/dcmdata/dcdicdir.h"
#include "dcmtk/dcmdata/dcdirrec.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include <corelib/HierarchyHelper.h>
#include <plugins/dicomImporter/Dicom.h>
//#include <plugins/dicomImporter/ILayeredImage.h>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/format.hpp>
#include "DicomParser.h"


using namespace dicomImporter;


void DicomImporter::handleFileRecord( DcmDirectoryRecord * fileRecord, internalData::ImagePtr image, std::string basePath )
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
                image->originFilePath = suffix.string();
            } 
        }
    }
}

void DicomImporter::handleSeriesRecord( DcmDirectoryRecord * seriesRecord, internalData::SeriePtr serie, std::string basePath )
{
    OFString serieId, modality, serieDate, serieTime, serieDesc, serieNumber, physicianName;
    seriesRecord->findAndGetOFString(DCM_SeriesInstanceUID, serieId);
    seriesRecord->findAndGetOFString(DCM_Modality, modality);
    seriesRecord->findAndGetOFString(DCM_SeriesDate, serieDate);
    seriesRecord->findAndGetOFString(DCM_SeriesTime, serieTime);
    seriesRecord->findAndGetOFString(DCM_SeriesDescription, serieDesc);
    seriesRecord->findAndGetOFString(DCM_SeriesNumber, serieNumber);
    seriesRecord->findAndGetOFString(DCM_ReferringPhysicianName, physicianName);
    
    serie->serieId = serieId.c_str();
    serie->modality = modality.c_str();
    serie->serieDate = serieDate.c_str();
    serie->serieTime = serieTime.c_str();
    serie->serieDesc = serieDesc.c_str();
    serie->serieNumber = serieNumber.c_str();
    serie->physicianName = physicianName.c_str();

    DcmDirectoryRecord * fileRecord = nullptr;
    bool powerDoppler = false;
    while((fileRecord = seriesRecord->nextSub(fileRecord)) != nullptr) {
        auto image = utils::make_shared<internalData::Image>();
        handleFileRecord(fileRecord, image, basePath);
        image->isPowerDoppler = powerDoppler;
        powerDoppler = !powerDoppler;
        serie->images.push_back(image);
    }	
}

void DicomImporter::handleStudyRecord( DcmDirectoryRecord * studyRecord, internalData::StudyPtr study, std::string basePath )
{
    OFString studyId, studyDesc, studyDate, studyTime, physicianName, accesionNumber;
    studyRecord->findAndGetOFString(DCM_StudyInstanceUID, studyId);
    studyRecord->findAndGetOFString(DCM_StudyDescription, studyDesc);
    studyRecord->findAndGetOFString(DCM_StudyDate, studyDate);
    studyRecord->findAndGetOFString(DCM_StudyTime, studyTime);
    studyRecord->findAndGetOFString(DCM_ReferringPhysicianName, physicianName);
    studyRecord->findAndGetOFString(DCM_AccessionNumber, accesionNumber);

    study->studyNumber = studyCurrentIndex++;
    study->studyId = studyId.c_str();
    study->studyDesc = studyDesc.c_str();
    study->studyDate = studyDate.c_str();
    study->studyTime = studyTime.c_str();
    study->physicianName = physicianName.c_str();
    study->accesionNumber = accesionNumber.c_str();

    DcmDirectoryRecord* seriesRecord = nullptr;    
    while (((seriesRecord = studyRecord->nextSub(seriesRecord)) != nullptr)) {
        auto serie = utils::make_shared<internalData::Serie>();
        handleSeriesRecord(seriesRecord, serie, basePath);
        study->series.push_back(serie);
    }
}

void DicomImporter::handlePatientRecord( DcmDirectoryRecord * patientRecord, internalData::PatientPtr patient, std::string basePath )
{
    OFString patientID, patientName, patientBirthdate, patientSex;
    patientRecord->findAndGetOFString(DCM_PatientID,patientID);
    patientRecord->findAndGetOFString(DCM_PatientName,patientName);
    patientRecord->findAndGetOFString(DCM_PatientBirthDate, patientBirthdate);
    patientRecord->findAndGetOFString(DCM_PatientSex, patientSex);
    
    patient->patientID = patientID.c_str();
    patient->patientName = patientName.c_str();
    patient->patientBirthdate = patientBirthdate.c_str();
    patient->patientSex = patientSex.c_str();

    DcmDirectoryRecord* studyRecord = nullptr;
    while ((studyRecord = patientRecord->nextSub(studyRecord)) != nullptr)  {
        auto study = utils::make_shared<internalData::Study>();
        handleStudyRecord(studyRecord, study, basePath);
        patient->sessions.push_back(study);
    }

}

dicomImporter::DicomInternalStructPtr DicomImporter::import( const core::Filesystem::Path& from )
{
    DicomInternalStructPtr internalStruct = utils::make_shared<DicomInternalStruct>();

    core::Filesystem::Path dirfile = from / "DICOMDIR";
    if (core::Filesystem::pathExists(dirfile)) {
        DcmDicomDir dicomdir(dirfile.string().c_str());
        DcmDirectoryRecord* root = &(dicomdir.getRootRecord());

        std::string basePath = from.string();

        if (root) {
            DcmDirectoryRecord *patientRecord = nullptr;
            while ((patientRecord = root->nextSub(patientRecord)) != nullptr) {
                internalData::PatientPtr patient = utils::make_shared<internalData::Patient>();
                handlePatientRecord(patientRecord, patient, basePath);
                internalStruct->addPatient(patient);
            }
        } 

    } else {
        throw std::runtime_error("file not found");
    }

    return internalStruct;
}

void dicomImporter::DicomImporter::convertImages( DicomInternalStructPtr inter, const core::Filesystem::Path& from, const core::Filesystem::Path& to )
{
    for (auto itPatient = inter->patients.begin(); itPatient != inter->patients.end(); ++itPatient) {
        for (auto itSession = (*itPatient)->sessions.begin(); itSession != (*itPatient)->sessions.end(); ++itSession) {
            std::string sessionDir = (*itSession)->getOutputDirectory();
            core::Filesystem::createDirectory(to / sessionDir);
            int trialImageNo = 1;
            for (auto itSerie = (*itSession)->series.begin(); itSerie != (*itSession)->series.end(); ++itSerie) {
                internalData::SeriePtr serie = *itSerie;
                int trialCount = serie->images.size();
                for (int i = 0; i < trialCount; ++i) {
                    internalData::DateTuple dt = internalData::extractDate((*itSession)->studyDate);
                    if (inter->isSingle()) {
                        boost::format fmt("%s-%s-%s-S%04d-T%04d");
                        fmt % dt.get<0>() % dt.get<1>() % dt.get<2>() % (*itSession)->studyNumber % trialImageNo++;
                        convertImage((*itSerie)->images[i], from, to/sessionDir, fmt.str());
                    } else {
                        boost::format fmt("%s/%s-%s-%s-S%04d-T%04d");
                        fmt % sessionDir % dt.get<0>() % dt.get<1>() % dt.get<2>() % (*itSession)->studyNumber % trialImageNo++;
                        convertImage((*itSerie)->images[i], from, to, fmt.str());
                    }
                }
            }
        }
    }
}

void dicomImporter::DicomImporter::convertImage( internalData::ImagePtr inter, const core::Filesystem::Path& from, const core::Filesystem::Path& to, const std::string& filenameBase )
{
    core::Filesystem::Path origin = from / inter->originFilePath;

    if (core::Filesystem::pathExists(origin)) {
        DicomParser parser;
        parser.parse(origin.string());
        core::Objects objs;
        parser.getObjects(objs);
        DicomImagePtr image = (*objs.begin())->get();
        QPixmap pixmap = convertToPixmap(image);

        core::Filesystem::Path outfile = to / (filenameBase + ".png");
    
        pixmap.save(outfile.string().c_str());

        inter->imageFile = filenameBase + ".png";
        inter->adnotationsFile = filenameBase + ".xml";

        refresh(filenameBase);
    }
}



std::vector<DicomInternalStructPtr> dicomImporter::DicomImporter::split( DicomInternalStructPtr root )
{
    std::vector<DicomInternalStructPtr> results;
    int count = root->getNumPatients();
    for (int i = 0; i < count; ++i) {
        internalData::PatientConstPtr patient = root->getPatient(i);
        for (auto it = patient->sessions.begin(); it != patient->sessions.end(); ++it) {
            internalData::PatientPtr patientClone = utils::make_shared<internalData::Patient>();
            patientClone->cloneMeta(*patient);
            patientClone->sessions.push_back(utils::make_shared<internalData::Study>(**it));

            DicomInternalStructPtr inter = utils::make_shared<DicomInternalStruct>();
            inter->cloneMeta(*root);
            inter->addPatient(patientClone);
            results.push_back(inter);
        }

    }

    return results;
}


static void dummy(const std::string&) {}

dicomImporter::DicomImporter::DicomImporter( int studyFristId /*= 1*/ ) : 
    studyCurrentIndex(studyFristId)
{
    refresh = dummy;
}

void dicomImporter::DicomImporter::setCallBack( refresher r )
{
    refresh = r;
}

void DicomSaver::save( const core::Filesystem::Path& to, DicomInternalStructPtr inter )
{
    core::Filesystem::Path filename;
    if (inter->isSingle()) {
        std::string sessionName = inter->getSingleSessionOutputDir();
        filename = to / sessionName / (sessionName + ".xml");
    } else {
        filename = to / "main.xml";
    }
    std::ofstream ofs(filename.c_str());
    if(ofs.good()) {
        boost::archive::xml_oarchive oa(ofs);
        oa << BOOST_SERIALIZATION_NVP(inter);
        ofs.close();
    }
}

dicomImporter::DicomInternalStructPtr dicomImporter::DicomLoader::load( const core::Filesystem::Path& from )
{
    DicomInternalStructPtr inter = utils::make_shared<DicomInternalStruct>();
    std::ifstream ifs(from.c_str());
    if (ifs.good()) {
        boost::archive::xml_iarchive xmlIn(ifs);
        xmlIn >> BOOST_SERIALIZATION_NVP(inter);
    }

    return inter;
}
