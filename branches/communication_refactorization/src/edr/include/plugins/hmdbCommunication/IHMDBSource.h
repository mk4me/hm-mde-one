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

		//! Struktura opisuj�ca konfiguracj� u�ytkownika
		struct UserCredentialsConfiguration
		{
			QString user;		//! User
			QString password;	//! Has�o
		};

		//! Struktura opisuj�ca konfiguracj� storage
		struct StorageConfiguration
		{
			QString path;		//! �cie�ka do storage
			QString password;	//! Has�o
		};

		struct ServiceConfiguration
		{
			QString url;		//! Adres us�ugi
			QString caPath;		//! �cie�ka certyfikatu SSL
		};

		//! Struktura opisuj�ca dost�p do us�ug zdalnych
		struct RemoteConfiguration
		{
			ServiceConfiguration serviceConfiguration;
			UserCredentialsConfiguration userConfiguration;
		};

		//! Struktura opisuj�ca konfiguracje ca�ego kontekstu
		struct ContextConfiguration
		{
			QString name;										//! Nazwa konfiguracji (unikalna!!)
			StorageConfiguration storageConfiguration;			//! Konfiguracja storage
			RemoteConfiguration motionServicesConfiguration;	//! Konfiguracja us�ug ruchu
			RemoteConfiguration motionDataConfiguration;		//! Konfiguracja danych ruchu
			RemoteConfiguration medicalServicesConfiguration;	//! Konfiguracja us�ug medycznych
			RemoteConfiguration medicalDataConfiguration;		//! Konfiguracja danych medycznych
		};

		//! Interfejs widok�w kontekstu �r�d�a
		class IHMDBSourceContextView
		{
		public:
			//! Destruktor wirtualny
			virtual ~IHMDBSourceContextView() {}
			//! \return Nazwa widoku
			virtual const QString name() const = 0;
			//! \param shallowCopyContext Kontekst p�ytkiej kopii bazy danych jakim zasilamy widok
			//! \return Widok obs�uguj�cy kontekst
			virtual QWidget * createView(IHMDBShallowCopyContextPtr shallowCopyContext) = 0;
			//! \return Ikona widoku
			virtual const QIcon icon() const { return QIcon(); };
			//! \return Czy dany widok wymaga po��czenia z us�ugami webowymi
			virtual const bool requiresRemoteContext() const = 0;
		};

	public:
		//! Destruktor wirtualny
		virtual ~IHMDBSource() {}

		//! \param storage Miejsce sk��dowania danych
		//! \param user Nazwa u�ytkownika
		//! \param password	Has�o u�ytkownika
		//! \param session Sesja us�ug zdalnych
		//! \return Kontekst �r�d�a dancyh
		virtual const IHMDBSourceContextPtr createSourceContext(IHMDBStoragePtr storage,
			const std::string & user, const std::string & password,
			IHMDBSessionPtr session = nullptr) = 0;		

		//! \param sourceContext Kontekst �r�d�a danych
		//! \return Konteks p�ytkiej kopii bazy danych
		virtual const IHMDBShallowCopyContextPtr createShallowCopyContext(IHMDBSourceContextPtr sourceContext) = 0;

		//! \param prototype Prototyp widoku kontekst�w
		virtual void registerSourceContextViewPrototype(IHMDBSourceContextView * prototype) = 0;
		//! \return Ilo�c zarejestrowanych prototyp�w widok�w
		virtual const unsigned int sourceContextViewPrototypesCount() const = 0;
		//! \param idx Indeks prototypu widoku
		//! \return Prototyp widoku
		virtual IHMDBSourceContextView * sourceContextViewPrototype(const unsigned int idx) = 0;
		//! \param idx Indeks prototypu widoku
		//! \return Prototyp widoku
		virtual const IHMDBSourceContextView * sourceContextViewPrototype(const unsigned int idx) const = 0;

		//! \param prototype Prototyp widoku kontekst�w dla kt�rego chcemy zarejestrowa� konfiguracj�
		//! \param config Konfiguracja widoku
		//! \return Czy uda�o si� zarejestrowa� konfiguracj� (nazwa musi by� unikalna!!)
		virtual const bool registerSourceContextViewConfiguration(IHMDBSourceContextView * prototype,
			const ContextConfiguration & config) = 0;

		//! \param prototype Prototyp widoku dla kt�rego szukam predefiniowanych konfiguracji
		virtual const unsigned int sourceContextViewConfigurationsCount(IHMDBSourceContextView * prototype) const = 0;

		//! \param prototype Prototyp widoku dla kt�rego szukam predefiniowanych konfiguracji
		//! \param idx Indeks konfiguracji
		//! \return Konfiguracja
		virtual const ContextConfiguration sourceContextViewConfiguration(IHMDBSourceContextView * prototype, const unsigned int idx) const = 0;

		//! \param url Adres serwisu gdzie zak�adamy konto
		//! \param login Nazwa u�ytkownika dla kt�rego chcemy za�o�yc konto
		//! \param password Has�o nowego konta
		//! \param name Imi� nowego u�ytkownika
		//! \param surname Nazwisko nowego u�ytkownika
		//! \param email Email nowego u�ytkownika
		//! \param multi Czy rejestrowa� w bazie ruchu i medycznej
		//! \param caPath �cie�ka certyfikatu do uwie�ytelnienia us�ugi
		virtual void registerAccount(const std::string & url,
			const std::string & login, const std::string & password,
			const std::string & name, const std::string & surname,
			const std::string & email, const bool multi = true,
			const std::string & caPath = std::string()) = 0;

		//! \param url Adres serwisu gdzie aktywujemy konto
		//! \param login Nazwa u�ytkownika dla kt�rego chcemy przeprowadzi� aktywacj�
		//! \param activationCode Kod aktywacyjny
		//! \param multi Czy rejestrowa� w bazie ruchu i medycznej
		//! \param caPath �cie�ka certyfikatu do uwie�ytelnienia us�ugi
		virtual void activateAccount(const std::string & url,
			const std::string & login, const std::string & activationCode,
			const bool multi = true,
			const std::string & caPath = std::string()) = 0;

		//! \param url Adres serwisu gdzie aktywujemy konto
		//! \param email Adres email zwi�zany z kontem na kt�ry wys�ane zostan� dalsze instrukcje
		//! \param multi Czy rejestrowa� w bazie ruchu i medycznej
		//! \param caPath �cie�ka certyfikatu do uwie�ytelnienia us�ugi
		virtual void resetPassword(const std::string & url, const std::string & email,
			const bool multi = true,
			const std::string & caPath = std::string()) = 0;

		/*
		//! \return Ilo�� kontekst�w danych
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