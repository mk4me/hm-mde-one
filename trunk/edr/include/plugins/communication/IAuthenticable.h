/**
@author Marek Daniluk
@brief Interfejs IAuthenticable dodaje funkcjonalnoœæ autentykacji.
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
		Metoda setUser ustala nazwê u¿ytkownika wykorzystywan¹ do logowania.
		@param usr nazwa u¿ytkownika
		*/
		virtual void setUser(const std::string& usr) = 0;
		/**
		Metoda setUser ustala has³o u¿ytkownika wykorzystywane do logowania.
		@param pswd has³o u¿ytkownika
		*/
		virtual void setPassword(const std::string& pswd) = 0;
		/**
		Metoda setCredentials ustala dane u¿ytkownika wykorzystywane do logowania takie jak host,
		nazwa i has³o u¿ytkownika.
		@param uri adres URI
		@param usr nazwa u¿ytkownika
		@param pswd has³o u¿ytkownika
		*/
		virtual void setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd) = 0;
		/**
		Metoda zwraca adres URI.
		@return adres URI
		*/
		virtual const std::string& getUri() const = 0;
		/**
		Metoda zwraca login u¿ytkownika u¿ywana dla danego po³¹czenia.
		@return login u¿ytkownika
		*/
		virtual const std::string& getUser() const = 0;
		/**
		Metoda zwraca has³o u¿ytkownika u¿ywana dla danego po³¹czenia.
		@return has³o u¿ytkownika
		*/
		virtual const std::string& getPassword() const = 0;
	};
}
#endif
