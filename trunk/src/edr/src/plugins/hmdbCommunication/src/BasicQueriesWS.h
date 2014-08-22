/********************************************************************
	created:  2014/05/19	12:16:56
	filename: BasicQueriesWS.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__BASICQUERIESWS_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__BASICQUERIESWS_H__

#include "WSDLServiceTypes.h"
#include <hmdbserviceslib/IBasicQueriesWS.h>

namespace hmdbCommunication
{
	class GeneralBasicQueriesWS : public hmdbServices::IGeneralBasicQueriesWS
	{
	public:
		/**
		Konstruktor klasy GeneralBasicQueriesWS.
		*/
		GeneralBasicQueriesWS(const WSDLServiceCreator creator);
		/**
		Wirtualny destruktor klasy GeneralBasicQueriesWS.
		*/
		virtual ~GeneralBasicQueriesWS();

		//! \return Data ostatniej modyfikacji danych
		virtual const hmdbServices::DateTime dataModificationTime() const;
		//! \return Data ostatniej modyfikacji metadanych
		virtual const hmdbServices::DateTime metadataModificationTime() const;

	private:
		//! Serwis obsługujący tą usługę
		const WSDLServiceCreator creator;
	};

	class MotionBasicQueriesWS : public hmdbServices::IMotionBasicQueriesWS
	{
	public:
		/**
		Konstruktor klasy MotionBasicQueriesService.
		*/
		MotionBasicQueriesWS(const WSDLServiceCreator creator);
		/**
		Wirtualny destruktor klasy MotionBasicQueriesService.
		*/
		virtual ~MotionBasicQueriesWS();

		//! \param id Identyfikator użytkownika
		//! \return Opis XML dla danego użytkownika
		virtual const std::string getPerformerByIdXML(const int id) const;

		//! \param id Identyfikator sesji
		//! \return Opis XML dla danej sesji
		virtual const std::string getSessionByIdXML(const int id) const;

		//! \param id Identyfikator sesji
		//! \return Nazwa sesji
		virtual const std::string getSessionLabel(const int id) const;

		//! \param id Identyfikator próby pomiarowej
		//! \return Opis XML dla danej próby pomiarowej
		virtual const std::string getTrialByIdXML(const int id) const;

		//! \param id Identyfikator konfiguracji pomiarowej
		//! \return Opis XML dla danej konfiguracji pomiarowej
		virtual const std::string getMeasurementConfigurationByIdXML(const int id) const;

		//! \param id Identyfikator konfiguracji aktora
		//! \return Opis XML dla danej konfiguracji aktora
		virtual const std::string getPerformerConfigurationByIdXML(const int id) const;

		//! \return Opis XML dla aktorów
		virtual const std::string listPerformersXML() const;

		//! \return Opis XML dla aktorów wraz z ich atrybutami
		virtual const std::string listPerformersWithAttributesXML() const;

		//! \param labID Identyfikator laboratorium
		//! \return Opis XML dla aktorów wraz z ich atrybutami dla zadanego laboratorium
		virtual const std::string listLabPerformersWithAttributesXML(const int labID) const;

		//! \param labID Identyfikator sesji
		//! \return Opis XML dla aktorów wraz z ich atrybutami dla zadanej sesji
		virtual const std::string listSessionPerformersWithAttributesXML(const int sessionID) const;

		//! \param performerID Identyfikator aktora
		//! \return Opis XML dla sesji aktora
		virtual const std::string listPerformerSessionsXML(const int performerID) const;

		//! \param performerID Identyfikator aktora
		//! \return Opis XML dla sesji aktora wraz z ich atrybutami
		virtual const std::string listPerformerSessionsWithAttributesXML(const int performerID) const;

		//! \param labID Identyfikator laboratorium
		//! \return Opis XML dla sesji wykonanych w danym laboratorium wraz z ich atrybutami
		virtual const std::string listLabSessionsWithAttributesXML(const int labID) const;

		//! \param sessionGroupID Identyfikator grupy sesji
		//! \return Opis XML dla sesji w ramach danej grupy sesji wraz z ich atrybutami
		virtual const std::string listGroupSessionsWithAttributesXML(const int sessionGroupID) const;

		//! \param measurementConfID Identyfikator konfiguracji pomiarowej
		//! \return Opis XML dla sesji związanych z daną konfiguracją pomiarową wraz z ich atrybutami
		virtual const std::string listMeasurementConfigSessionsWithAttributesXML(const int measurementConfID) const;

		//! \param sessionID Identyfikator sesji
		//! \return Opis XML dla grup sesji do których należy dana sesja
		virtual const std::string listSessionSessionGroups(const int sessionID) const;

		//! \param sessionID Identyfikator sesji
		//! \return Opis XML dla prób pomiarowych związanych z tą sesją
		virtual const std::string listSessionTrialsXML(const int sessionID) const;

		//! \param sessionID Identyfikator sesji
		//! \return Opis XML dla prób pomiarowych związanych z tą sesją wraz z ich atrybutami
		virtual const std::string listSessionTrialsWithAttributesXML(const int sessionID) const;

		//! \return Opis XML dla konfiguracji pomiarowych wraz z ich atrybutami
		virtual const std::string listMeasurementConfigurationsWithAttributesXML() const;

		//! \param sessionID Identyfikator sesji
		//! \return Opis XML dla konfiguracji aktorów wraz z atrybutami dla zadanej sesji
		virtual const std::string listSessionPerformerConfsWithAttributesXML(const int sessionID) const;

		//! \param subjectID Identyfikator sesji
		//! \param subjectType Typ plików
		//! \return Opis XML dla plików zadanego aktora i typu plików
		virtual const std::string listFilesXML(const int subjectID, const std::string & subjectType) const;

		//! \param subjectID Identyfikator sesji
		//! \param subjectType Typ plików
		//! \return Opis XML dla plików wraz z atrybutami zadanego aktora i typu plików
		virtual const std::string listFilesWithAttributesXML(const int subjectID, const std::string & subjectType) const;

		//! \param subjectID Identyfikator sesji
		//! \param subjectType Typ plików
		//! \return Opis XML dla plików wraz z atrybutami zadanego aktora i typu plików
		virtual const std::string listFileAttributeDataXML(const int subjectID, const std::string & subjectType) const;

		//! \param subjectID Identyfikator sesji
		//! \param subjectType Typ plików
		//! \return Opis XML dla plików wraz z atrybutami zadanego aktora i typu plików
		virtual const std::string listFileAttributeDataWithAttributesXML(const int subjectID, const std::string & subjectType) const;

		//! \param attributeGroupName Nazwa grupy atrybutów
		//! \param entityKind Typ encji
		//! \return Opis XML dla atrybutów dla zadanej encji w zdefiniowanej grupie
		virtual const std::string listAttributesDefined(const std::string & attributeGroupName,
			const std::string & entityKind) const;

		//! \param entityKind Typ encji
		//! \return Opis XML dla grup atrybutów dla zadanej encji
		virtual const std::string listAttributeGroupsDefined(const std::string & entityKind) const;

		//! \return Opis XML dla grup sesji
		virtual const std::string listSessionGroupsDefined() const;

		//! \return Opis XML dla typów ruchów
		virtual const std::string listMotionKindsDefined() const;

		//! \param attributName Nazwa atrybutu
		//! \param entityKind Typ encji
		//! \return Opis XML dla typów ruchów
		virtual const std::string listEnumValues(const std::string & attributeName,
			const std::string & entityKind) const;

		//! \return Data ostatniej modyfikacji danych ruchu
		virtual const hmdbServices::DateTime dataModificationTime() const;
		//! \return Data ostatniej modyfikacji metadanych ruchu
		virtual const hmdbServices::DateTime metadataModificationTime() const;

		//! Metoda zwraca listę adnotacji użytkownika dla danych USG - użytkownik brany z połączenia
		virtual const std::string listMyAnnotationsXML() const;

		virtual const std::string listAnnotationsXML() const;

		//! Metoda zwraca listę adnotacji oczekujących na weryfikację dla danych USG - użytkownik brany z połączenia
		virtual const std::string listMyAwaitingAnnotationsXML() const;

		//! Metoda zwraca listę adnotacji zatwierdzonych dla danych USG - użytkownik brany z połączenia
		virtual const std::string listMyReviewedAnnotationsXML() const;

	private:
		//! Serwis obsługujący tą usługę
		const WSDLServiceCreator creator;
	};

	class MedicalBasicQueriesWS : public hmdbServices::IMedicalBasicQueriesWS
	{
	public:
		/**
		Konstruktor klasy MotionBasicQueriesService.
		*/
		MedicalBasicQueriesWS(const WSDLServiceCreator creator);
		/**
		Wirtualny destruktor klasy MotionBasicQueriesService.
		*/
		virtual ~MedicalBasicQueriesWS();

		//! \return Data ostatniej modyfikacji danych pacjentów
		virtual const hmdbServices::DateTime dataModificationTime() const;
		//! \return Data ostatniej modyfikacji metadanych pacjentów
		virtual const hmdbServices::DateTime metadataModificationTime() const;

	private:
		//! Serwis obsługujący tą usługę
		const WSDLServiceCreator creator;
	};
}
#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__BASICQUERIESWS_H__
