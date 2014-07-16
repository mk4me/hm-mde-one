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
	//! Interfejs serwisu do rejestracji i aktywacji kont w jednej usłudze - Med lub Motion
	class ISingleAccountFactoryWS : public IWebService
	{
	public:
		//! Destruktor wirtualny
		virtual ~ISingleAccountFactoryWS() {}

		//! \param login Login nowego konta
		//! \param email Skojarzone konto email (np. kod aktywacyjny, odzyskanie hasła)
		//! \param password Hasło dla nowego konta
		//! \param firstName Imię użytkownika
		//! \param lastName Nazwisko użytkownika
		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName) = 0;

		//! \param login Login który chcemy aktywować
		//! \param activationCode Kod aktywacyjny którym aktywujemy - przysdzedł na zarejestrowany email
		//! \return Prawda jeśli poprawnie aktywowano
		virtual const bool activateUserAccount(const std::string & login, const std::string & activationCode) = 0;

		//! \param email Konto pocztowe związane z naszym użytkownikiem, dla którego chcemy zmienić hasło
		//! \return Czy poprawnie zresetowano hasło
		virtual const bool resetPassword(const std::string & email) = 0;
	};

	typedef boost::shared_ptr<ISingleAccountFactoryWS> SingleAccountFactoryWSPtr;
	typedef boost::shared_ptr<const ISingleAccountFactoryWS> SingleAccountFactoryWSConstPtr;

	//! Interfejs serwisu do rejestracji i aktywacji kont w obu usługach - Med i Motion
	class IMultiAccountFactoryWS : public IWebService
	{
	public:
		//! Destruktor wirtualny
		virtual ~IMultiAccountFactoryWS() {}

		//! \param login Login nowego konta
		//! \param email Skojarzone konto email (np. kod aktywacyjny, odzyskanie hasła)
		//! \param password Hasło dla nowego konta
		//! \param firstName Imię użytkownika
		//! \param lastName Nazwisko użytkownika
		//! \param propagateToHMDB Informacja czy utworzyć automatycznie konta w obu usługach
		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName, const bool propagateToHMDB) = 0;

		//! \param login Login który chcemy aktywować
		//! \param activationCode Kod aktywacyjny którym aktywujemy - przysdzedł na zarejestrowany email
		//! \param propagateToHMDB Informacja czy aktywować automatycznie konta w obu usługach
		//! \return Prawda jeśli poprawnie aktywowano
		virtual const bool activateUserAccount(const std::string & login, const std::string & activationCode,
			const bool propagateToHMDB) = 0;

		//! \param email Konto pocztowe związane z naszym użytkownikiem, dla którego chcemy zmienić hasło
		//! \param propagateToHMDB Informacja czy aktywować automatycznie konta w obu usługach
		//! \return Czy poprawnie zresetowano hasło
		virtual const bool resetPassword(const std::string & email, const bool propagateToHMDB) = 0;
	};

	typedef boost::shared_ptr<IMultiAccountFactoryWS> MultiAccountFactoryWSPtr;
	typedef boost::shared_ptr<const IMultiAccountFactoryWS> MultiAccountFactoryWSConstPtr;
}

#endif	//	HEADER_GUARD___IACCOUNTFACTORYWS_H__
