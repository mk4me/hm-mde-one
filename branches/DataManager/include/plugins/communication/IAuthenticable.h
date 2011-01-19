/**
@author Marek Daniluk
@brief Interfejs IAuthenticable dodaje funkcjonalność autentykacji.
*/

#ifndef _IAUTHENTICABLE_H_
#define _IAUTHENTICABLE_H_

namespace communication
{

	class IAuthenticable
	{
	public:
		/**
		Wirtualny destruktor klasy IAuthenticable.
		*/
		virtual ~IAuthenticable() {}
		/**
		Metoda setUri ustala URI zasobu.
		@param uri adres URI
		*/
		virtual void setUri(const std::string& uri) = 0;
		/**
		Metoda setUser ustala nazwę użytkownika wykorzystywaną do logowania.
		@param usr nazwa użytkownika
		*/
		virtual void setUser(const std::string& usr) = 0;
		/**
		Metoda setUser ustala hasło użytkownika wykorzystywane do logowania.
		@param pswd hasło użytkownika
		*/
		virtual void setPassword(const std::string& pswd) = 0;
		/**
		Metoda setCredentials ustala dane użytkownika wykorzystywane do logowania takie jak host,
		nazwa i hasło użytkownika.
		@param uri adres URI
		@param usr nazwa użytkownika
		@param pswd hasło użytkownika
		*/
		virtual void setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd) = 0;
		/**
		Metoda zwraca adres URI.
		@return adres URI
		*/
		virtual const std::string& getUri() const = 0;
		/**
		Metoda zwraca login użytkownika używana dla danego połączenia.
		@return login użytkownika
		*/
		virtual const std::string& getUser() const = 0;
		/**
		Metoda zwraca hasło użytkownika używana dla danego połączenia.
		@return hasło użytkownika
		*/
		virtual const std::string& getPassword() const = 0;
	};
}
#endif
