/**
@author Marek Daniluk
@brief Klasa MotionBasicUpdatesService dziedzicząca po WsdlConnection wykonująca operacje dokumentu MotionBasicUpdatesService.wsdl.
*/

#ifndef HEADER_GUARD_COMMUNICATION_BASICUPDATESSERVICE_H__
#define HEADER_GUARD_COMMUNICATION_BASICUPDATESSERVICE_H__

#include <webserviceslib/IBasicUpdatesWS.h>
#include <webserviceslib/IWebService.h>
#include <webserviceslib/Entity.h>

namespace webservices
{

	class MotionBasicUpdatesWS : public WebServiceT<IMotionBasicUpdatesWS>
	{
	public:
		/**
		Konstruktor klasy MotionBasicUpdatesService.
		*/
		MotionBasicUpdatesWS(const WSConnectionPtr & connection = WSConnectionPtr());
		/**
		Wirtualny destruktor klasy MotionBasicUpdatesService.
		*/
		virtual ~MotionBasicUpdatesWS();

		virtual const int createPerformer(const xmlWsdl::PerformerData & performerData);

		//! \param labID Identyfikator laboratorium w którym przeprowadzono sesję
		//! \param motionKindName Nazwa typu ruchu nagrywanego podczas sesji
		//! \param sessionDate Data sesji
		//! \param sessionDescription Opis sesji
		//! \param sessionGroupIDs Identyfikatory grup sesji do tkóej nowa sesja ma należeć
		//! \return Identyfikator nowej sesji
		virtual const int createSession(const int labID, const std::string & motionKindName,
			const DateTime & sessionDate, const std::string & sessionDescription,
			const std::set<int> & sessionGroupIDs);

		//! \param sessionID Identyfikator sesji w ramach której przeprowadzony był trial (próba pomiarowa)
		//! \param trialDescription Opis próby pomiarowej
		//! \return Identyfokator nowej próby pomiarowej
		virtual const int createTrial(const int sessionID, const std::string & trialDescription);

		//! \param mcName Nazwa konfiguracji pomiarowej
		//! \param mcKind Typ konfiguracji pomiarowej
		//! \param mcDescription Opis konfiguracji pomiarowej
		//! \return Identyfokator nowej konfiguracji pomiarowej
		virtual const int createMeasurementConfiguration(const std::string & mcName,
			const std::string & mcKind, const std::string & mcDescription);

		//! \param sessionID Identyfikator przypisywanej sesji
		//! \param sessionsGroupID Identyfikator grupy sesji
		//! \return Wartość logiczna informująca o powodzeniu operacji (TODO - wyjaśnić)
		virtual const int assignSessionToGroup(const int sessionID, const int sessionsGroupID);

		//! \param performerID Identyfikator przypisywanego aktora
		//! \param sessionID Identyfikator sesji
		//! \return Identyfikator powstałej konfiguracji aktora
		virtual const int assignPerformerToSession(const int performerID, const int sessionID);


		//! \param performerID Identyfikator przypisywanego aktora
		//! \param measurementID Identyfikator pomiaru
		//! \return Rezultat operacji - powodzenie/niepowodzenie przypisania
		virtual const bool assignPerformerToMeasurement(const int performerID, const int measurementID);


		//! \param performerID Identyfikator aktora
		//! \param attributeName Nazwa atrybutu
		//! \param attributeValue Wartośc atrybutu
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setPerformerAttribute(const int performerID, const std::string & attributeName,
			const std::string & attributeValue, const bool update);


		//! \param sessionID Identyfikator sesji
		//! \param attributeName Nazwa atrybutu
		//! \param attributeValue Wartośc atrybutu
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setSessionAttribute(const int sessionID, const std::string & attributeName,
			const std::string & attributeValue, const bool update);


		//! \param trialID Identyfikator próby pomiarowej
		//! \param attributeName Nazwa atrybutu
		//! \param attributeValue Wartośc atrybutu
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setTrialAttribute(const int trialID, const std::string & attributeName,
			const std::string & attributeValue, const bool update);

		//! \param measurementConfID Identyfikator konfiguracji pomiarowej
		//! \param attributeName Nazwa atrybutu
		//! \param attributeValue Wartośc atrybutu
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setMeasurementConfAttribute(const int measurementConfID, const std::string & attributeName,
			const std::string & attributeValue, const bool update);


		//! \param performerConfID Identyfikator konfiguracji aktora
		//! \param attributeName Nazwa atrybutu
		//! \param attributeValue Wartośc atrybutu
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setPerformerConfAttribute(const int performerConfID, const std::string & attributeName,
			const std::string & attributeValue, const bool update);

		//! \param fileID Identyfikator pliku
		//! \param attributeName Nazwa atrybutu
		//! \param attributeValue Wartośc atrybutu
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setFileAttribute(const int fileID, const std::string & attributeName,
			const std::string & attributeValue, const bool update);

		//! \param resourceID Identyfikator zasobu
		//! \param entity Nazwa encji
		//! \param attributeName Nazwa atrybutu
		//! \param fileID Identyfikator pliku
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setFileTypedAttributeValue(const int resourceID, const std::string & entity,
			const std::string & attributeName, const int fileID, const bool update);

		//! \param resourceID Identyfikator zasobu
		//! \param attributeName Nazwa atrybutu
		//! \param entity Nazwa encji
		virtual void clearAttributeValue(const int resourceID, const std::string & attributeName,
			const std::string & entity);
	};

    class MedicalBasicUpdatesWS : public WebServiceT<IMedicalBasicUpdatesWS>
    {
    public:
        MedicalBasicUpdatesWS(const WSConnectionPtr & connection = WSConnectionPtr());
        virtual ~MedicalBasicUpdatesWS();
    };
}
#endif
