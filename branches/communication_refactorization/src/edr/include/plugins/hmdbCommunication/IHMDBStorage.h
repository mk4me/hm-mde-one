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
#include <utils/ITTransaction.h>

namespace hmdbCommunication
{

	class IHMDBStorageOperations
	{
	public:
		//! Klucze
		typedef std::set<std::string> Keys;
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBStorageOperations() {}
		//! \param key
		//! \return Czy klucz wyst�puje w storage
		virtual const bool exists(const std::string & key) const = 0;
		//! \param key Klucz o kt�ry pytamy
		//! \return Wska�nik do strumienia pozwalaj�cego czyta� i zapisywa� dane spod klucza lub nullptr jesli klucza nie ma
		virtual std::iostream * get(const std::string & key) = 0;
		//! \param key Klucz o kt�ry pytamy
		//! \return Wska�nik do strumienia pozwalaj�cego czyta� dane spod klucza lub nullptr jesli klucza nie ma
		virtual std::istream * get(const std::string & key) const = 0;
		//! Metoda nadpisuje dane je�li ju� wyst�puj�
		//! \param key Klucz pod kt�rym zapisujemy warto��
		//! \param input Dane do zapisania
		//! \return Czy uda�o si� zapisa� poprawnie dane
		virtual const bool set(const std::string & key, std::istream * input) = 0;
		//! \param key Klucz kt�ry usuwam
		virtual void remove(const std::string & key) = 0;
		//! \param oldKey Klucz kt�remu zmieniamy nazw�
		//! \param newKey Nowa nazwa klucza (unikalna)
		//! \param overwrite Czy nadpisa� je�li istnieje ju� taki klucz
		//! \return Czy operacja si� powiod�a
		virtual const bool rename(const std::string & oldKey,
			const std::string & newKey, const bool overwrite = false) = 0;
		//! \return Lista kluczy w storage
		virtual const Keys keys() const = 0;
	};

	class IHMDBStorage : public utils::ITTransaction<IHMDBStorageOperations>
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
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSTORAGE_H__