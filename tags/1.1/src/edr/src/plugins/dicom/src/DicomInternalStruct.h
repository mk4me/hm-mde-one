/********************************************************************
	created:	2013/10/21
	created:	21:10:2013   8:38
	filename: 	DicomInternalStruct.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DICOMINTERNALSTRUCT_H__
#define HEADER_GUARD_DICOM__DICOMINTERNALSTRUCT_H__

#include <plugins/dicom/IDicomInternalStruct.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

namespace dicom {


//! Wewn�trzne drzewo danych
struct DicomInternalStruct : public IDicomInternalStruct
{
public:
	//! Destruktor wirtualny
	virtual ~DicomInternalStruct();

	//! \param patient Pacjent
    void addPatient(internalData::PatientPtr patient);
	//! \return Ilo�� pacjent�w
    int getNumPatients() const;
	//! \return Ilo�� obraz�w
    int getNumImages() const;
	//! \param no Numer Pacjenta
	//! \return Pacjent o zadanym numerze
    internalData::PatientConstPtr getPatient(int no) const;
	//! \param other Drzewo z kt�rego kopiujemy metadane
    void cloneMeta(const DicomInternalStruct& other) {}
	//! \return Czy mamy jednego pacjenta z jednym badaniem
    bool isSingle() const;
	//! \return �cie�ka katalogu badania dla pojedynczego pacjenta
    std::string getSingleSessionOutputDir() const;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::make_nvp("patients", patients);
    }
	//! \param sessionName Nazwa badania
	//! \return Badanie o zadanej nazwie
    internalData::StudyConstPtr getSession(const std::string& sessionName) const;
	//! \param sessionName Nazwa badania
	//! \return Badanie o zadanej nazwie lub nullptr
    internalData::StudyConstPtr tryGetSession(const std::string& sessionName) const;
	//! \param imageFilename Nazwa obrazu
	//! \return Obraz dla zadanej nazwy
    internalData::ImageConstPtr getImage(const std::string& imageFilename) const;
	//! \param imageFilename Nazwa obrazu
	//! \return Obraz dla zadanej nazwy
    internalData::ImageConstPtr tryGetImage(const std::string& imageFilename) const;

private:
	//! Pacjenci
    std::vector<internalData::PatientPtr> patients;
};
DEFINE_SMART_POINTERS(DicomInternalStruct);
}


#endif
