/********************************************************************
    created:  2012/03/02
    created:  2:3:2012   14:38
    filename: UserPersonalSpaceWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___USERPERSONALSPACEWS_H__
#define HEADER_GUARD___USERPERSONALSPACEWS_H__

#include <webserviceslib/IUserPersonalSpaceWS.h>

namespace webservices
{
    class UserPersonalSpaceWS : public WebServiceT<IUserPersonalSpaceWS>
    {
	public:
		UserPersonalSpaceWS(const WSConnectionPtr & connection = WSConnectionPtr());
		virtual ~UserPersonalSpaceWS();
    };
}

#endif	//	HEADER_GUARD___USERPERSONALSPACEWS_H__
