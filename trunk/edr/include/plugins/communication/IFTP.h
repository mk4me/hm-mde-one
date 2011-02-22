/**
@author Marek Daniluk
@brief Interfejs IFTP implementuje interfejs ISendable. Dostarcza funkcjonalnoœci potrzebnej do implementacji
protoko³u FTP.
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
		Przejœcie do katalogu direcotry na serwerze FTP.
		@param directory katalog do którego chcemy przejœæ
		*/
		virtual void cd(const std::string& directory) = 0;
		/**
		Zwraca nazwe katalogu w którym aktualnie przebywa klient.
		@return nazwa zdalnego katalogu w którym znajduje siê klient
		*/
		virtual const std::string& pwd() const = 0;
		/**
		Wylistowanie plików i katalogów w zdalnym katalogu w którym aktualnie przebywa klient.
		@return lista katalogów i plików ze zdalnego katalogu w którym znajduje siê klient
		*/
		virtual std::vector<const std::string&>& ls() const = 0;
		/**
		Tworzy katalog na serwerze w zdalnym katalogu w którym aktualnie przebywa klient.
		@param dirname nazwa katalogu do utworzenia
		*/
		virtual void make(const std::string& dirname) = 0;
		/**
		Usuwa katalog lub plik na serwerze w zdalnym katalogu w którym aktualnie przebywa klient.
		@param filename nazwa katalogu lub pliku do usuniêcia
		*/
		virtual void del(const std::string& filename) = 0;
		/**
		Konczy po³¹czenie z serwerem FTP.
		*/
		virtual void disconnect() = 0;
		/**
		Postêp operacji przesy³ania.
		@return postêp wyra¿ony w procentach
		*/
		virtual int getProgress() const = 0;
		/**
		Anuluje obecnie wykonywan¹ operacjê.
		*/
		virtual void abort() = 0;
	};
}
#endif
