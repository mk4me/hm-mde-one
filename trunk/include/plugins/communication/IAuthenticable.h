/**
@author Marek Daniluk
@brief Interfejs IAuthenticable dodaje funkcjonalnosc autentykacji.
*/

#ifndef _IAUTHENTICABLE_H_
#define _IAUTHENTICABLE_H_

namespace communication {

	class IAuthenticable {
	public:
		/**
		Wirtualny destruktor klasy IAuthenticable.
		*/
		virtual ~IAuthenticable() {}
		/**
		Metoda setUri ustala URI zasobu.
		@param uri adres URI typu string
		*/
		virtual void setUri(const std::string& uri) = 0;
		/**
		Metoda setUser ustala nazwe uzytkownika wykorzystywana do logowania.
		@param usr nazwa uzytkownika typu string
		*/
		virtual void setUser(const std::string& usr) = 0;
		/**
		Metoda setUser ustala haslo uzytkownika wykorzystywane do logowania.
		@param pswd haslo uzytkownika typu string
		*/
		virtual void setPassword(const std::string& pswd) = 0;
		/**
		Metoda setCredentials ustala dane uzytkownika wykorzystywane do logowania takie jak host,
		nazwa i haslo uzytkownika.
		@param uri adres URI typu string
		@param usr nazwa uzytkownika typu string
		@param pswd haslo uzytkownika typu string
		*/
		virtual void setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd) = 0;
		/**
		Metoda zwraca adres URI.
		@return adres URI
		*/
		virtual const std::string& getUri() const = 0;
		/**
		Metoda zwraca login uzytkownika uzywana dla danego polaczenia.
		@return login uzytkownika
		*/
		virtual const std::string& getUser() const = 0;
		/**
		Metoda zwraca haslo uzytkownika uzywana dla danego polaczenia.
		@return haslo uzytkownika
		*/
		virtual const std::string& getPassword() const = 0;
	};
}
#endif
