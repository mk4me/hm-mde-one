/********************************************************************
	created:  2012/02/28
	created:  28:2:2012   22:36
	filename: IUserPersonalSpaceWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_IUSERPERSONALSPACEWS_H__
#define HEADER_GUARD__HMDBSERVICES_IUSERPERSONALSPACEWS_H__

#include <utils/SmartPtr.h>

namespace hmdbServices
{
	//! Interfejs usługi zarządzającej prywatną przestrzenią użytkownika w bazie
	class IUserPersonalSpaceWS
	{
	public:
		//! Wirtualny destruktor
		virtual ~IUserPersonalSpaceWS() {}
	};

	typedef utils::shared_ptr<IUserPersonalSpaceWS> UserPersonalSpaceWSPtr;
	typedef utils::shared_ptr<const IUserPersonalSpaceWS> UserPersonalSpaceWSConstPtr;
}

#endif	//	HEADER_GUARD__HMDBSERVICES_IUSERPERSONALSPACEWS_H__
