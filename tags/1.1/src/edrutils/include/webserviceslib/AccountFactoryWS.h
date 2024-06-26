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
	class SingleAccountFactoryWS : public WebServiceT<ISingleAccountFactoryWS>
	{
	public:
		SingleAccountFactoryWS(const WSConnectionPtr & connection = WSConnectionPtr());
		virtual ~SingleAccountFactoryWS();

		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName);

		virtual const bool activateUserAccount(const std::string & login, const std::string & activationCode);

		virtual const bool resetPassword(const std::string & email);
	};

	//! Klasa tworząca konto po stronie bazy danych (medycznej i ruchu)
	class MultiAccountFactoryWS : public WebServiceT<IMultiAccountFactoryWS>
	{
	public:
		MultiAccountFactoryWS(const WSConnectionPtr & connection = WSConnectionPtr());
		virtual ~MultiAccountFactoryWS();

		//! \param login Login użytkownika
		//! \param email Adres email użytkownika
		//! \param password Hasło użytkownika
		//! \param firstName Imię użytkownika
		//! \param lastName Nazwisko użytkownika
		//! \param propagateToHMDB Czy propagować tworzenie konta na bazę pacjentów
		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName, const bool propagateToHMDB);

		//! \param login Login użytkownika
		//! \param activationCode Kod aktywacyjny użytkownika	
		//! \param propagateToHMDB Czy propagować aktywację konta na bazę pacjentów
		//! \return Czy udało się aktywować konto
		virtual const bool activateUserAccount(const std::string & login, const std::string & activationCode,
			const bool propagateToHMDB);

		//! \param email Adres email użytkownika dla którego chcemy zresetować hasło
		//! \param propagateToHMDB Czy propagować reset hasła na bazę pacjentów
		//! \return Czy udało się zresetować hasło
		virtual const bool resetPassword(const std::string & email, const bool propagateToHMDB);
	};
}

#endif	//	HEADER_GUARD___ACCOUNTFACTORYWS_H__
