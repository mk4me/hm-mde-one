/********************************************************************
	created:  2014/08/10	16:05:08
	filename: MemoryStorage.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__MEMORYSTORAGE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__MEMORYSTORAGE_H__

#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <mutex>
#include <map>

namespace hmdbCommunication
{
	class MemoryStorage : public IHMDBStorage
	{
	private:

		class MemoryStorageTransaction;
		friend class MemoryStorageTransaction;

		typedef std::map<std::string, std::string> Storage;

		enum {
			MaxBufferSize = 1 << 20
		};

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
		virtual const IOStreamPtr get(const std::string & key);
		//! \param key Klucz o kt�ry pytamy
		//! \return Wska�nik do strumienia pozwalaj�cego czyta� dane spod klucza lub nullptr jesli klucza nie ma
		virtual const IStreamPtr get(const std::string & key) const;
		//! Metoda nadpisuje dane je�li ju� wyst�puj�
		//! \param key Klucz pod kt�rym zapisujemy warto��
		//! \param input Dane do zapisania
		//! \return Czy uda�o si� zapisa� poprawnie dane
		virtual const bool set(const std::string & key, IStreamPtr input);
		//! Metoda nadpisuje dane je�li ju� wyst�puj�
		//! \param key Klucz pod kt�rym zapisujemy warto��
		//! \param input Dane do zapisania
		//! \param progress Obiekt steruj�cy post�pem zapisu
		virtual void set(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress);
		//! \param key Klucz kt�ry usuwam
		virtual const bool remove(const std::string & key);
		//! Metoda powinna zamyka� storage, zapisuj�c wszystkie jeszcze niezapisane dane
		//! Po tym wywo�aniu isOpened powinno zwraca� false!!
		virtual void close();
		//! \param size Ilo�� bajt�w do zapisu
		//! \return Czy storage b�dzie jeszcze w stanie zapisa� tak� ilo�� danych
		virtual const bool canStore(const unsigned long long size) const;

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
		const IOStreamPtr rawGet(const std::string & key);
		//! \param key Klucz o kt�ry pytamy
		//! \return Wska�nik do strumienia pozwalaj�cego czyta� dane spod klucza lub nullptr jesli klucza nie ma
		const IStreamPtr rawGet(const std::string & key) const;
		//! Metoda nadpisuje dane je�li ju� wyst�puj�
		//! \param key Klucz pod kt�rym zapisujemy warto��
		//! \param input Dane do zapisania
		//! \return Czy uda�o si� zapisa� poprawnie dane
		const bool rawSet(const std::string & key, IStreamPtr input);
		//! Metoda nadpisuje dane je�li ju� wyst�puj�
		//! \param key Klucz pod kt�rym zapisujemy warto��
		//! \param input Dane do zapisania
		//! \param progress Obiekt steruj�cy post�pem zapisu		
		void rawSet(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress);
		//! \param key Klucz kt�ry usuwam
		const bool rawRemove(const std::string & key);
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
		mutable std::recursive_mutex sync_;
		//! Dane
		Storage storage;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__MEMORYSTORAGE_H__