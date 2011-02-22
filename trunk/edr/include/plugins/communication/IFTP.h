/**
@author Marek Daniluk
@brief Interfejs IFTP implementuje interfejs ISendable. Dostarcza funkcjonalno�ci potrzebnej do implementacji
protoko�u FTP.
*/

#ifndef HEADER_GUARD_COMMUNICATION_IFTP_H__
#define HEADER_GUARD_COMMUNICATION_IFTP_H__

#include "ISendable.h"

namespace communication
{

	class IFTP : public ISendable
	{
	public:
		/**
		Wirtualny destruktor klasy IFTP.
		*/
		virtual ~IFTP() {}
		/**
		Przej�cie do katalogu direcotry na serwerze FTP.
		@param directory katalog do kt�rego chcemy przej��
		*/
		virtual void cd(const std::string& directory) = 0;
		/**
		Zwraca nazwe katalogu w kt�rym aktualnie przebywa klient.
		@return nazwa zdalnego katalogu w kt�rym znajduje si� klient
		*/
		virtual const std::string& pwd() const = 0;
		/**
		Wylistowanie plik�w i katalog�w w zdalnym katalogu w kt�rym aktualnie przebywa klient.
		@return lista katalog�w i plik�w ze zdalnego katalogu w kt�rym znajduje si� klient
		*/
		virtual std::vector<const std::string&>& ls() const = 0;
		/**
		Tworzy katalog na serwerze w zdalnym katalogu w kt�rym aktualnie przebywa klient.
		@param dirname nazwa katalogu do utworzenia
		*/
		virtual void make(const std::string& dirname) = 0;
		/**
		Usuwa katalog lub plik na serwerze w zdalnym katalogu w kt�rym aktualnie przebywa klient.
		@param filename nazwa katalogu lub pliku do usuni�cia
		*/
		virtual void del(const std::string& filename) = 0;
		/**
		Konczy po��czenie z serwerem FTP.
		*/
		virtual void disconnect() = 0;
		/**
		Post�p operacji przesy�ania.
		@return post�p wyra�ony w procentach
		*/
		virtual int getProgress() const = 0;
		/**
		Anuluje obecnie wykonywan� operacj�.
		*/
		virtual void abort() = 0;
	};
}
#endif
