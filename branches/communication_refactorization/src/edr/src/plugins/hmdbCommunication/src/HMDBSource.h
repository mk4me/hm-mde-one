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

	public:
		//! Konstruktor domyslny
		HMDBSource();
		//! Destruktor wirtualny
		virtual ~HMDBSource();

		//! Inicjalizacja �r�d�a. Nast�puje ju� po wczytaniu plugin�w i skonstruowaniu
		//! (nie zainicjalizowaniu!) wszystkich �r�de�.
		virtual void init(core::IMemoryDataManager * memoryDM,
			core::IStreamDataManager * streamDM,
			core::IFileDataManager * fileDM);

		//! Zobacz podobn� metode dla IService
		//! P�na inicjalizacja �r�d�a, nast�puje po wczytaniu i inicjalizacji wszystkich innych �r�de�
		virtual bool lateInit();

		//! Metoda powinna w bezpieczny spos�b zwalniac zasoby, maj�c na uwadze �e niekoniecznie wszystkie us�ugi i zasoby pobrane z zewn�trz s� jeszcze dost�pne.
		//! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, kt�re uruchomi� serwis, mo�e tez zwalniac pamie� przydzielon� dynamicznie
		//! Generalnie to taki bezpieczny destruktor uniezale�niaj�cy dana us�ug� od pozosta�ych us�ug i przygotowuj�cy ja na usuni�cie
		virtual void finalize();

		//! Metoda aktualizuj�ca pochodzi z w�tku UI! Powinny tu by� realizowane lekkie operacje od�wie�ania widget�w!!
		//! Jako parametr dostajemy przyrost czasu jaki mina� od poprzedniego wywo�ania
		virtual void update(double deltaTime);

		//! �r�d�o nie musi mie� wizualnej reprezentacji.
		//! \return Widget tworzony przez �r�d�o b�d� NULL.
		virtual QWidget* getWidget();
		//! \return Widget kontroluj�cy zachowanie us�ugi/us�ug zale�nych.
		virtual QWidget* getControlWidget();
		//! \return Widget dostarczaj�cy opcji zwi�zanych z us�ug�/us�ugami zale�nymi.
		virtual QWidget* getSettingsWidget();
		//! \param offeredTypes Typy oferowane przez to �r�d�o
		virtual void getOfferedTypes(utils::TypeInfoList & offeredTypes) const;

		//-----------------------------------------------------------------

		//! \param storage Miejsce sk��dowania danych
		//! \param user Nazwa u�ytkownika
		//! \param password	Has�o u�ytkownika
		//! \param session Sesja us�ug zdalnych
		//! \return Kontekst �r�d�a dancyh
		virtual const IHMDBSourceContextPtr createSourceContext(IHMDBStoragePtr storage,
			const std::string & user, const std::string & password,
			IHMDBSessionPtr session = IHMDBSessionPtr());

		//! \param sourceContext Kontekst �r�d�a danych
		//! \return Konteks p�ytkiej kopii bazy danych
		virtual const IHMDBShallowCopyContextPtr createShallowCopyContext(IHMDBSourceContextPtr sourceContext);

		//! \param url Adres serwisu gdzie zak�adamy konto
		//! \param login Nazwa u�ytkownika dla kt�rego chcemy za�o�yc konto
		//! \param password Has�o nowego konta
		//! \param name Imi� nowego u�ytkownika
		//! \param surname Nazwisko nowego u�ytkownika
		//! \param email Email nowego u�ytkownika
		virtual void registerAccount(const std::string & url,
			const std::string & login, const std::string & password,
			const std::string & name, const std::string & surname,
			const std::string & email, const bool multi = true,
			const std::string & caPath = std::string());


		//! \param url Adres serwisu gdzie aktywujemy konto
		//! \param login Nazwa u�ytkownika dla kt�rego chcemy przeprowadzi� aktywacj�
		//! \param activationCode Kod aktywacyjny
		virtual void activateAccount(const std::string & url,
			const std::string & login, const std::string & activationCode,
			const bool multi = true,
			const std::string & caPath = std::string());

		//! \param url Adres serwisu gdzie aktywujemy konto
		//! \param email Adres email zwi�zany z kontem na kt�ry wys�ane zostan� dalsze instrukcje
		virtual void resetPassword(const std::string & url, const std::string & email,
			const bool multi = true,
			const std::string & caPath = std::string());

		//! \return Manager widowkow
		virtual IHMDBSourceViewManager * viewManager();
		//! \return Manager widowkow
		virtual const IHMDBSourceViewManager * viewManager() const;

		/*
		//! \return Ilo�� obs�ugiwanych kontekst�w danych
		virtual const unsigned int size() const;
		//! \param idx Indeks kontekstu danych
		//! \return Konteks danych dla zadanego indeksu
		virtual IHMDBSourceContext * sourceContext(const unsigned int idx);
		//! \param idx Indeks kontekstu danych
		//! \return Konteks danych dla zadanego indeksu
		virtual const IHMDBSourceContext * sourceContext(const unsigned int idx) const;
		*/
		//! \param data Dane
		//! \return Konteks danych lub nullptr je�li nie znaleziono
		//virtual IHMDBSourceContext * sourceContextForData(core::VariantConstPtr data);
		//! \param data Dane
		//! \return Konteks danych lub nullptr je�li nie znaleziono
		//virtual const IHMDBSourceContext * sourceContextForData(core::VariantConstPtr data) const;

	private:
		//! Widget
		HMDBSourceWidget * mainWidget;
		//! Obiekt synchronizuj�cy
		mutable threadingUtils::RecursiveSyncPolicy sync_;
		//! Lista kontekst�w danych
		//std::list<IHMDBSourceContext*> sourceContexts_;
		//! Manager pami�ci
		core::IMemoryDataManager * memoryDM;
		//! Manager strumieni
		core::IStreamDataManager * streamDM;
		//! Manager danych
		core::IFileDataManager * fileDM;
		//! Manager ustawie� widok�w
		utils::shared_ptr<IHMDBSourceViewManager> viewManager_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCE_H__