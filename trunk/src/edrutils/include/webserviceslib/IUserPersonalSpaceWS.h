/********************************************************************
    created:  2012/02/28
    created:  28:2:2012   22:36
    filename: IUserPersonalSpaceWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IUSERPERSONALSPACEWS_H__
#define HEADER_GUARD___IUSERPERSONALSPACEWS_H__

#include <webserviceslib/IWebService.h>

namespace webservices
{

//! Interfejs us�ugi zarz�dzaj�cej prywatn� przestrzeni� u�ytkownika w bazie
class IUserPersonalSpaceWS : public IWebService
{
public:
    //! Wirtualny destruktor
    virtual ~IUserPersonalSpaceWS() {}
};

typedef boost::shared_ptr<IUserPersonalSpaceWS> UserPersonalSpaceWSPtr;
typedef boost::shared_ptr<const IUserPersonalSpaceWS> UserPersonalSpaceWSConstPtr;

}

#endif	//	HEADER_GUARD___IUSERPERSONALSPACEWS_H__
