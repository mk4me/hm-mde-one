/********************************************************************
	created:  2014/06/02	13:28:35
	filename: HMDBSource.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_medusaDatabase__HMDBSOURCE_H__
#define __HEADER_GUARD_medusaDatabase__HMDBSOURCE_H__

#include <corelib/ISource.h>
#include <plugins/medusaDatabase/IHMDBSource.h>
#include <mutex>

class HMDBSourceWidget;

namespace medusaDatabase
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
		virtual void init(core::IDataManager * memoryDM,
			core::IStreamDataManager * streamDM,
			core::IFileDataManager * fileDM,
			core::IDataHierarchyManager * hierarchyDM);

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

		//! \param data Dane dla kt�rych szukamy kontekstu �r�d�a
		//! \return Konteks �r�d�a dla danych
		virtual const IHMDBSourceContextPtr sourceContextForData(const core::VariantConstPtr data) const;
		//! \param data Dane dla kt�rych szukamy kontekstu p�ytkiej kopii
		//! \return Konteks p�ytkiej kopii dla danych
		virtual const IHMDBShallowCopyContextPtr shallowContextForData(const core::VariantConstPtr data) const;
		//! \param data Dane dla kt�rych szukamy kontekstu �r�d�a
		//! \return Konteks �r�d�a dla danych
		virtual const IHMDBSourceContextPtr sourceContextForData(const void * data) const;
		//! \param data Dane dla kt�rych szukamy kontekstu p�ytkiej kopii
		//! \return Konteks p�ytkiej kopii dla danych
		virtual const IHMDBShallowCopyContextPtr shallowContextForData(const void * data) const;

		//! \param sourceContext Konteks �r�d�a dla danych		
		virtual void registerSourceContext(const IHMDBSourceContextPtr sourceContext);
		//! \param shallowContext Konteks p�ytkiej kopii dla danych		
		virtual void registerShallowContext(const IHMDBShallowCopyContextPtr shallowContext);
		//! \return Ilo�� zarejestrowanych kontekst�w p�ytkiej kopii bazy danych
		virtual unsigned int shallowContextsCount() const;
		//! \param idx Indeks kontekstu
		//! \return Kontekst p�ytkiej kopii lub nullptr je�eli nie ma
		virtual const IHMDBShallowCopyContextPtr shallowContext(const unsigned int idx);

	private:
		//! Widget
		HMDBSourceWidget * mainWidget;
		//! Obiekt synchronizuj�cy
		mutable std::recursive_mutex sync_;
		//! Lista kontekst�w p�ytkiej kopii
		mutable std::list<IHMDBSourceContextWeakPtr> sourceContexts_;
		//! Lista kontekst�w p�ytkiej kopii
		mutable std::list<IHMDBShallowCopyContextWeakPtr> shallowContexts_;

		//! Lista kontekst�w danych
		//std::list<IHMDBSourceContext*> sourceContexts_;
		//! Manager pami�ci
		core::IDataManager * memoryDM;
		//! Manager strumieni
		core::IStreamDataManager * streamDM;
		//! Manager danych
		core::IFileDataManager * fileDM;

		core::IDataHierarchyManager * hierarchyDM;
		//! Manager ustawie� widok�w
		utils::shared_ptr<IHMDBSourceViewManager> viewManager_;
	};
}

#endif	// __HEADER_GUARD_medusaDatabase__HMDBSOURCE_H__
