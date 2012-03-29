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

//! Interfejs dostarczaj�cy informacji o czasie ostatnich zmian danych po stronie bazy danych
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

//! Interfejs us�ugi zwi�zanej z danymi ruchu
class IMotionBasicQueriesWS : public IGeneralBasicQueriesWS
{
public:
    //! Wirtualny destruktor
    virtual ~IMotionBasicQueriesWS() {}

    /**
	Listuje pr�by pomiarowe sesji.
	@param sessionID id sesji kt�rej pr�by pomiarowe maj� zosta� wylistowane
	@return zbi�r pr�b pomiarowych dla wybranej sesji
	*/
	virtual const std::vector<motionWsdl::Trial> listSessionTrials(int sessionID) = 0;
	/**
	Listuje sesje dla danego laboratorium.
	@param labID id labu kt�rego sesje maj� zosta� wylistowane
	@return zbi�r sesji dla wybranego labu
	*/
	virtual const std::vector<motionWsdl::Session> listLabSessionsWithAttributes(int labID) = 0;
	/**
	Listuje pliki dla encji typu subjectType i id ID.
	@param ID id encji dla kt�rej maj� zosta� wylistowane pliki
	@param subjectType typ encji dla kt�rej maj� zosta� wylistowane pliki
	@return zbi�r plik�w dla wybranej encji
	*/
	virtual const std::vector<motionWsdl::File> listFiles(int ID, const std::string& subjectType) = 0;
	/**
	TODO: Listuje encje bazodanowe, sesje, podleg�e sesjom pr�by pomiarowe i
	informacje o plikach poszczeg�lnych sesji i pr�b pomiarowych.
	@return lista pr�b pomiarowych, w przysz�o�ci powinno znale�� si� drzewko zale�no�ci mi�dzy encjami
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
