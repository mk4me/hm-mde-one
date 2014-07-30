/********************************************************************
	created:  2014/06/02	13:28:35
	filename: HMDBSource.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCE_H__

#include <corelib/ISource.h>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <threadingUtils/SynchronizationPolicies.h>

class HMDBSourceWidget;

namespace hmdbCommunication
{
	class HMDBSource : public IHMDBSource, public plugin::ISource
	{

		UNIQUE_ID("{00001231-BACA-42C6-9A8E-706F561A563F}")
		CLASS_DESCRIPTION("HMDB Source", "HMDB Source")

	private:

		typedef std::map<QString, ContextConfiguration> ContextConfigurations;
		typedef utils::shared_ptr<IHMDBSourceContextView> SourceContextViewPtr;
		typedef std::map<SourceContextViewPtr, ContextConfigurations> ContextViews;

	public:
		//! Konstruktor domyslny
		HMDBSource();
		//! Destruktor wirtualny
		virtual ~HMDBSource();

		//! Inicjalizacja ürÛd≥a. NastÍpuje juø po wczytaniu pluginÛw i skonstruowaniu
		//! (nie zainicjalizowaniu!) wszystkich ürÛde≥.
		virtual void init(core::IMemoryDataManager * memoryDM,
			core::IStreamDataManager * streamDM,
			core::IFileDataManager * fileDM);

		//! Zobacz podobnπ metode dla IService
		//! PÛüna inicjalizacja ürÛd≥a, nastÍpuje po wczytaniu i inicjalizacji wszystkich innych ürÛde≥
		virtual bool lateInit();

		//! Metoda powinna w bezpieczny sposÛb zwalniac zasoby, majπc na uwadze øe niekoniecznie wszystkie us≥ugi i zasoby pobrane z zewnπtrz sπ jeszcze dostÍpne.
		//! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, ktÛre uruchomi≥ serwis, moøe tez zwalniac pamieÊ przydzielonπ dynamicznie
		//! Generalnie to taki bezpieczny destruktor uniezaleøniajπcy dana us≥ugÍ od pozosta≥ych us≥ug i przygotowujπcy ja na usuniÍcie
		virtual void finalize();

		//! Metoda aktualizujπca pochodzi z wπtku UI! Powinny tu byÊ realizowane lekkie operacje odúwieøania widgetÛw!!
		//! Jako parametr dostajemy przyrost czasu jaki mina≥ od poprzedniego wywo≥ania
		virtual void update(double deltaTime);

		//! èrÛd≥o nie musi mieÊ wizualnej reprezentacji.
		//! \return Widget tworzony przez ürÛd≥o bπdü NULL.
		virtual QWidget* getWidget();
		//! \return Widget kontrolujπcy zachowanie us≥ugi/us≥ug zaleønych.
		virtual QWidget* getControlWidget();
		//! \return Widget dostarczajπcy opcji zwiπzanych z us≥ugπ/us≥ugami zaleønymi.
		virtual QWidget* getSettingsWidget();
		//! \param offeredTypes Typy oferowane przez to ürÛd≥o
		virtual void getOfferedTypes(utils::TypeInfoList & offeredTypes) const;

		//-----------------------------------------------------------------

		//! \param session Sesja us≥ug bazy danych ruchu
		//! \param storage Obiekt do przechowywania danych
		//! \return Konteks ürÛdla danych
		virtual IHMDBSourceContext * createSourceContext(IHMDBSession * session,
			IHMDBStorage * storage, const std::string & userName,
			const std::string & userHash);

		//! \param prototype Prototyp widoku kontekstÛw
		virtual void registerSourceContextViewPrototype(IHMDBSourceContextView * prototype);
		//! \return Iloúc zarejestrowanych prototypÛw widokÛw
		virtual const unsigned int sourceContextViewPrototypesCount() const;
		//! \param idx Indeks prototypu widoku
		//! \return Prototyp widoku
		virtual IHMDBSourceContextView * sourceContextViewPrototype(const unsigned int idx);
		//! \param idx Indeks prototypu widoku
		//! \return Prototyp widoku
		virtual const IHMDBSourceContextView * sourceContextViewPrototype(const unsigned int idx) const;

		//! \param prototype Prototyp widoku kontekstÛw dla ktÛrego chcemy zarejestrowaÊ konfiguracjÍ
		//! \param config Konfiguracja widoku
		//! \return Czy uda≥o siÍ zarejestrowaÊ konfiguracjÍ (nazwa musi byÊ unikalna!!)
		virtual const bool registerSourceContextConfiguration(IHMDBSourceContextView * prototype,
			const ContextConfiguration & config);
		//! \param prototype Prototyp widoku kontekstÛw
		//! \return Iloúc zarejestrowanych konfiguracji widoku
		const unsigned int sourceContextConfigurationsCount(IHMDBSourceContextView * prototype) const;
		//! \param prototype Prototyp widoku kontekstÛw
		//! \param idx Indeks konfiguracji
		//! \return Konfiguracja
		const ContextConfiguration sourceContextConfiguration(IHMDBSourceContextView * prototype,
			const unsigned int idx) const;

		//! \param url Adres serwisu gdzie zak≥adamy konto
		//! \param login Nazwa uøytkownika dla ktÛrego chcemy za≥oøyc konto
		//! \param password Has≥o nowego konta
		//! \param name ImiÍ nowego uøytkownika
		//! \param surname Nazwisko nowego uøytkownika
		//! \param email Email nowego uøytkownika
		virtual void registerAccount(const std::string & url,
			const std::string & login, const std::string & password,
			const std::string & name, const std::string & surname,
			const std::string & email, const bool multi = true,
			const std::string & caPath = std::string());


		//! \param url Adres serwisu gdzie aktywujemy konto
		//! \param login Nazwa uøytkownika dla ktÛrego chcemy przeprowadziÊ aktywacjÍ
		//! \param activationCode Kod aktywacyjny
		virtual void activateAccount(const std::string & url,
			const std::string & login, const std::string & activationCode,
			const bool multi = true,
			const std::string & caPath = std::string());

		//! \param url Adres serwisu gdzie aktywujemy konto
		//! \param email Adres email zwiπzany z kontem na ktÛry wys≥ane zostanπ dalsze instrukcje
		virtual void resetPassword(const std::string & url, const std::string & email,
			const bool multi = true,
			const std::string & caPath = std::string());

		//! \return IloúÊ obs≥ugiwanych kontekstÛw danych
		virtual const unsigned int size() const;
		//! \param idx Indeks kontekstu danych
		//! \return Konteks danych dla zadanego indeksu
		virtual IHMDBSourceContext * sourceContext(const unsigned int idx);
		//! \param idx Indeks kontekstu danych
		//! \return Konteks danych dla zadanego indeksu
		virtual const IHMDBSourceContext * sourceContext(const unsigned int idx) const;
		//! \param data Dane
		//! \return Konteks danych lub nullptr jeúli nie znaleziono
		//virtual IHMDBSourceContext * sourceContextForData(core::VariantConstPtr data);
		//! \param data Dane
		//! \return Konteks danych lub nullptr jeúli nie znaleziono
		//virtual const IHMDBSourceContext * sourceContextForData(core::VariantConstPtr data) const;

	private:
		//! Widget
		HMDBSourceWidget * mainWidget;
		//! Obiekt synchronizujπcy
		mutable threadingUtils::RecursiveSyncPolicy sync_;
		//! Lista kontekstÛw danych
		std::list<IHMDBSourceContext*> sourceContexts_;
		//! Manager pamiÍci
		core::IMemoryDataManager * memoryDM;
		//! Manager strumieni
		core::IStreamDataManager * streamDM;
		//! Manager danych
		core::IFileDataManager * fileDM;
		//! Prototypy kontekstÛw
		ContextViews contextViews;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCE_H__