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
    class UserPersonalSpaceWS : public IUserPersonalSpaceWS
    {
	public:
		UserPersonalSpaceWS(const WSConnectionPtr & connection = WSConnectionPtr());
		virtual ~UserPersonalSpaceWS();

		//! \param connection Po³¹czenie przez które bêdzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
    };
}

#endif	//	HEADER_GUARD___USERPERSONALSPACEWS_H__
