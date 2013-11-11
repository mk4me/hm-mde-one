/********************************************************************
    created:  2012/02/28
    created:  28:2:2012   22:39
    filename: IBasicUpdatesWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IBASICUPDATESWS_H__
#define HEADER_GUARD___IBASICUPDATESWS_H__

#include <set>
#include <webserviceslib/IWebService.h>
#include <webserviceslib/Entity.h>

namespace webservices
{

//! Interfejs usługi pozwalającej edytować dane w bazie
class IMotionBasicUpdatesWS : public IWebService
{
public:
    //! Wirtualny destruktor
    virtual ~IMotionBasicUpdatesWS() {}

    //! \param performerData Dane aktora
	//! \return Identyfikator nowego aktora
	virtual const int createPerformer(const xmlWsdl::PerformerData & performerData) = 0;

	//! \param labID Identyfikator laboratorium w którym przeprowadzono sesję
	//! \param motionKindName Nazwa typu ruchu nagrywanego podczas sesji
	//! \param sessionDate Data sesji
	//! \param sessionDescription Opis sesji
	//! \param sessionGroupIDs Identyfikatory grup sesji do tkóej nowa sesja ma należeć
	//! \return Identyfikator nowej sesji
	virtual const int createSession(const int labID, const std::string & motionKindName,
		const DateTime & sessionDate, const std::string & sessionDescription,
		const std::set<int> & sessionGroupIDs) = 0;

	//! \param sessionID Identyfikator sesji w ramach której przeprowadzony był trial (próba pomiarowa)
	//! \param trialDescription Opis próby pomiarowej
	//! \return Identyfokator nowej próby pomiarowej
	virtual const int createTrial(const int sessionID, const std::string & trialDescription) = 0;

	//! \param mcName Nazwa konfiguracji pomiarowej
	//! \param mcKind Typ konfiguracji pomiarowej
	//! \param mcDescription Opis konfiguracji pomiarowej
	//! \return Identyfokator nowej konfiguracji pomiarowej
	virtual const int createMeasurementConfiguration(const std::string & mcName,
		const std::string & mcKind, const std::string & mcDescription) = 0;

	//! \param sessionID Identyfikator przypisywanej sesji
	//! \param sessionsGroupID Identyfikator grupy sesji
	//! \return Wartość logiczna informująca o powodzeniu operacji (TODO - wyjaśnić)
	virtual const int assignSessionToGroup(const int sessionID, const int sessionsGroupID) = 0;

	//! \param performerID Identyfikator przypisywanego aktora
	//! \param sessionID Identyfikator sesji
	//! \return Identyfikator powstałej konfiguracji aktora
	virtual const int assignPerformerToSession(const int performerID, const int sessionID) = 0;


	//! \param performerID Identyfikator przypisywanego aktora
	//! \param measurementID Identyfikator pomiaru
	//! \return Rezultat operacji - powodzenie/niepowodzenie przypisania
	virtual const bool assignPerformerToMeasurement(const int performerID, const int measurementID) = 0;


	//! \param performerID Identyfikator aktora
	//! \param attributeName Nazwa atrybutu
	//! \param attributeValue Wartośc atrybutu
	//! \param update Czy aktualizować wartość jeśli istnieje
	virtual void setPerformerAttribute(const int performerID, const std::string & attributeName,
		const std::string & attributeValue, const bool update) = 0;


	//! \param sessionID Identyfikator sesji
	//! \param attributeName Nazwa atrybutu
	//! \param attributeValue Wartośc atrybutu
	//! \param update Czy aktualizować wartość jeśli istnieje
	virtual void setSessionAttribute(const int sessionID, const std::string & attributeName,
		const std::string & attributeValue, const bool update) = 0;


	//! \param trialID Identyfikator próby pomiarowej
	//! \param attributeName Nazwa atrybutu
	//! \param attributeValue Wartośc atrybutu
	//! \param update Czy aktualizować wartość jeśli istnieje
	virtual void setTrialAttribute(const int trialID, const std::string & attributeName,
		const std::string & attributeValue, const bool update) = 0;

	//! \param measurementConfID Identyfikator konfiguracji pomiarowej
	//! \param attributeName Nazwa atrybutu
	//! \param attributeValue Wartośc atrybutu
	//! \param update Czy aktualizować wartość jeśli istnieje
	virtual void setMeasurementConfAttribute(const int measurementConfID, const std::string & attributeName,
		const std::string & attributeValue, const bool update) = 0;


	//! \param performerConfID Identyfikator konfiguracji aktora
	//! \param attributeName Nazwa atrybutu
	//! \param attributeValue Wartośc atrybutu
	//! \param update Czy aktualizować wartość jeśli istnieje
	virtual void setPerformerConfAttribute(const int performerConfID, const std::string & attributeName,
		const std::string & attributeValue, const bool update) = 0;

	//! \param fileID Identyfikator pliku
	//! \param attributeName Nazwa atrybutu
	//! \param attributeValue Wartośc atrybutu
	//! \param update Czy aktualizować wartość jeśli istnieje
	virtual void setFileAttribute(const int fileID, const std::string & attributeName,
		const std::string & attributeValue, const bool update) = 0;

	//! \param resourceID Identyfikator zasobu
	//! \param entity Nazwa encji
	//! \param attributeName Nazwa atrybutu
	//! \param fileID Identyfikator pliku
	//! \param update Czy aktualizować wartość jeśli istnieje
	virtual void setFileTypedAttributeValue(const int resourceID, const std::string & entity,
		const std::string & attributeName, const int fileID, const bool update) = 0;

	//! \param resourceID Identyfikator zasobu
	//! \param attributeName Nazwa atrybutu
	//! \param entity Nazwa encji
	virtual void clearAttributeValue(const int resourceID, const std::string & attributeName,
		const std::string & entity) = 0;
};

typedef boost::shared_ptr<IMotionBasicUpdatesWS> MotionBasicUpdatesWSPtr;
typedef boost::shared_ptr<const IMotionBasicUpdatesWS> MotionBasicUpdatesWSConstPtr;

class IMedicalBasicUpdatesWS : public IWebService
{
public:
    virtual ~IMedicalBasicUpdatesWS() {}
};

typedef boost::shared_ptr<IMedicalBasicUpdatesWS> MedicalBasicUpdatesWSPtr;
typedef boost::shared_ptr<const IMedicalBasicUpdatesWS> MedicalBasicUpdatesWSConstPtr;

}

#endif	//	HEADER_GUARD___IBASICUPDATESWS_H__
