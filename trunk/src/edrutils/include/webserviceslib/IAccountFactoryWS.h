/********************************************************************
    created:  2012/04/06
    created:  6:4:2012   8:49
    filename: IAccountFactoryWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IACCOUNTFACTORYWS_H__
#define HEADER_GUARD___IACCOUNTFACTORYWS_H__

#include <webserviceslib/IWebService.h>

namespace webservices
{

	//! Interfejs serwisu do rejestracji i aktywacji kont
	class IAccountFactoryWS : public IWebService
	{
	public:
		//! Destruktor wirtualny
		virtual ~IAccountFactoryWS() {}

		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName) = 0;

		virtual bool activateUserAccount(const std::string & login, const std::string & activationCode) = 0;
	};

	typedef boost::shared_ptr<IAccountFactoryWS> AccountFactoryWSPtr;
	typedef boost::shared_ptr<const IAccountFactoryWS> AccountFactoryWSConstPtr;

}

#endif	//	HEADER_GUARD___IACCOUNTFACTORYWS_H__
