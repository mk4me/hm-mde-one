/********************************************************************
    created:  2012/04/06
    created:  6:4:2012   8:54
    filename: AccountFactoryWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ACCOUNTFACTORYWS_H__
#define HEADER_GUARD___ACCOUNTFACTORYWS_H__

#include <webserviceslib/IAccountFactoryWS.h>

namespace webservices{

	//! Klasa tworząca konto po stronie bazy danych
	class SingleAccountFactoryWS : public ISingleAccountFactoryWS
	{
	public:
		SingleAccountFactoryWS(const WSConnectionPtr & connection = WSConnectionPtr());
		virtual ~SingleAccountFactoryWS();

		//! \param connection Połączenie przez które będzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName);

		virtual bool activateUserAccount(const std::string & login, const std::string & activationCode);

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
	};

	//! Klasa tworząca konto po stronie bazy danych (medycznej i ruchu)
	class MultiAccountFactoryWS : public IMultiAccountFactoryWS
	{
	public:
		MultiAccountFactoryWS(const WSConnectionPtr & connection = WSConnectionPtr());
		virtual ~MultiAccountFactoryWS();

		//! \param connection Połączenie przez które będzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName, bool propagateToHMDB);

		virtual bool activateUserAccount(const std::string & login, const std::string & activationCode,
			bool propagateToHMDB);

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
	};
}

#endif	//	HEADER_GUARD___ACCOUNTFACTORYWS_H__
