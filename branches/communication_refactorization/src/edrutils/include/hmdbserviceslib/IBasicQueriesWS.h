/********************************************************************
	created:  2012/02/28
	created:  28:2:2012   22:41
	filename: IBasicQueriesWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_IBASICQUERIESWS_H__
#define HEADER_GUARD__HMDBSERVICES_IBASICQUERIESWS_H__

#include <hmdbserviceslib/Export.h>
#include <string>
#include <utils/SmartPtr.h>
#include <hmdbserviceslib/DateTime.h>

namespace hmdbServices
{
	//! Interfejs dostarczający informacji o czasie ostatnich zmian danych po stronie bazy danych
	class HMDBSERVICES_EXPORT IGeneralBasicQueriesWS
	{
	public:
		//! Wirtualny destruktor
		virtual ~IGeneralBasicQueriesWS() {}
		//! \return Czas ostatniej zmiany danych
		virtual const DateTime dataModificationTime() const = 0;
		//! \return Czas ostatniej zmiany metadanych
		virtual const DateTime metadataModificationTime() const = 0;
	};

	typedef utils::shared_ptr<IGeneralBasicQueriesWS> GeneralBasicQueriesWSPtr;
	typedef utils::shared_ptr<const IGeneralBasicQueriesWS> GeneralBasicQueriesWSConstPtr;

	//! Interfejs usługi związanej z danymi ruchu
	class HMDBSERVICES_EXPORT IMotionBasicQueriesWS : public IGeneralBasicQueriesWS
	{
	public:
		//! Wirtualny destruktor
		virtual ~IMotionBasicQueriesWS() {}

		//! \param id Identyfikator użytkownika
		//! \return Opis XML dla danego użytkownika
		virtual const std::string getPerformerByIdXML(const int id) const = 0;

		//! \param id Identyfikator sesji
		//! \return Opis XML dla danej sesji
		virtual const std::string getSessionByIdXML(const int id) const = 0;

		//! \param id Identyfikator sesji
		//! \return Nazwa sesji
		virtual const std::string getSessionLabel(const int id) const = 0;

		//! \param id Identyfikator próby pomiarowej
		//! \return Opis XML dla danej próby pomiarowej
		virtual const std::string getTrialByIdXML(const int id) const = 0;

		//! \param id Identyfikator konfiguracji pomiarowej
		//! \return Opis XML dla danej konfiguracji pomiarowej
		virtual const std::string getMeasurementConfigurationByIdXML(const int id) const = 0;

		//! \param id Identyfikator konfiguracji aktora
		//! \return Opis XML dla danej konfiguracji aktora
		virtual const std::string getPerformerConfigurationByIdXML(const int id) const = 0;

		//! \return Opis XML dla aktorów
		virtual const std::string listPerformersXML() const = 0;

		//! \return Opis XML dla aktorów wraz z ich atrybutami
		virtual const std::string listPerformersWithAttributesXML() const = 0;

		//! \param labID Identyfikator laboratorium
		//! \return Opis XML dla aktorów wraz z ich atrybutami dla zadanego laboratorium
		virtual const std::string listLabPerformersWithAttributesXML(const int labID) const = 0;

		//! \param labID Identyfikator sesji
		//! \return Opis XML dla aktorów wraz z ich atrybutami dla zadanej sesji
		virtual const std::string listSessionPerformersWithAttributesXML(const int sessionID) const = 0;

		//! \param performerID Identyfikator aktora
		//! \return Opis XML dla sesji aktora
		virtual const std::string listPerformerSessionsXML(const int performerID) const = 0;

		//! \param performerID Identyfikator aktora
		//! \return Opis XML dla sesji aktora wraz z ich atrybutami
		virtual const std::string listPerformerSessionsWithAttributesXML(const int performerID) const = 0;

		//! \param labID Identyfikator laboratorium
		//! \return Opis XML dla sesji wykonanych w danym laboratorium wraz z ich atrybutami
		virtual const std::string listLabSessionsWithAttributesXML(const int labID) const = 0;

		//! \param sessionGroupID Identyfikator grupy sesji
		//! \return Opis XML dla sesji w ramach danej grupy sesji wraz z ich atrybutami
		virtual const std::string listGroupSessionsWithAttributesXML(const int sessionGroupID) const = 0;

		//! \param measurementConfID Identyfikator konfiguracji pomiarowej
		//! \return Opis XML dla sesji związanych z daną konfiguracją pomiarową wraz z ich atrybutami
		virtual const std::string listMeasurementConfigSessionsWithAttributesXML(const int measurementConfID) const = 0;

		//! \param sessionID Identyfikator sesji
		//! \return Opis XML dla grup sesji do których należy dana sesja
		virtual const std::string listSessionSessionGroups(const int sessionID) const = 0;

		//! \param sessionID Identyfikator sesji
		//! \return Opis XML dla prób pomiarowych związanych z tą sesją
		virtual const std::string listSessionTrialsXML(const int sessionID) const = 0;

		//! \param sessionID Identyfikator sesji
		//! \return Opis XML dla prób pomiarowych związanych z tą sesją wraz z ich atrybutami
		virtual const std::string listSessionTrialsWithAttributesXML(const int sessionID) const = 0;

		//! \return Opis XML dla konfiguracji pomiarowych wraz z ich atrybutami
		virtual const std::string listMeasurementConfigurationsWithAttributesXML() const = 0;

		//! \param sessionID Identyfikator sesji
		//! \return Opis XML dla konfiguracji aktorów wraz z atrybutami dla zadanej sesji
		virtual const std::string listSessionPerformerConfsWithAttributesXML(const int sessionID) const = 0;

		//! \param subjectID Identyfikator sesji
		//! \param subjectType Typ plików
		//! \return Opis XML dla plików zadanego aktora i typu plików
		virtual const std::string listFilesXML(const int subjectID, const std::string & subjectType) const = 0;

		//! \param subjectID Identyfikator sesji
		//! \param subjectType Typ plików
		//! \return Opis XML dla plików wraz z atrybutami zadanego aktora i typu plików
		virtual const std::string listFilesWithAttributesXML(const int subjectID, const std::string & subjectType) const = 0;

		//! \param subjectID Identyfikator sesji
		//! \param subjectType Typ plików
		//! \return Opis XML dla plików wraz z atrybutami zadanego aktora i typu plików
		virtual const std::string listFileAttributeDataXML(const int subjectID, const std::string & subjectType) const = 0;

		//! \param subjectID Identyfikator sesji
		//! \param subjectType Typ plików
		//! \return Opis XML dla plików wraz z atrybutami zadanego aktora i typu plików
		virtual const std::string listFileAttributeDataWithAttributesXML(const int subjectID, const std::string & subjectType) const = 0;

		//! \param attributeGroupName Nazwa grupy atrybutów
		//! \param entityKind Typ encji
		//! \return Opis XML dla atrybutów dla zadanej encji w zdefiniowanej grupie
		virtual const std::string listAttributesDefined(const std::string & attributeGroupName,
			const std::string & entityKind) const = 0;

		//! \param entityKind Typ encji
		//! \return Opis XML dla grup atrybutów dla zadanej encji
		virtual const std::string listAttributeGroupsDefined(const std::string & entityKind) const = 0;

		//! \return Opis XML dla grup sesji
		virtual const std::string listSessionGroupsDefined() const = 0;

		//! \return Opis XML dla typów ruchów
		virtual const std::string listMotionKindsDefined() const = 0;

		//! \param attributName Nazwa atrybutu
		//! \param entityKind Typ encji
		//! \return Opis XML dla typów ruchów
		virtual const std::string listEnumValues(const std::string & attributeName,
			const std::string & entityKind) const = 0;

		//! \return Lista adnotacji użytkownika dla danych USG - użytkownik brany z połączenia
		virtual const std::string listMyAnnotationsXML() const = 0;
		//! \return Lista wszystkich adnotacji w bazie dla danych USG
		virtual const std::string listAnnotationsXML() const = 0;

		//! Metoda zwraca listę adnotacji oczekujących na weryfikację dla danych USG - użytkownik brany z połączenia
		virtual const std::string listMyAwaitingAnnotationsXML() const = 0;

		//! Metoda zwraca listę adnotacji zatwierdzonych dla danych USG - użytkownik brany z połączenia
		virtual const std::string listMyReviewedAnnotationsXML() const = 0;
	};

	typedef utils::shared_ptr<IMotionBasicQueriesWS> MotionBasicQueriesWSPtr;
	typedef utils::shared_ptr<const IMotionBasicQueriesWS> MotionBasicQueriesWSConstPtr;

	typedef IGeneralBasicQueriesWS IMedicalBasicQueriesWS;

	typedef utils::shared_ptr<IMedicalBasicQueriesWS> MedicalBasicQueriesWSPtr;
	typedef utils::shared_ptr<const IMedicalBasicQueriesWS> MedicalBasicQueriesWSConstPtr;
}

#endif	//	HEADER_GUARD__HMDBSERVICES_IBASICQUERIESWS_H__
