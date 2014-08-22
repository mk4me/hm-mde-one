/********************************************************************
	created:  2014/05/27	17:41:31
	filename: CURLFTPTransfer.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFER_H__

#include <networkUtils/CURLFTPHelper.h>
#include <networkUtils/CURLManager.h>
#include <threadingUtils/SynchronizationPolicies.h>
#include <plugins/hmdbCommunication/IHMDBFtp.h>

namespace hmdbCommunication
{
	struct CURLFTPTransferData;
	typedef utils::shared_ptr<networkUtils::CURLManager> CURLManagerPtr;
	typedef utils::weak_ptr<networkUtils::CURLManager> CURLManagerWPtr;

	class CURLFTPTransfer : public IHMDBFtp::ITransfer
	{
	private:
		
		typedef threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> ScopedLock;

	public:

		//! Konstruktor
		//! \param curl Skonfigurowane po��czenie
		//! \param manager Manager po��cze� curla
		//! \param file Plik kt�rego dotyczy transfer
		//! \param size Rozmiar danych do przetworzenia
		CURLFTPTransfer(const Direction direction, CURL * curl, CURLManagerPtr manager,
			const std::string & file, const size_t size);
		//! Destruktor wirtualny
		virtual ~CURLFTPTransfer();
		//! \return Plik, kt�rego dotyczy transfer - lokalny lub zdalny
		virtual const std::string file() const;
		//! Domy�lna implementacja post�pu
		virtual const float progress() const;
		//! \return Czas [s] jaki up�yn�� od rozpocz�cia zadania
		virtual const float elapsed() const;
		//! \return Szacowany czas [s] jaki zosta� do zako�czenia zadania
		virtual const float remaining() const;
		//! \return Ilo�� danych ju� przetworzona [bytes]
		virtual const unsigned long long processed() const;
		//! \return Ilo�� danych do przetworzenia
		virtual const unsigned long long size() const;
		//! \return �rednia pr�dko�� pobierania bytes/second
		virtual const float averageSpeed() const;
		//! Metoda rozpoczyna operacj�
		virtual void start();
		//! Metoda przerywa operacj�
		virtual void abort();
		//! Metoda czeka do zako�czenia operacji
		virtual void wait();
		//! \return Stan operacji
		virtual const Status status() const;
		//! \return Opis b��du
		virtual const std::string error() const;
		//! \return Typ transferu
		virtual const Direction direction() const;
		
		//! \return Uchwyt do po��czenia
		const CURL * curl() const;
		//! \return Uchwyt do po��czenia
		CURL * curl();

	private:
		//! Plik k�rego dotyczy transfer
		const std::string file_;
		//! Rozmiar danych do przetworzenia
		const size_t size_;
		//! Obiekt synchronizujacy
		mutable threadingUtils::RecursiveSyncPolicy sync_;
		//! Po��czenie curla
		CURL * curl_;
		//! Manager po��cze� curlowych
		CURLManagerWPtr manager_;
		//! Dane transferu
		utils::shared_ptr<CURLFTPTransferData> data_;
		//! Typ transferu
		const Direction direction_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFER_H__