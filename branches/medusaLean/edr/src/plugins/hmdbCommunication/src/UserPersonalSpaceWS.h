/********************************************************************
	created:  2012/03/02
	created:  2:3:2012   14:38
	filename: UserPersonalSpaceWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__hmdbCommunication_USERPERSONALSPACEWS_H__
#define HEADER_GUARD__hmdbCommunication_USERPERSONALSPACEWS_H__

#include "WSDLServiceTypes.h"
#include <hmdbserviceslib/IUserPersonalSpaceWS.h>

namespace hmdbCommunication
{
	class UserPersonalSpaceWS : public hmdbServices::IUserPersonalSpaceWS
	{
	public:
		UserPersonalSpaceWS(const WSDLServiceCreator creator);
		virtual ~UserPersonalSpaceWS();

	private:
		//! Serwis obs�uguj�cy t� us�ug�
		const WSDLServiceCreator creator;
	};
}

#endif	//	HEADER_GUARD__hmdbCommunication_USERPERSONALSPACEWS_H__
