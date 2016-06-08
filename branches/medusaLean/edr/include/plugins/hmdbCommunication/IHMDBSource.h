/********************************************************************
	created:  2014/05/22	14:16:35
	filename: IHMDBSource.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_hmdbCommunication__IHMDBSOURCE_H__
#define __HEADER_GUARD_hmdbCommunication__IHMDBSOURCE_H__

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
	class IHMDBSourceViewManager;


	class IHMDBSource
	{
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

		//! \return Manager widowkow
		virtual IHMDBSourceViewManager * viewManager() = 0;
		//! \return Manager widowkow
		virtual const IHMDBSourceViewManager * viewManager() const = 0;

		//! \param data Dane dla kt�rych szukamy kontekstu �r�d�a
		//! \return Konteks �r�d�a dla danych
		virtual const IHMDBSourceContextPtr sourceContextForData(const core::VariantConstPtr data) const = 0;
		//! \param data Dane dla kt�rych szukamy kontekstu p�ytkiej kopii
		//! \return Konteks p�ytkiej kopii dla danych
		virtual const IHMDBShallowCopyContextPtr shallowContextForData(const core::VariantConstPtr data) const = 0;
		//! \param data Dane dla kt�rych szukamy kontekstu �r�d�a
		//! \return Konteks �r�d�a dla danych
		virtual const IHMDBSourceContextPtr sourceContextForData(const void * data) const = 0;
		//! \param data Dane dla kt�rych szukamy kontekstu p�ytkiej kopii
		//! \return Konteks p�ytkiej kopii dla danych
		virtual const IHMDBShallowCopyContextPtr shallowContextForData(const void * data) const = 0;

		//! \param sourceContext Konteks �r�d�a dla danych
		virtual void registerSourceContext(const IHMDBSourceContextPtr sourceContext) = 0;
		//! \param shallowContext Konteks p�ytkiej kopii dla danych		
		virtual void registerShallowContext(const IHMDBShallowCopyContextPtr shallowContext) = 0;
		//! \return Ilo�� zarejestrowanych kontekst�w p�ytkiej kopii bazy danych
		virtual unsigned int shallowContextsCount() const = 0;
		//! \param idx Indeks kontekstu
		//! \return Kontekst p�ytkiej kopii lub nullptr je�eli nie ma
		virtual const IHMDBShallowCopyContextPtr shallowContext(const unsigned int idx) = 0;
	};
}

#endif	// __HEADER_GUARD_hmdbCommunication__IHMDBSOURCE_H__
