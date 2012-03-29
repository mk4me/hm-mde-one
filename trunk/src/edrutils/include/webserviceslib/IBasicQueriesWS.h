/********************************************************************
    created:  2012/02/28
    created:  28:2:2012   22:41
    filename: IBasicQueriesWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IBASICQUERIESWS_H__
#define HEADER_GUARD___IBASICQUERIESWS_H__

#include <webserviceslib/IWebService.h>
#include <webserviceslib/Entity.h>

namespace webservices
{

//! Interfejs dostarczaj¹cy informacji o czasie ostatnich zmian danych po stronie bazy danych
class IGeneralBasicQueriesWS : public IWebService
{
public:
    //! Wirtualny destruktor
    virtual ~IGeneralBasicQueriesWS() {}
    //! \return Czas ostatniej zmiany danych
    virtual const DateTime dataModificationTime() = 0;
    //! \return Czas ostatniej zmiany metadanych
    virtual const DateTime metadataModificationTime() = 0;
};

typedef boost::shared_ptr<IGeneralBasicQueriesWS> GeneralBasicQueriesWSPtr;
typedef boost::shared_ptr<const IGeneralBasicQueriesWS> GeneralBasicQueriesWSConstPtr;

//! Interfejs us³ugi zwi¹zanej z danymi ruchu
class IMotionBasicQueriesWS : public IGeneralBasicQueriesWS
{
public:
    //! Wirtualny destruktor
    virtual ~IMotionBasicQueriesWS() {}

    /**
	Listuje próby pomiarowe sesji.
	@param sessionID id sesji której próby pomiarowe maj¹ zostaæ wylistowane
	@return zbiór prób pomiarowych dla wybranej sesji
	*/
	virtual const std::vector<motionWsdl::Trial> listSessionTrials(int sessionID) = 0;
	/**
	Listuje sesje dla danego laboratorium.
	@param labID id labu którego sesje maj¹ zostaæ wylistowane
	@return zbiór sesji dla wybranego labu
	*/
	virtual const std::vector<motionWsdl::Session> listLabSessionsWithAttributes(int labID) = 0;
	/**
	Listuje pliki dla encji typu subjectType i id ID.
	@param ID id encji dla której maj¹ zostaæ wylistowane pliki
	@param subjectType typ encji dla której maj¹ zostaæ wylistowane pliki
	@return zbiór plików dla wybranej encji
	*/
	virtual const std::vector<motionWsdl::File> listFiles(int ID, const std::string& subjectType) = 0;
	/**
	TODO: Listuje encje bazodanowe, sesje, podleg³e sesjom próby pomiarowe i
	informacje o plikach poszczególnych sesji i prób pomiarowych.
	@return lista prób pomiarowych, w przysz³oœci powinno znaleŸæ siê drzewko zale¿noœci miêdzy encjami
	*/
	virtual const std::vector<motionWsdl::Trial> listSessionContents() = 0;
};

typedef boost::shared_ptr<IMotionBasicQueriesWS> MotionBasicQueriesWSPtr;
typedef boost::shared_ptr<const IMotionBasicQueriesWS> MotionBasicQueriesWSConstPtr;

typedef IGeneralBasicQueriesWS IMedicalBasicQueriesWS;

typedef boost::shared_ptr<IMedicalBasicQueriesWS> MedicalBasicQueriesWSPtr;
typedef boost::shared_ptr<const IMedicalBasicQueriesWS> MedicalBasicQueriesWSConstPtr;

}

#endif	//	HEADER_GUARD___IBASICQUERIESWS_H__
