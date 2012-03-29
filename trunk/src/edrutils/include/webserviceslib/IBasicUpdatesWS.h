/********************************************************************
    created:  2012/02/28
    created:  28:2:2012   22:39
    filename: IBasicUpdatesWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IBASICUPDATESWS_H__
#define HEADER_GUARD___IBASICUPDATESWS_H__

#include <webserviceslib/IWebService.h>
#include <webserviceslib/Entity.h>

namespace webservices
{

//! Interfejs us³ugi pozwalaj¹cej edytowaæ dane w bazie
class IMotionBasicUpdatesWS : public IWebService
{
public:
    //! Wirtualny destruktor
    virtual ~IMotionBasicUpdatesWS() {}

    /**
	Manualne utworzenie encji performera w oparciu o dane z aplikacji klienckiej
	@param name imiê performera
	@param surname nazwisko performera
	@return utworzony Performer
	*/
	//virtual const motionWsdl::Performer createPerformer() = 0;
	/**
	Manualne utworzenie encji sesji przez u¿ytkownika
	@param userID identyfikator u¿ytkownika
	@param labID identyfikator laboratorium
	@param performerID identyfikator performera
	@param sessionDate data powstania sesji
	@param sessionDescription opis sesji
	@param motionKindName ???
	@param groupsIDs referencja do wektora przechowuj¹cego identyfikatory grup do których ma nale¿eæ sesja
	@return wskaŸnik na obiekt klasy Session
	*/
	/*virtual const motionWsdl::Session createSession(int labID, int performerID, DateTime & sessionDate,
        const std::string & sessionDescription, const std::string & motionKindName,
        const std::vector<int> & groupsIDs) = 0;*/
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
