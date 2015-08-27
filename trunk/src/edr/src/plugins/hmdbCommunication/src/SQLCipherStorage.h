/********************************************************************
	created:  2014/06/03	13:48:46
	filename: SQLCipherStorage.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__SQLCIPHERSTORAGE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__SQLCIPHERSTORAGE_H__

#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <corelib/Filesystem.h>
#include <mutex>
#include <sqlite3.h>
#include <boost/thread/shared_mutex.hpp>

namespace hmdbCommunication
{
	class SQLCipherStorage : public IHMDBStorage
	{
	public:

		struct SharedState
		{
			//! Klucz bazy
			std::string key_;
			//! �cie�ka do pliku z baz�
			core::Filesystem::Path path_;
		};

		typedef threadingUtils::TransactionSharedState<SharedState> TransactionSharedState;
		typedef threadingUtils::TransactionImplHelper<IHMDBStorage::transaction_type, TransactionSharedState>::SharedStatePtr SharedStatePtr;

	public:
		//! Konstruktor domy�lny
		SQLCipherStorage();
		//! Destruktor wirtualny
		virtual ~SQLCipherStorage();
		//! \param path �cie�ka do pliku z baz�
		//! \param key Klucz, kt�rym zaszyfrowana jest baza
		void open(const core::Filesystem::Path & path,
			const std::string & key);

		//! \param path �cie�ka do pliku z baz�
		//! \param key Klucz, kt�rym zaszyfrowana jest baza
		//! \return Czy uda�o si� stworzy� baz�
		static const bool create(const core::Filesystem::Path & path,
			const std::string & key);

		//! \param path �cie�ka do pliku z baz�
		//! \param oldKey Aktualny klucz, kt�rym zaszyfrowana jest baza
		//! \param newKey Klucz, kt�rym baza zostanie zaszyfrowana
		//! \return Czy uda�o si� zmieni� klucz
		static const bool changeKey(const core::Filesystem::Path & path,
			const std::string & oldKey,	const std::string & newKey);

		//! \param path �cie�ka do pliku z baz�
		//! \param key Klucz, kt�rym baza zostanie zaszyfrowana
		//! \return Czy uda�o si� ustawi� klucz
		static const bool setKey(const core::Filesystem::Path & path,
			const std::string & key);

		//! \param path �cie�ka do pliku z baz�
		//! \param key Klucz, kt�rym zaszyfrowana jest baza
		//! \return Czy baza ma poprawny format
		static const bool verify(const core::Filesystem::Path & path,
			const std::string & key);

		//! \return Nazwa storage
		virtual const std::string name() const override;
		//! \return Nazwa protoko�u (http, file, ftp, ...)
		virtual const std::string protocol() const override;
		//! \return Czy sotrage otwarty do dzia�ania
		virtual const bool isOpened() const override;
		//! Metoda powinna zamyka� storage, zapisuj�c wszystkie jeszcze niezapisane dane
		//! Po tym wywo�aniu isOpened powinno zwraca� false!!
		virtual void close() override;
		//! \return Transakcja
		virtual TransactionPtr transaction() override;
		//! \return Transakcja
		virtual TransactionConstPtr transaction() const override;

	private:
		//! Wsp�ldzielony stan transakcji
		SharedStatePtr sharedState;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__SQLCIPHERSTORAGE_H__