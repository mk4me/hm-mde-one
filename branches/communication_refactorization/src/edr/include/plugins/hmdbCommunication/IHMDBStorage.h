/********************************************************************
	created:  2014/06/03	13:37:35
	filename: IHMDBStorage.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSTORAGE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSTORAGE_H__

#include <string>
#include <set>
#include <threadingUtils/ITTransaction.h>
#include <corelib/Filesystem.h>
#include <networkUtils/CURLFTPHelper.h>

namespace hmdbCommunication
{

	class IHMDBStorageProgress : public networkUtils::ICURLFTPBasicProgress
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBStorageProgress() {}
		//! \param error Opis b��du
		virtual void setError(const std::string & error) = 0;
	};

	class IHMDBStorageOperations
	{
	public:
		//! Klucze
		typedef std::set<std::string> Keys;
		//!
		typedef utils::shared_ptr<std::istream> IStreamPtr;
		//!
		typedef utils::shared_ptr<std::iostream> IOStreamPtr;
		//!
		typedef utils::shared_ptr<std::ostream> OStreamPtr;
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBStorageOperations() {}
		//! \param key
		//! \return Czy klucz wyst�puje w storage
		virtual const bool exists(const std::string & key) const = 0;
		//! \param key Klucz o kt�ry pytamy
		//! \return Wska�nik do strumienia pozwalaj�cego czyta� i zapisywa� dane spod klucza lub nullptr jesli klucza nie ma
		virtual const IOStreamPtr get(const std::string & key) = 0;
		//! \param key Klucz o kt�ry pytamy
		//! \return Wska�nik do strumienia pozwalaj�cego czyta� dane spod klucza lub nullptr jesli klucza nie ma
		virtual const IStreamPtr get(const std::string & key) const = 0;
		//! Metoda nadpisuje dane je�li ju� wyst�puj�
		//! \param key Klucz pod kt�rym zapisujemy warto��
		//! \param input Dane do zapisania
		//! \return Czy uda�o si� zapisa� poprawnie dane
		virtual const bool set(const std::string & key, IStreamPtr input) = 0;
		//! Metoda nadpisuje dane je�li ju� wyst�puj�
		//! \param key Klucz pod kt�rym zapisujemy warto��
		//! \param input Dane do zapisania
		//! \param progress Obiekt steruj�cy post�pem zapisu		
		//! \param div Dzielnik dla progresu, jesli zapis cz�ci� wi�kszej transakcji
		virtual void set(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress, const float div = 1.0) = 0;
		//! \param key Klucz kt�ry usuwam
		virtual const bool remove(const std::string & key) = 0;
		//! \param oldKey Klucz kt�remu zmieniamy nazw�
		//! \param newKey Nowa nazwa klucza (unikalna)
		//! \param overwrite Czy nadpisa� je�li istnieje ju� taki klucz
		//! \return Czy operacja si� powiod�a
		virtual const bool rename(const std::string & oldKey,
			const std::string & newKey, const bool overwrite = false) = 0;
		//! \return Lista kluczy w storage
		virtual const Keys keys() const = 0;
		//! \param size Ilo�� bajt�w do zapisu
		//! \return Czy storage b�dzie jeszcze w stanie zapisa� tak� ilo�� danych
		virtual const bool canStore(const unsigned long long size) const = 0;
		//! \param path �cie�ka
		//! \return Czy dana �cie�ka dzieli przestrze� dyskow� z naszym storage
		virtual const bool shareDiskSpace(const core::Filesystem::Path & path) const { return false; }
	};

	class IHMDBStorage : public threadingUtils::ITTransaction<IHMDBStorageOperations>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBStorage() {}
		//! \return Nazwa storage
		virtual const std::string name() const = 0;
		//! \return Nazwa protoko�u (http, file, ftp, ...)
		virtual const std::string protocol() const = 0;

		//! \return Czy sotrage otwarty do dzia�ania
		virtual const bool isOpened() const = 0;
		
		//! Metoda powinna zamyka� storage, zapisuj�c wszystkie jeszcze niezapisane dane
		//! Po tym wywo�aniu isOpened powinno zwraca� false!!
		virtual void close() = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBStorage);
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSTORAGE_H__