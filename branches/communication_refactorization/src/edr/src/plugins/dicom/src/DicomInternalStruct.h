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

//! Wewnêtrzna reprezentacja danych
namespace internalData {

	//! Reprezentacja obrazu USG
    struct Image {
		//! Œcie¿ka
        std::string imageFile;
		//! Œcie¿ka pliku z adnotacjami
        std::string adnotationsFile;
		//! Oryginalna nazwa pliku
        std::string originFilePath;
		//! Czy dane zawieraj¹ opis Power Doplera
        bool isPowerDoppler;

		//! Konstruktor domyslny
        Image() : isPowerDoppler(false) {}

        template <typename Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("imageFile", imageFile);
            ar & boost::serialization::make_nvp("adnotationsFile", adnotationsFile);
            ar & boost::serialization::make_nvp("originFilePath", originFilePath);
            ar & boost::serialization::make_nvp("isPowerDoppler", isPowerDoppler);
        }
    };
    DEFINE_SMART_POINTERS(Image);


    typedef boost::tuple<std::string, std::string, std::string> DateTuple;
    DateTuple extractDate( std::string date );
    bool isDate(const std::string& date);

	//! Opis badania
    struct Serie {
		//! Identyfikator
        std::string serieId;
		//!
        std::string modality;
		//! Data
        std::string serieDate;
		//! Godzina
        std::string serieTime;
		//! Opis
        std::string serieDesc;
		//! Numer
        std::string serieNumber;
		//! Nazwa osoby przeprowadzaj¹cej badanie
        std::string physicianName;
		//! Obrazy powsta³e podczas badania
        std::vector<ImagePtr> images;

		//! Konstruktor domyslny
        Serie();

		//! \param other Kopiowane badanie
        Serie(const Serie& other);

        template <typename Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("serieId", serieId);
            ar & boost::serialization::make_nvp("modality", modality);
            ar & boost::serialization::make_nvp("serieDate", serieDate);
            ar & boost::serialization::make_nvp("serieTime", serieTime);
            ar & boost::serialization::make_nvp("serieDesc", serieDesc);
            ar & boost::serialization::make_nvp("serieNumber", serieNumber);
            ar & boost::serialization::make_nvp("physicianName", physicianName);
            ar & boost::serialization::make_nvp("images", images);
        }
    };
    DEFINE_SMART_POINTERS(Serie)

	//! Opis wizyty
	struct Study {
		//! Identyfikator
		std::string studyId;
		//! Opis
		std::string studyDesc;
		//! Data
		std::string studyDate;
		//! Godzina
		std::string studyTime;
		//! Nazwa przyjmuj¹cego
		std::string physicianName;
		//!
		std::string accesionNumber;
		//! Przeprowadzone badania
		std::vector<SeriePtr> series;
		//! Numer
		int studyNumber;

		//! Konstruktor domyœlny
		Study();
		//! \param other Kopiowana wizyta
		Study(const Study& other);
		//! \return Katalog docelowy
		std::string getOutputDirectory();

		template <typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("studyId", studyId);
			ar & boost::serialization::make_nvp("studyDesc", studyDesc);
			ar & boost::serialization::make_nvp("studyDate", studyDate);
			ar & boost::serialization::make_nvp("studyTime", studyTime);
			ar & boost::serialization::make_nvp("physicianName", physicianName);
			ar & boost::serialization::make_nvp("accesionNumber", accesionNumber);
			ar & boost::serialization::make_nvp("series", series); 
			ar & boost::serialization::make_nvp("studyNumber", studyNumber);
		}
	};
	DEFINE_SMART_POINTERS(Study)

	//! Opis pacjenta
	struct Patient {
		//! Identyfikator
		std::string patientID;
		//! Nazwa
		std::string patientName;
		//! Data urodzenia
		std::string patientBirthdate;
		//! P³eæ pacjenta
		std::string patientSex;
		//! Wizyty pacjenta
		std::vector<StudyPtr> sessions;

		//! Konstruktor domyslny
		Patient();
		//! \param other Kopiowany pacjent
		Patient(const Patient& other);
		//! \param other Pacjent z którego kopiujemy metadane
		void cloneMeta(const Patient& other);

		template <typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("patientID", patientID);
			ar & boost::serialization::make_nvp("patientName", patientName);
			ar & boost::serialization::make_nvp("patientBirthdate", patientBirthdate);
			ar & boost::serialization::make_nvp("patientSex", patientSex);
			ar & boost::serialization::make_nvp("sessions", sessions);
		}
	};
	DEFINE_SMART_POINTERS(Patient);
}

//! Wewnêtrzne drzewo danych
struct DicomInternalStruct : public IDicomInternalStruct
{
public:
	//! Destruktor wirtualny
	virtual ~DicomInternalStruct();

	//! \param patient Pacjent
    void addPatient(internalData::PatientPtr patient);
	//! \return Iloœæ pacjentów
    int getNumPatients() const;
	//! \return Iloœæ obrazów
    int getNumImages() const;
	//! \param no Numer Pacjenta
	//! \return Pacjent o zadanym numerze
    internalData::PatientConstPtr getPatient(int no) const;
	//! \param other Drzewo z którego kopiujemy metadane
    void cloneMeta(const DicomInternalStruct& other) {}
	//! \return Czy mamy jednego pacjenta z jednym badaniem
    bool isSingle() const;
	//! \return Œcie¿ka katalogu badania dla pojedynczego pacjenta
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


DEFINE_WRAPPER(dicom::DicomInternalStruct, utils::PtrPolicyBoost, utils::ClonePolicyForbidden);
#endif
