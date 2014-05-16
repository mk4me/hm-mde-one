/********************************************************************
	created:  2012/03/02
	created:  2:3:2012   14:38
	filename: UserPersonalSpaceWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___USERPERSONALSPACEWS_H__
#define HEADER_GUARD___USERPERSONALSPACEWS_H__

#include <hmdbserviceslib/IUserPersonalSpaceWS.h>
#include <networkUtils/IWSDLService.h>

namespace hmdbServices
{
	class UserPersonalSpaceWS : public IUserPersonalSpaceWS
	{
	public:
		UserPersonalSpaceWS(networkUtils::IWSDLServicePtr wsdlService);
		virtual ~UserPersonalSpaceWS();

	private:
		networkUtils::IWSDLServicePtr wsdlService;
	};
}

#endif	//	HEADER_GUARD___USERPERSONALSPACEWS_H__
