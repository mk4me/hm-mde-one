/********************************************************************
	created:  2014/05/27	17:41:31
	filename: CURLFTPTransfer.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFER_H__

#include <mutex>
#include <plugins/hmdbCommunication/IHMDBFtp.h>
#include "CURLFTPManager.h"

namespace hmdbCommunication
{
	struct CURLFTPTransferData;
	typedef utils::shared_ptr<CURLFTPManager> CURLFTPManagerPtr;
	typedef utils::weak_ptr<CURLFTPManager> CURLFTPManagerWPtr;

	class CURLFTPTransfer : public IHMDBFtp::ITransfer
	{
	private:
		
		typedef std::lock_guard<std::mutex> ScopedLock;

	public:

		//! Konstruktor
		//! \param curl Skonfigurowane po��czenie
		//! \param manager Manager po��cze� curla
		//! \param data Post�p i dane
		//! \param file Plik kt�rego dotyczy transfer
		//! \param size Rozmiar danych do przetworzenia
		CURLFTPTransfer(const Direction direction, CURLFTPManagerPtr manager,
			utils::shared_ptr<CURLFTPTransferData> data,
			const std::string & file, const size_t size);
		//! Destruktor wirtualny
		virtual ~CURLFTPTransfer();
		//! \return Plik, kt�rego dotyczy transfer - lokalny lub zdalny
		virtual std::string file() const;
		//! Domy�lna implementacja post�pu
		virtual float normalizedProgress() const;
		//! \return Czas [s] jaki up�yn�� od rozpocz�cia zadania
		virtual float elapsed() const;
		//! \return Szacowany czas [s] jaki zosta� do zako�czenia zadania
		virtual float remaining() const;
		//! \return Ilo�� danych ju� przetworzona [bytes]
		virtual std::size_t processed() const;
		//! \return Ilo�� danych do przetworzenia
		virtual std::size_t size() const;
		//! \return �rednia pr�dko�� pobierania bytes/second
		virtual float averageSpeed() const;
		//! Metoda rozpoczyna operacj�
		virtual void start();
		//! Metoda przerywa operacj�
		virtual void abort();
		//! Metoda czeka do zako�czenia operacji
		virtual void wait();
		//! \return Stan operacji
		virtual Status status() const;
		//! \return Opis b��du
		virtual std::string error() const;
		//! \return Typ transferu
		virtual Direction direction() const;

	private:
		//! Plik k�rego dotyczy transfer
		const std::string file_;
		//! Rozmiar danych do przetworzenia
		const size_t size_;
		//! Obiekt synchronizujacy
		mutable std::mutex sync_;
		//! Manager po��cze� curlowych
		CURLFTPManagerWPtr manager_;
		//! Dane transferu
		utils::shared_ptr<CURLFTPTransferData> data_;
		//! Typ transferu
		const Direction direction_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFER_H__