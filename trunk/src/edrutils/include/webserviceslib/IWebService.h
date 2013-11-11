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
#include <boost/type_traits.hpp>
#include <utils/Debug.h>

namespace webservices
{
    //! Podstawowy interfejs każdedo web serwisu
    class IWebService
    {
    public:

        //! Wirtualny destruktor
        virtual ~IWebService() {}

        //! \param connection Połączenie przez które będzie realizowany serwis
        virtual void setConnection(const WSConnectionPtr & connection) = 0;
        //! \return Połączenie przez które jest realizowany serwis
        virtual const WSConnectionPtr & connection() = 0;
        //! \return Połączenie przez które jest realizowany serwis
        virtual const WSConnectionConstPtr & connection() const = 0;
    };

	typedef boost::shared_ptr<IWebService> WebServicePtr;
	typedef boost::shared_ptr<const IWebService> WebServiceConstPtr;

	template<class T>
	class WebServiceT : public T
	{

		UTILS_STATIC_ASSERT((boost::is_base_of<IWebService, T>::value), "Base class should inherit from IWebService");

	public:

		WebServiceT(const WSConnectionPtr & connection = WSConnectionPtr())
			: connection_(connection), constConnection_(connection)
		{

		}

		virtual ~WebServiceT() {}

		//! \param connection Połączenie przez które będzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection)
		{			
			connection_ = connection;
			constConnection_ = connection;
		}

		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection()
		{
			return connection_;
		}

		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const
		{
			return constConnection_;
		}	

	private:
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
	};
}

#endif	//	HEADER_GUARD___IWEBSERVICE_H__
