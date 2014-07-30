/********************************************************************
	created:  2014/05/22	14:16:35
	filename: IHMDBSource.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCE_H__

#include <string>
#include <corelib/Variant.h>

class QWidget;

namespace hmdbCommunication
{
	//! Forward declaration
	class IHMDBSession;
	class IHMDBStorage;
	class IHMDBSourceContext;

	class IHMDBSource
	{
	public:

		//! Struktura opisuj¹ca konfiguracjê u¿ytkownika
		struct UserCredentialsConfiguration
		{
			QString user;		//! User
			QString password;	//! Has³o
		};

		//! Struktura opisuj¹ca konfiguracjê storage
		struct StorageConfiguration
		{
			QString path;		//! Œcie¿ka do storage
			QString password;	//! Has³o
		};

		struct ServiceConfiguration
		{
			QString url;		//! Adres us³ugi
			QString caPath;		//! Œcie¿ka certyfikatu SSL
		};

		//! Struktura opisuj¹ca dostêp do us³ug zdalnych
		struct RemoteConfiguration
		{
			ServiceConfiguration serviceConfiguration;
			UserCredentialsConfiguration userConfiguration;
		};

		//! Struktura opisuj¹ca konfiguracje ca³ego kontekstu
		struct ContextConfiguration
		{
			QString name;										//! Nazwa konfiguracji (unikalna!!)
			RemoteConfiguration motionServicesConfiguration;	//! Konfiguracja us³ug ruchu
			RemoteConfiguration motionDataConfiguration;		//! Konfiguracja danych ruchu
			RemoteConfiguration medicalServicesConfiguration;	//! Konfiguracja us³ug medycznych
			RemoteConfiguration medicalDataConfiguration;		//! Konfiguracja danych medycznych
		};

		//! Interfejs widoków kontekstu Ÿród³a
		class IHMDBSourceContextView
		{
		public:
			//! Destruktor wirtualny
			virtual ~IHMDBSourceContextView() {}
			//! \return Nazwa widoku
			virtual const QString name() const = 0;
			//! \param sourceContext Kontekst jakim zasilamy widok
			//! \return Widok obs³uguj¹cy kontekst
			virtual QWidget * createView(IHMDBSourceContext * sourceContext) = 0;
		};

	public:
		//! Destruktor wirtualny
		virtual ~IHMDBSource() {}
		
		//! \param session Sesja us³ug bazy danych ruchu
		//! \param storage Obiekt do przechowywania danych
		//! \param userHash Skrót u¿ytkownika na potrzeby obs³ugi shallowCopy w storage
		//! \return Konteks Ÿródla danych
		virtual IHMDBSourceContext * createSourceContext(IHMDBSession * session,
			IHMDBStorage * storage, const std::string & userName,
			const std::string & userHash) = 0;

		//! \param prototype Prototyp widoku kontekstów
		virtual void registerSourceContextViewPrototype(IHMDBSourceContextView * prototype) = 0;
		//! \return Iloœc zarejestrowanych prototypów widoków
		virtual const unsigned int sourceContextViewPrototypesCount() const = 0;
		//! \param idx Indeks prototypu widoku
		//! \return Prototyp widoku
		virtual IHMDBSourceContextView * sourceContextViewPrototype(const unsigned int idx) = 0;
		//! \param idx Indeks prototypu widoku
		//! \return Prototyp widoku
		virtual const IHMDBSourceContextView * sourceContextViewPrototype(const unsigned int idx) const = 0;

		//! \param prototype Prototyp widoku kontekstów dla którego chcemy zarejestrowaæ konfiguracjê
		//! \param config Konfiguracja widoku
		//! \return Czy uda³o siê zarejestrowaæ konfiguracjê (nazwa musi byæ unikalna!!)
		virtual const bool registerSourceContextConfiguration(IHMDBSourceContextView * prototype,
			const ContextConfiguration & config) = 0;

		//! \param url Adres serwisu gdzie zak³adamy konto
		//! \param login Nazwa u¿ytkownika dla którego chcemy za³o¿yc konto
		//! \param password Has³o nowego konta
		//! \param name Imiê nowego u¿ytkownika
		//! \param surname Nazwisko nowego u¿ytkownika
		//! \param email Email nowego u¿ytkownika
		//! \param multi Czy rejestrowaæ w bazie ruchu i medycznej
		//! \param caPath Œcie¿ka certyfikatu do uwie¿ytelnienia us³ugi
		virtual void registerAccount(const std::string & url,
			const std::string & login, const std::string & password,
			const std::string & name, const std::string & surname,
			const std::string & email, const bool multi = true,
			const std::string & caPath = std::string()) = 0;

		//! \param url Adres serwisu gdzie aktywujemy konto
		//! \param login Nazwa u¿ytkownika dla którego chcemy przeprowadziæ aktywacjê
		//! \param activationCode Kod aktywacyjny
		//! \param multi Czy rejestrowaæ w bazie ruchu i medycznej
		//! \param caPath Œcie¿ka certyfikatu do uwie¿ytelnienia us³ugi
		virtual void activateAccount(const std::string & url,
			const std::string & login, const std::string & activationCode,
			const bool multi = true,
			const std::string & caPath = std::string()) = 0;

		//! \param url Adres serwisu gdzie aktywujemy konto
		//! \param email Adres email zwi¹zany z kontem na który wys³ane zostan¹ dalsze instrukcje
		//! \param multi Czy rejestrowaæ w bazie ruchu i medycznej
		//! \param caPath Œcie¿ka certyfikatu do uwie¿ytelnienia us³ugi
		virtual void resetPassword(const std::string & url, const std::string & email,
			const bool multi = true,
			const std::string & caPath = std::string()) = 0;

		//! \return Iloœæ kontekstów danych
		virtual const unsigned int size() const = 0;
		//! \param idx Indeks kontekstu danych
		//! \return Konteks danych dla zadanego indeksu
		virtual IHMDBSourceContext * sourceContext(const unsigned int idx) = 0;
		//! \param idx Indeks kontekstu danych
		//! \return Konteks danych dla zadanego indeksu
		virtual const IHMDBSourceContext * sourceContext(const unsigned int idx) const = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCE_H__