/********************************************************************
	created:	2013/10/21
	created:	21:10:2013   8:37
	filename: 	IDicomInternalStruct.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__IDICOMINTERNALSTRUCT_H__
#define HEADER_GUARD_DICOM__IDICOMINTERNALSTRUCT_H__

#include <vector>
#include <boost/tuple/tuple.hpp>
#include <boost/range.hpp>

namespace dicom {

	namespace internalData {

		//! Reprezentacja obrazu USG
		struct Image {
			//! �cie�ka
			std::string imageFile;
			//! �cie�ka pliku z adnotacjami
			std::string adnotationsFile;
			//! Oryginalna nazwa pliku
			std::string originFilePath;
			//! Czy dane zawieraj� opis Power Doplera
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
		DateTuple extractDate(std::string date);
		bool isDate(const std::string& date);

		//! Opis badania
		struct Serie {
			typedef std::vector<internalData::ImagePtr> Images;
			typedef boost::iterator_range<Images::iterator> ImagesRange;

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
			//! Nazwa osoby przeprowadzaj�cej badanie
			std::string physicianName;
			//! Obrazy powsta�e podczas badania
			Images images;

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
			//! Nazwa przyjmuj�cego
			std::string physicianName;
			//!
			std::string accesionNumber;
			//! Przeprowadzone badania
			std::vector<SeriePtr> series;
			//! Numer
			int studyNumber;

			//! Konstruktor domy�lny
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
			//! P�e� pacjenta
			std::string patientSex;
			//! Wizyty pacjenta
			std::vector<StudyPtr> sessions;

			//! Konstruktor domyslny
			Patient();
			//! \param other Kopiowany pacjent
			Patient(const Patient& other);
			//! \param other Pacjent z kt�rego kopiujemy metadane
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


class IDicomInternalStruct
{
public:
	virtual ~IDicomInternalStruct() {}

	//! \return Ilo�� pacjent�w
	virtual int getNumPatients() const = 0;
	//! \return Ilo�� obraz�w
	virtual int getNumImages() const = 0;
	//! \param no Numer Pacjenta
	//! \return Pacjent o zadanym numerze
	virtual internalData::PatientConstPtr getPatient(int no) const = 0;
};
DEFINE_SMART_POINTERS(IDicomInternalStruct);
}

DEFINE_WRAPPER(dicom::IDicomInternalStruct, utils::PtrPolicyBoost, utils::ClonePolicyForbidden);
#endif
