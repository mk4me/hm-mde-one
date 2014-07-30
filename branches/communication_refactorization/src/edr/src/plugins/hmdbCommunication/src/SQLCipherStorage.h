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
		//! Konstruktor domyœlny
		SQLCipherStorage();
		//! Destruktor wirtualny
		virtual ~SQLCipherStorage();
		//! \param path Œcie¿ka do pliku z baz¹
		//! \param key Klucz, którym zaszyfrowana jest baza
		void open(const core::Filesystem::Path & path,
			const std::string & key);

		//! \param path Œcie¿ka do pliku z baz¹
		//! \param key Klucz, którym zaszyfrowana jest baza
		//! \return Czy uda³o siê stworzyæ bazê
		static const bool create(const core::Filesystem::Path & path,
			const std::string & key);

		//! \param path Œcie¿ka do pliku z baz¹
		//! \param oldKey Aktualny klucz, którym zaszyfrowana jest baza
		//! \param newKey Klucz, którym baza zostanie zaszyfrowana
		//! \return Czy uda³o siê zmieniæ klucz
		static const bool changeKey(const core::Filesystem::Path & path,
			const std::string & oldKey,	const std::string & newKey);

		//! \param path Œcie¿ka do pliku z baz¹
		//! \param key Klucz, którym baza zostanie zaszyfrowana
		//! \return Czy uda³o siê ustawiæ klucz
		static const bool setKey(const core::Filesystem::Path & path,
			const std::string & key);

		//! \param path Œcie¿ka do pliku z baz¹
		//! \param key Klucz, którym zaszyfrowana jest baza
		//! \return Czy baza ma poprawny format
		static const bool verify(const core::Filesystem::Path & path,
			const std::string & key);

		//! \return Czy sotrage otwarty do dzia³ania
		virtual const bool isOpened() const;
		//! \param key
		//! \return Czy klucz wystêpuje w storage
		virtual const bool exists(const std::string & key) const;
		//! \param key Klucz o który pytamy
		//! \return WskaŸnik do strumienia pozwalaj¹cego czytaæ i zapisywaæ dane spod klucza lub nullptr jesli klucza nie ma
		virtual std::iostream * get(const std::string & key);
		//! \param key Klucz o który pytamy
		//! \return WskaŸnik do strumienia pozwalaj¹cego czytaæ dane spod klucza lub nullptr jesli klucza nie ma
		virtual std::istream * get(const std::string & key) const;
		//! Metoda nadpisuje dane jeœli ju¿ wystêpuj¹
		//! \param key Klucz pod którym zapisujemy wartoœæ
		//! \param input Dane do zapisania
		//! \return Czy uda³o siê zapisaæ poprawnie dane
		virtual const bool set(const std::string & key, std::istream * input);
		//! \param key Klucz który usuwam
		virtual void remove(const std::string & key);
		//! Metoda powinna zamykaæ storage, zapisuj¹c wszystkie jeszcze niezapisane dane
		//! Po tym wywo³aniu isOpened powinno zwracaæ false!!
		virtual void close();

		//! \param oldKey Klucz któremu zmieniamy nazwê
		//! \param newKey Nowa nazwa klucza (unikalna)
		//! \param overwrite Czy nadpisaæ jeœli istnieje ju¿ taki klucz
		//! \return Czy operacja siê powiod³a
		virtual const bool rename(const std::string & oldKey,
			const std::string & newKey, const bool overwrite = false);
		//! \return Lista kluczy w storage
		virtual const Keys keys() const;
		//! \return Nazwa storage
		virtual const std::string name() const;
		//! \return Nazwa protoko³u (http, file, ftp, ...)
		virtual const std::string protocol() const;

		//! \return Transakcja
		virtual const TransactionPtr transaction();
		//! \return Transakcja
		virtual const TransactionConstPtr transaction() const;

	private:

		//! \return Czy klucz wystêpuje w storage
		const bool rawExists(const std::string & key) const;
		//! \param key Klucz o który pytamy
		//! \return WskaŸnik do strumienia pozwalaj¹cego czytaæ i zapisywaæ dane spod klucza lub nullptr jesli klucza nie ma
		std::iostream * rawGet(const std::string & key);
		//! \param key Klucz o który pytamy
		//! \return WskaŸnik do strumienia pozwalaj¹cego czytaæ dane spod klucza lub nullptr jesli klucza nie ma
		std::istream * rawGet(const std::string & key) const;
		//! Metoda nadpisuje dane jeœli ju¿ wystêpuj¹
		//! \param key Klucz pod którym zapisujemy wartoœæ
		//! \param input Dane do zapisania
		//! \return Czy uda³o siê zapisaæ poprawnie dane
		const bool rawSet(const std::string & key, std::istream * input);
		//! \param key Klucz który usuwam
		void rawRemove(const std::string & key);
		//! \param oldKey Klucz któremu zmieniamy nazwê
		//! \param newKey Nowa nazwa klucza (unikalna)
		//! \param overwrite Czy nadpisaæ jeœli istnieje ju¿ taki klucz
		//! \return Czy operacja siê powiod³a
		const bool rawRename(const std::string & oldKey,
			const std::string & newKey, const bool overwrite = false);
		//! \return Lista kluczy w storage
		const Keys rawKeys() const;

	private:
		//! Obiekt synchronizuj¹cy
		mutable threadingUtils::RecursiveSyncPolicy sync_;
		//! Uchwyt do bazy
		sqlite3 * db;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__SQLCIPHERSTORAGE_H__