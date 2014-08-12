/********************************************************************
	created:  2014/08/10	16:05:08
	filename: MemoryStorage.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__MEMORYSTORAGE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__MEMORYSTORAGE_H__

#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <threadingUtils/SynchronizationPolicies.h>
#include <map>

namespace hmdbCommunication
{
	class MemoryStorage : public IHMDBStorage
	{
	private:

		class MemoryStorageTransaction;
		friend class MemoryStorageTransaction;

		typedef std::map<std::string, std::string> Storage;

	public:
		//! Konstruktor domy�lny
		MemoryStorage();
		//! Destruktor wirtualny
		virtual ~MemoryStorage();

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
		//! Dane
		Storage storage;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__MEMORYSTORAGE_H__