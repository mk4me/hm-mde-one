/**
@author Marek Daniluk
@brief Klasa WSDL_Wsdlpull implementuje technologie WSDL przy pomocy biblioteki wsdlpull.
*/

#ifndef _WSDL_WSDLPULL_H_
#define _WSDL_WSDLPULL_H_

#include "IAuthenticable.h"
#include "EDRException.h"

namespace communication 
{

	class WSDL_Wsdlpull : public IAuthenticable 
	{
	protected:
		/**
		Pole klasy przechowujace nazwe uzytkownika. Pole jest typu string.
		*/
		std::string usr;
		/**
		Pole klasy przechowujace haslo uzytkownika. Pole jest typu string.
		*/
		std::string pswd;
		/**
		Pole klasy przechowujace URI do definicji operacji. Pole jest typu string.
		*/
		std::string uri;
		/**
		Obiekt klasy WsdlInvoker biblioteki wsdlpull
		*/
		WsdlPull::WsdlInvoker invoker;
		/**
		Metoda konfigurujaca obiekt invoker przed wykonaniem operacji web serwisowych.
		@throw EDRException jesli inicjalizacja nie przebiegla pomyslnie lub nie ustawiono danych
		potrzebnych do autentykacji
		*/
		void initializeInvoker();
	public:
		/**
		Konstruktor klasy WSDL_Wsdlpull.
		*/
		WSDL_Wsdlpull();
		/**
		Konstruktor klasy WSDL_Wsdlpull.
		@param uri URI do dokumentu definicji operacji sieciowych typu string
		@param usr nazwa uzytkownika typu string
		@param pswd haslo uzytkownika typu string
		*/
		WSDL_Wsdlpull(const std::string& uri, const std::string& usr, const std::string& pswd);
		/**
		Wirtualny destruktor klasy WSDL_Wsdlpull.
		*/
		virtual ~WSDL_Wsdlpull();
		/**
		Metoda setUri ustala URI do dokumentu definicji operacji sieciowych typu string
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@param uri URI do definicji operacji sieciowych typu string
		*/
		void setUri(const std::string& uri);
		/**
		Metoda setUser ustala nazwe uzytkownika wykorzystywana do logowania.
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@param usr nazwa uzytkownika typu string
		*/
		void setUser(const std::string& usr);
		/**
		Metoda setUser ustala haslo uzytkownika wykorzystywane do logowania.
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@param pswd haslo uzytkownika typu string
		*/
		void setPassword(const std::string& pswd);
		/**
		Metoda setCredentials ustala dane uzytkownika wykorzystywane do logowania takie jak host,
		nazwa i haslo uzytkownika.
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@param uri adres zasobu typu string
		@param usr nazwa uzytkownika typu string
		@param pswd haslo uzytkownika typu string
		*/
		void setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd);
		/**
		Metoda zwraca nazwe hosta lub adres ip (w zaleznosci co zostalo podane) uzywana dla danego polaczenia.
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@return URI do definicji operacji sieciowych
		*/
		const std::string& getUri() const;
		/**
		Metoda zwraca login uzytkownika uzywana dla danego polaczenia.
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@return login uzytkownika
		*/
		const std::string& getUser() const;
		/**
		Metoda zwraca haslo uzytkownika uzywane dla danego polaczenia.
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@return haslo uzytkownika
		*/
		const std::string& getPassword() const;
		/**
		Metoda ustala operacje web serwisowa jaka ma zostac wykonana.
		@param name nazwa operacji ktora ma zostac wykonana
		@throw EDRException jesli inicjalizacja nie przebiegla pomyslnie lub nie ustawiono danych
		potrzebnych do autentykacji
		*/
		void setOperation(const std::string& name);
		/**
		Metoda podaje wartosc parametru operacji.
		@param param nazwa parametru operacji
		@param value wartosc parametru operacji
		@throw EDRException jesli inicjalizacja nie przebiegla pomyslnie lub nie ustawiono danych
		potrzebnych do autentykacji
		*/
		void setValue(const std::string& param, const std::string& value);
		/**
		Metoda wywoluje operacje web serwisowa.
		@throw EDRException jesli inicjalizacja nie przebiegla pomyslnie lub nie ustawiono danych
		potrzebnych do autentykacji
		*/
		void invokeOperation();
		/**
		Metoda zwraca odpowiedz serwera w formie XML.
		@return odpowiedz serwera na operacje w formie XML
		@throw EDRException jesli inicjalizacja nie przebiegla pomyslnie lub nie ustawiono danych
		potrzebnych do autentykacji
		*/
		std::string getXMLResponse();
		/**
		Metoda zamieniajaca liczbe na string
		@param value liczba do zamiany na string
		@return wartosc value zapisana jako string
		*/
		template <typename T> static std::string toString(T& value) 
		{
			std::ostringstream sstream;
			sstream << value;
			return sstream.str();
		}
		/**
		Metoda zamieniajaca przyjmuje wektor z liczbami i zwraca wektor z napisami
		@param value wektor liczb do zamiany na napisy
		@return wektor z napisami zamiast liczb
		*/
		template <typename T> static std::vector<std::string> toStringVector(std::vector<T>& value) 
		{
			std::vector<std::string> sVector;
			for(unsigned int i = 0; i < value.size(); i++) 
			{
				std::ostringstream sstream;
				sstream << value[i];
				sVector.push_back(sstream.str());
			}
			return sVector;
		}


		//tymczasowe i pomocnicze
		void displayOperations();
		void displayPortTypes();
		static const std::string wsdl_domain;
		static const std::string wsdl_usr;
		static const std::string wsdl_pswd;
	};
}
#endif
