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
#include <threadingUtils/SynchronizationPolicies.h>
#include <sqlite3.h>

namespace hmdbCommunication
{
	class SQLCipherStorage : public IHMDBStorage
	{
	private:

		class SQLCipherStorageTransaction;
		friend class SQLCipherStorageTransaction;

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

		//! \return Czy sotrage otwarty do dzia�ania
		virtual const bool isOpened() const;
		//! \param key
		//! \return Czy klucz wyst�puje w storage
		virtual const bool exists(const std::string & key) const;
		//! \param key Klucz o kt�ry pytamy
		//! \return Wska�nik do strumienia pozwalaj�cego czyta� i zapisywa� dane spod klucza lub nullptr jesli klucza nie ma
		virtual std::iostream * get(const std::string & key);
		//! \param key Klucz o kt�ry pytamy
		//! \return Wska�nik do strumienia pozwalaj�cego czyta� dane spod klucza lub nullptr jesli klucza nie ma
		virtual std::istream * get(const std::string & key) const;
		//! Metoda nadpisuje dane je�li ju� wyst�puj�
		//! \param key Klucz pod kt�rym zapisujemy warto��
		//! \param input Dane do zapisania
		//! \return Czy uda�o si� zapisa� poprawnie dane
		virtual const bool set(const std::string & key, std::istream * input);
		//! \param key Klucz kt�ry usuwam
		virtual void remove(const std::string & key);
		//! Metoda powinna zamyka� storage, zapisuj�c wszystkie jeszcze niezapisane dane
		//! Po tym wywo�aniu isOpened powinno zwraca� false!!
		virtual void close();

		//! \param oldKey Klucz kt�remu zmieniamy nazw�
		//! \param newKey Nowa nazwa klucza (unikalna)
		//! \param overwrite Czy nadpisa� je�li istnieje ju� taki klucz
		//! \return Czy operacja si� powiod�a
		virtual const bool rename(const std::string & oldKey,
			const std::string & newKey, const bool overwrite = false);
		//! \return Lista kluczy w storage
		virtual const Keys keys() const;
		//! \return Nazwa storage
		virtual const std::string name() const;
		//! \return Nazwa protoko�u (http, file, ftp, ...)
		virtual const std::string protocol() const;

		//! \return Transakcja
		virtual const TransactionPtr transaction();
		//! \return Transakcja
		virtual const TransactionConstPtr transaction() const;

	private:

		//! \return Czy klucz wyst�puje w storage
		const bool rawExists(const std::string & key) const;
		//! \param key Klucz o kt�ry pytamy
		//! \return Wska�nik do strumienia pozwalaj�cego czyta� i zapisywa� dane spod klucza lub nullptr jesli klucza nie ma
		std::iostream * rawGet(const std::string & key);
		//! \param key Klucz o kt�ry pytamy
		//! \return Wska�nik do strumienia pozwalaj�cego czyta� dane spod klucza lub nullptr jesli klucza nie ma
		std::istream * rawGet(const std::string & key) const;
		//! Metoda nadpisuje dane je�li ju� wyst�puj�
		//! \param key Klucz pod kt�rym zapisujemy warto��
		//! \param input Dane do zapisania
		//! \return Czy uda�o si� zapisa� poprawnie dane
		const bool rawSet(const std::string & key, std::istream * input);
		//! \param key Klucz kt�ry usuwam
		void rawRemove(const std::string & key);
		//! \param oldKey Klucz kt�remu zmieniamy nazw�
		//! \param newKey Nowa nazwa klucza (unikalna)
		//! \param overwrite Czy nadpisa� je�li istnieje ju� taki klucz
		//! \return Czy operacja si� powiod�a
		const bool rawRename(const std::string & oldKey,
			const std::string & newKey, const bool overwrite = false);
		//! \return Lista kluczy w storage
		const Keys rawKeys() const;

	private:
		//! Obiekt synchronizuj�cy
		mutable threadingUtils::RecursiveSyncPolicy sync_;
		//! Uchwyt do bazy
		sqlite3 * db;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__SQLCIPHERSTORAGE_H__