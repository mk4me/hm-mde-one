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
	//! Interfejs serwisu do rejestracji i aktywacji kont w jednej us³udze - Med lub Motion
	class ISingleAccountFactoryWS : public IWebService
	{
	public:
		//! Destruktor wirtualny
		virtual ~ISingleAccountFactoryWS() {}

		//! \param login Login nowego konta
		//! \param email Skojarzone konto email (np. kod aktywacyjny, odzyskanie has³a)
		//! \param password Has³o dla nowego konta
		//! \param firstName Imiê u¿ytkownika
		//! \param lastName Nazwisko u¿ytkownika
		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName) = 0;

		//! \param login Login który chcemy aktywowaæ
		//! \param activationCode Kod aktywacyjny którym aktywujemy - przysdzed³ na zarejestrowany email
		//! \return Prawda jeœli poprawnie aktywowano
		virtual bool activateUserAccount(const std::string & login, const std::string & activationCode) = 0;
	};

	typedef boost::shared_ptr<ISingleAccountFactoryWS> SingleAccountFactoryWSPtr;
	typedef boost::shared_ptr<const ISingleAccountFactoryWS> SingleAccountFactoryWSConstPtr;

	//! Interfejs serwisu do rejestracji i aktywacji kont w obu us³ugach - Med i Motion
	class IMultiAccountFactoryWS : public IWebService
	{
	public:
		//! Destruktor wirtualny
		virtual ~IMultiAccountFactoryWS() {}

		//! \param login Login nowego konta
		//! \param email Skojarzone konto email (np. kod aktywacyjny, odzyskanie has³a)
		//! \param password Has³o dla nowego konta
		//! \param firstName Imiê u¿ytkownika
		//! \param lastName Nazwisko u¿ytkownika
		//! \param propagateToHMDB Informacja czy utworzyæ automatycznie konta w obu us³ugach
		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName, bool propagateToHMDB = false) = 0;

		//! \param login Login który chcemy aktywowaæ
		//! \param activationCode Kod aktywacyjny którym aktywujemy - przysdzed³ na zarejestrowany email
		//! \param propagateToHMDB Informacja czy aktywowaæ automatycznie konta w obu us³ugach
		//! \return Prawda jeœli poprawnie aktywowano
		virtual bool activateUserAccount(const std::string & login, const std::string & activationCode,
			bool propagateToHMDB = false) = 0;
	};

	typedef boost::shared_ptr<IMultiAccountFactoryWS> MultiAccountFactoryWSPtr;
	typedef boost::shared_ptr<const IMultiAccountFactoryWS> MultiAccountFactoryWSConstPtr;
}

#endif	//	HEADER_GUARD___IACCOUNTFACTORYWS_H__
