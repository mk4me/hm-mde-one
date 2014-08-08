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
		//! \param curl Skonfigurowane po³¹czenie
		//! \param manager Manager po³¹czeñ curla
		//! \param file Plik którego dotyczy transfer
		//! \param size Rozmiar danych do przetworzenia
		CURLFTPTransfer(const Direction direction, CURL * curl, CURLManagerPtr manager,
			const std::string & file, const size_t size);
		//! Destruktor wirtualny
		virtual ~CURLFTPTransfer();
		//! \return Plik, którego dotyczy transfer - lokalny lub zdalny
		virtual const std::string file() const;
		//! Domyœlna implementacja postêpu
		virtual const float progress() const;
		//! \return Czas [s] jaki up³yn¹³ od rozpoczêcia zadania
		virtual const float elapsed() const;
		//! \return Szacowany czas [s] jaki zosta³ do zakoñczenia zadania
		virtual const float remaining() const;
		//! \return Iloœæ danych ju¿ przetworzona [bytes]
		virtual const unsigned long long processed() const;
		//! \return Iloœæ danych do przetworzenia
		virtual const unsigned long long size() const;
		//! \return Œrednia prêdkoœæ pobierania bytes/second
		virtual const float averageSpeed() const;
		//! Metoda rozpoczyna operacjê
		virtual void start();
		//! Metoda przerywa operacjê
		virtual void abort();
		//! Metoda czeka do zakoñczenia operacji
		virtual void wait();
		//! \return Stan operacji
		virtual const Status status() const;
		//! \return Opis b³êdu
		virtual const std::string error() const;
		//! \return Typ transferu
		virtual const Direction direction() const;
		
		//! \return Uchwyt do po³¹czenia
		const CURL * curl() const;
		//! \return Uchwyt do po³¹czenia
		CURL * curl();

	private:
		//! Plik kórego dotyczy transfer
		const std::string file_;
		//! Rozmiar danych do przetworzenia
		const size_t size_;
		//! Obiekt synchronizujacy
		mutable threadingUtils::RecursiveSyncPolicy sync_;
		//! Po³¹czenie curla
		CURL * curl_;
		//! Manager po³¹czeñ curlowych
		CURLManagerWPtr manager_;
		//! Dane transferu
		utils::shared_ptr<CURLFTPTransferData> data_;
		//! Typ transferu
		const Direction direction_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFER_H__