#include "DicomPCH.h"
#include "DicomInternalStruct.h"
#include <boost/format.hpp>

using namespace dicom;


internalData::PatientConstPtr dicom::DicomInternalStruct::getPatient( int no ) const
{
    return patients[no];
}

int dicom::DicomInternalStruct::getNumPatients() const
{
    return patients.size();
}

void dicom::DicomInternalStruct::addPatient( internalData::PatientPtr patient )
{
    patients.push_back(patient);
}

dicom::DicomInternalStruct::~DicomInternalStruct()
{

}

bool dicom::DicomInternalStruct::isSingle() const
{
    if (patients.size() == 1 && patients[0]->sessions.size() == 1) {
        return true;
    }

    return false;
}

std::string dicom::DicomInternalStruct::getSingleSessionOutputDir() const
{
    if (!isSingle()) {
        UTILS_ASSERT(false);
        throw std::runtime_error("Unable to get single output directory, multiple sessions are present");
    }

    return patients[0]->sessions[0]->getOutputDirectory();
}

int dicom::DicomInternalStruct::getNumImages() const
{
    int num = 0;
    for (auto itPatient = patients.begin(); itPatient != patients.end(); ++itPatient) {
        for (auto itSession = (*itPatient)->sessions.begin(); itSession != (*itPatient)->sessions.end(); ++itSession) {
            for (auto itSerie = (*itSession)->series.begin(); itSerie != (*itSession)->series.end(); ++itSerie) {
                num += (*itSerie)->images.size();
            }
        }
    }
    return num;
}

dicom::internalData::Patient::Patient( const Patient& other )
{
    cloneMeta(other);
    sessions.reserve(other.sessions.size());
    for (auto it = other.sessions.begin(); it != other.sessions.end(); ++it) {
        sessions.push_back(utils::make_shared<Study>(**it));
    }
}

dicom::internalData::Patient::Patient()
{

}

void dicom::internalData::Patient::cloneMeta( const Patient& other )
{
    patientID = other.patientID;
    patientName = other.patientName;
    patientBirthdate = other.patientBirthdate;
    patientSex = other.patientSex;
}

dicom::internalData::Study::Study( const Study& other ) :
studyId(other.studyId),
    studyDesc(other.studyDesc),
    studyDate(other.studyDate),
    studyTime(other.studyTime),
    physicianName(other.physicianName),
    accesionNumber(other.accesionNumber),
    studyNumber(other.studyNumber)
{
    series.reserve(other.series.size());
    for (auto it = other.series.begin(); it != other.series.end(); ++it) {
        series.push_back(utils::make_shared<Serie>(**it));
    }
}

dicom::internalData::Study::Study() :
    studyNumber(-1)
{

}

std::string dicom::internalData::Study::getOutputDirectory()
{
    DateTuple sdate = extractDate(studyDate);
    boost::format fmt("%s-%s-%s-S%04d");
    fmt % sdate.get<0>() % sdate.get<1>() % sdate.get<2>() % studyNumber;
    return fmt.str();
}

dicom::internalData::Serie::Serie( const Serie& other ) :
serieId(other.serieId),
    modality(other.modality),
    serieDate(other.serieDate),
    serieTime(other.serieTime),
    serieDesc(other.serieDesc),
    serieNumber(other.serieNumber),
    physicianName(other.physicianName)
{
    images.reserve(other.images.size());
    for (auto it = other.images.begin(); it != other.images.end(); ++it) {
        images.push_back(utils::make_shared<Image>(**it));
    }
}

dicom::internalData::Serie::Serie()
{

}


dicom::internalData::DateTuple dicom::internalData::extractDate( std::string date )
{
    if (!isDate(date)) {
        date = "XXXXXXXX";
    }
    return boost::make_tuple(date.substr(0,4), date.substr(4,2), date.substr(6,2));
}

bool dicom::internalData::isDate( const std::string& date )
{
    return (date.size() == 8) && (std::all_of(date.begin(), date.end(), ::isdigit));
}