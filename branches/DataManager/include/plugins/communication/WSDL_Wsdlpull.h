/**
@author Marek Daniluk
@brief Klasa WSDL_Wsdlpull implementuje technologi� WSDL przy pomocy biblioteki wsdlpull.
*/

#ifndef _WSDL_WSDLPULL_H_
#define _WSDL_WSDLPULL_H_

#include "IAuthenticable.h"

namespace communication 
{

	class WSDL_Wsdlpull : public IAuthenticable 
	{
	protected:
		/**
		Pole klasy przechowuj�ce nazw� u�ytkownika.
		*/
		std::string usr;
		/**
		Pole klasy przechowuj�ce has�o u�ytkownika.
		*/
		std::string pswd;
		/**
		Pole klasy przechowuj�ce URI do definicji operacji.
		*/
		std::string uri;
		/**
		Obiekt klasy WsdlInvoker biblioteki wsdlpull
		*/
		WsdlPull::WsdlInvoker invoker;
		/**
		Metoda konfiguruj�ca obiekt invoker przed wykonaniem operacji web serwisowych.
		*/
		void initializeInvoker();
	public:
		/**
		Konstruktor klasy WSDL_Wsdlpull.
		*/
		WSDL_Wsdlpull();
		/**
		Konstruktor klasy WSDL_Wsdlpull.
		@param uri URI do dokumentu definicji operacji sieciowych
		@param usr nazwa u�ytkownika
		@param pswd has�o u�ytkownika
		*/
		WSDL_Wsdlpull(const std::string& uri, const std::string& usr, const std::string& pswd);
		/**
		Wirtualny destruktor klasy WSDL_Wsdlpull.
		*/
		virtual ~WSDL_Wsdlpull();
		/**
		Metoda setUri ustala URI do dokumentu definicji operacji sieciowych
		@param uri URI do definicji operacji sieciowych
		*/
		virtual void setUri(const std::string& uri);
		/**
		Metoda setUser ustala nazw� u�ytkownika wykorzystywana do logowania.
		@param usr nazwa u�ytkownika
		*/
		virtual void setUser(const std::string& usr);
		/**
		Metoda setPassword ustala has�o u�ytkownika wykorzystywane do logowania.
		@param pswd has�o u�ytkownika 
		*/
		virtual void setPassword(const std::string& pswd);
		/**
		Metoda setCredentials ustala dane u�ytkownika wykorzystywane do logowania takie jak host,
		nazwa i has�o u�ytkownika.
		@param uri adres zasobu
		@param usr nazwa u�ytkownika
		@param pswd has�o u�ytkownika
		*/
		virtual void setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd);
		/**
		Metoda zwraca nazw� hosta lub adres ip (w zaleznosci co zostalo podane) uzywana dla danego polaczenia.
		@return URI do definicji operacji sieciowych
		*/
		virtual const std::string& getUri() const;
		/**
		Metoda zwraca login u�ytkownika u�ywany dla danego po��czenia.
		@return login u�ytkownika
		*/
		virtual const std::string& getUser() const;
		/**
		Metoda zwraca has�o u�ytkownika u�ywane dla danego po��czenia.
		@return has�o u�ytkownika
		*/
		virtual const std::string& getPassword() const;
		/**
		Metoda ustala operacje web serwisow� jaka ma zosta� wykonana.
		@param name nazwa operacji kt�ra ma zosta� wykonana
		*/
		void setOperation(const std::string& name);
		/**
		Metoda podaje warto�� parametru operacji.
		@param param nazwa parametru operacji
		@param value warto�� parametru operacji
		*/
		void setValue(const std::string& param, const std::string& value);
		/**
		Metoda wywo�uje operacje web serwisow�.
		*/
		void invokeOperation();
		/**
		Metoda zwraca odpowied� serwera w formie XML.
		@return odpowied� serwera na operacje w formie XML
		*/
		std::string getXMLResponse();
		/**
		Metoda zamieniaj�ca liczb� na string
		@param value liczba do zamiany na string
		@return warto�� value zapisana jako string
		*/
		template <typename T> static std::string toString(T& value) 
		{
			std::ostringstream sstream;
			sstream << value;
			return sstream.str();
		}

		//tymczasowe i pomocnicze
	};
}
#endif
