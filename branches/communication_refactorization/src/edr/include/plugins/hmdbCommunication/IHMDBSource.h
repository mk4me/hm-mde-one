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
#include <QtGui/QIcon>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <plugins/hmdbCommunication/IHMDBSourceContext.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>

class QWidget;

namespace hmdbCommunication
{
	class IShallowCopyFilterManager;


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
			StorageConfiguration storageConfiguration;			//! Konfiguracja storage
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
			//! \param shallowCopyContext Kontekst p³ytkiej kopii bazy danych jakim zasilamy widok
			//! \return Widok obs³uguj¹cy kontekst
			virtual QWidget * createView(IHMDBShallowCopyContextPtr shallowCopyContext) = 0;
			//! \return Ikona widoku
			virtual const QIcon icon() const { return QIcon(); };
			//! \return Czy dany widok wymaga po³¹czenia z us³ugami webowymi
			virtual const bool requiresRemoteContext() const = 0;
		};

	public:
		//! Destruktor wirtualny
		virtual ~IHMDBSource() {}

		//! \param storage Miejsce sk³¹dowania danych
		//! \param user Nazwa u¿ytkownika
		//! \param password	Has³o u¿ytkownika
		//! \param session Sesja us³ug zdalnych
		//! \return Kontekst Ÿród³a dancyh
		virtual const IHMDBSourceContextPtr createSourceContext(IHMDBStoragePtr storage,
			const std::string & user, const std::string & password,
			IHMDBSessionPtr session = nullptr) = 0;		

		//! \param sourceContext Kontekst Ÿród³a danych
		//! \return Konteks p³ytkiej kopii bazy danych
		virtual const IHMDBShallowCopyContextPtr createShallowCopyContext(IHMDBSourceContextPtr sourceContext) = 0;

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
		virtual const bool registerSourceContextViewConfiguration(IHMDBSourceContextView * prototype,
			const ContextConfiguration & config) = 0;

		//! \param prototype Prototyp widoku dla którego szukam predefiniowanych konfiguracji
		virtual const unsigned int sourceContextViewConfigurationsCount(IHMDBSourceContextView * prototype) const = 0;

		//! \param prototype Prototyp widoku dla którego szukam predefiniowanych konfiguracji
		//! \param idx Indeks konfiguracji
		//! \return Konfiguracja
		virtual const ContextConfiguration sourceContextViewConfiguration(IHMDBSourceContextView * prototype, const unsigned int idx) const = 0;

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

		/*
		//! \return Iloœæ kontekstów danych
		virtual const unsigned int size() const = 0;
		//! \param idx Indeks kontekstu danych
		//! \return Konteks danych dla zadanego indeksu
		virtual IHMDBSourceContext * sourceContext(const unsigned int idx) = 0;
		//! \param idx Indeks kontekstu danych
		//! \return Konteks danych dla zadanego indeksu
		virtual const IHMDBSourceContext * sourceContext(const unsigned int idx) const = 0;
		*/

		//! \return Manager filtrow danych
		virtual IShallowCopyFilterManager * filterManager() = 0;
		//! \return Manager filtrow danych
		virtual const IShallowCopyFilterManager * filterManager() const = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCE_H__