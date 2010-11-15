/**
@author Marek Daniluk
@brief Interfejs IFTP implementuje interfejs ISendable. Dostarcza funkcjonalnosci potrzebnej do implementacji
protokolu FTP przy pomocy roznych bibliotek.
*/

#ifndef _IFTP_H_
#define _IFTP_H_

#include <vector>

#include "ISendable.h"

namespace EDRCM {

	class IFTP : public ISendable {
	public:
		/**
		Wirtualny destruktor klasy IFTP.
		*/
		virtual ~IFTP() {}
		/**
		Przejscie do katalogu direcotry na serwerze FTP.
		@param directory katalog do ktorego chcemy przejsc
		*/
		virtual void cd(const std::string& directory) = 0;
		/**
		Zwraca nazwe katalogu w ktorym aktualnie przebywa klient.
		@return nazwa zdalnego katalogu w ktorym znajduje sie klient
		*/
		virtual const std::string& pwd() const = 0;
		/**
		Wylistowanie plikow i katalogow w zdalnym katalogu w ktorym aktualnie przebywa klient.
		@return lista katalogow i plikow ze zdalnego katalogu w ktorym znajduje sie klient
		*/
		virtual std::vector<const std::string&>& ls() const = 0;
		/**
		Tworzy katalog na serwerze w zdalnym katalogu w ktorym aktualnie przebywa klient.
		@param dirname nazwa katalogu do utworzenia
		*/
		virtual void make(const std::string& dirname) = 0;
		/**
		Usuwa katalog lub plik na serwerze w zdalnym katalogu w ktorym aktualnie przebywa klient.
		@param filename nazwa katalogu lub pliku do usuniecia
		*/
		virtual void del(const std::string& filename) = 0;
		/**
		Konczy polaczenie z serwerem FTP.
		*/
		virtual void disconnect() = 0;
	};
}
#endif
