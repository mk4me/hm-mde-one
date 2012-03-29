/********************************************************************
    created:  2012/03/02
    created:  2:3:2012   14:16
    filename: IWebService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IWEBSERVICE_H__
#define HEADER_GUARD___IWEBSERVICE_H__

#include <OpenThreads/ReentrantMutex>
#include <OpenThreads/ScopedLock>
#include <webserviceslib/IWSConnection.h>

namespace webservices
{
    //! Podstawowy interfejs ka¿dedo web serwisu
    class IWebService : public OpenThreads::ReentrantMutex
    {
	public:

		typedef OpenThreads::ReentrantMutex Mutex;
		typedef OpenThreads::ScopedLock<Mutex> ScopedLock;

    public:

        //! Wirtualny destruktor
        virtual ~IWebService() {}

        //! \param connection Po³¹czenie przez które bêdzie realizowany serwis
        virtual void setConnection(const WSConnectionPtr & connection) = 0;
        //! \return Po³¹czenie przez które jest realizowany serwis
        virtual const WSConnectionPtr & connection() = 0;
        //! \return Po³¹czenie przez które jest realizowany serwis
        virtual const WSConnectionConstPtr & connection() const = 0;
    };

	typedef boost::shared_ptr<IWebService> WebServicePtr;
	typedef boost::shared_ptr<const IWebService> WebServiceConstPtr;
}

#endif	//	HEADER_GUARD___IWEBSERVICE_H__
