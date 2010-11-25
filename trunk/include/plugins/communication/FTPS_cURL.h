/**
@author Marek Daniluk
@brief Klasa FTPS_cURL opiera sie o biblioteke LibCurl i protokol FTPS. Metody czytania i odbierania pakietow
zostaly zaimplementowane w sposob zapobiegajacy blokowania sie nawzajem klienta i serwera. Obecnie jest to
tymczasowa implementacja, brak obslugi bledow (nie wiadomo czy bedzie to wykonane przy pomocy wyjatkow, czy tez
innej metody), informacje sa przedstawiane w konsoli. Klasa implementuje interfejs ISendable.
*/

#ifndef _FTPS_CURL_H_
#define _FTPS_CURL_H_

#include "IFTP.h"

namespace communication
{

	class FTPS_cURL : public IFTP
	{
	private:
		/**
		Metoda statyczna (wymagana przez curla) typu callback wymagana przy przesylaniu danych na serwer.
		Daje nam informacje zwrotna o ilosci danych wyslanych na serwer.
		@param buffer wskaznik do bloku pamieci o rozmiarze size*nmemb
		@param size rozmiar w bajtach elementu do odczytania
		@param nmemb liczba elementow do odczytania
		@param stream wskaznik na strumien danych
		*/
		static size_t read(void* buffer, size_t size, size_t nmemb, void* stream);
		/**
		Metoda statyczna (wymagana przez curla) typu callback wymagana przy odbieraniu danych z serwera.
		Daje nam informacje zwrotna o ilosci danych pobranych z serwera.
		@param buffer wskaznik do bloku pamieci o rozmiarze size*nmemb
		@param size rozmiar w bajtach elementu do odczytania
		@param nmemb liczba elementow do odczytania
		@param stream wskaznik na strumien danych
		*/
		static size_t write(void *buffer, size_t size, size_t nmemb, void *stream);
	protected:
		/**
		Pole klasy przechowujace uri do serwera FTPS. Pole jest typu string.
		*/
		std::string uri;
		/**
		Pole klasy przechowujace nazwe uzytkownika. Pole jest typu string.
		*/
		std::string usr;
		/**
		Pole klasy przechowujace haslo uzytkownika. Pole jest typu string.
		*/
		std::string pswd;
		/**
		Wskaznik na obiekty typu CURL potrzebny do operacji na ftp
		*/
		CURL* curl;
		/**
		Pole klasy przechowujace wyniki dzialania funkcji biblioteki LibCurl.
		*/
		CURLcode res;
	public:
		/**
		Konstruktor klasy FTPS_cURL.
		@throws EDRException w przypadku problemu inicjalizacji polaczenia FTPS
		*/
		FTPS_cURL();
		/**
		Konstruktor klasy FTPS_cURL.
		@param uri adres serwera FTPS typu string
		@param usr nazwa uzytkownika typu string
		@param pswd haslo uzytkownika typu string
		@throws EDRException w przypadku problemu inicjalizacji polaczenia FTPS
		*/
		FTPS_cURL(const std::string& uri, const std::string& usr, const std::string& pswd);
		/**
		Wirtualny destruktor klasy FTPS_cURL.
		*/
		virtual ~FTPS_cURL();
		/**
		Metoda setUri ustala adres ip lub alias hosta z ktorym zostanie nawiazane polaczenie.
		@param uri adres serwera FTPS typu string
		*/
		void setUri(const std::string& uri);
		/**
		Metoda setUser ustala nazwe uzytkownika wykorzystywana do logowania.
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@param usr nazwa uzytkownika typu string
		*/
		void setUser(const std::string& usr);
		/**
		Metoda setPassword ustala haslo uzytkownika wykorzystywane do logowania.
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@param pswd haslo uzytkownika typu string
		*/
		void setPassword(const std::string& pswd);
		/**
		Metoda setCredentials ustala dane uzytkownika wykorzystywane do logowania takie jak host,
		nazwa i haslo uzytkownika.
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@param uri adres serwera FTPS typu string
		@param usr nazwa uzytkownika typu string
		@param pswd haslo uzytkownika typu string
		*/
		void setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd);
		/**
		Metoda zwraca nazwe hosta lub adres ip (w zaleznosci co zostalo podane) uzywana dla danego polaczenia.
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@return uri do serwera FTPS
		*/
		const std::string& getUri() const;
		/**
		Metoda zwraca login uzytkownika uzywana dla danego polaczenia.
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@return login uzytkownika
		*/
		const std::string& getUser() const;
		/**
		Metoda zwraca haslo uzytkownika uzywana dla danego polaczenia.
		Ta metoda pochodzi z interfejsu IAuthenticable i zostala przeslonieta.
		@return haslo uzytkownika
		*/
		const std::string& getPassword() const;
		/**
		Metoda wysylajaca na serwer plik. Nazwa wyslanego pliku przechowywana jest w zmiennej filename.
		Ta metoda pochodzi z interfejsu ITransportable i zostala przeslonieta.
		@param filename nazwa pliku ktory ma wziac udzial w operacji ftp
		@throws EDRException w przypadku nieudanej proby wyslania pliku
		*/
		void put(const std::string& filename);
		/**
		Metoda pobierajaca plik z serwera. Nazwa pobranego pliku przechowywana jest w zmiennej filename.
		Ta metoda pochodzi z interfejsu ITransportable i zostala przeslonieta.
		@param filename nazwa pliku ktory ma wziac udzial w operacji ftp
		@throws EDRException w przypadku nieudanej proby pobrania pliku
		*/
		void get(const std::string& filename);
		/**
		Przejscie do katalogu direcotry na serwerze FTP.
		Ta metoda pochodzi z interfejsu IFTP i zostala przeslonieta.
		@param directory katalog do ktorego chcemy przejsc
		*/
		void cd(const std::string& directory);
		/**
		Zwraca nazwe katalogu w ktorym aktualnie przebywa klient.
		Ta metoda pochodzi z interfejsu IFTP i zostala przeslonieta.
		@return nazwa zdalnego katalogu w ktorym znajduje sie klient
		*/
		const std::string& pwd() const;
		/**
		Wylistowanie plikow i katalogow w zdalnym katalogu w ktorym aktualnie przebywa klient.
		Ta metoda pochodzi z interfejsu IFTP i zostala przeslonieta.
		@return lista katalogow i plikow ze zdalnego katalogu w ktorym znajduje sie klient
		*/
		std::vector<const std::string&>& ls() const;
		/**
		Tworzy katalog na serwerze w zdalnym katalogu w ktorym aktualnie przebywa klient.
		Ta metoda pochodzi z interfejsu IFTP i zostala przeslonieta.
		@param dirname nazwa katalogu do utworzenia
		*/
		void make(const std::string& dirname);
		/**
		Usuwa katalog lub plik na serwerze w zdalnym katalogu w ktorym aktualnie przebywa klient.
		Ta metoda pochodzi z interfejsu IFTP i zostala przeslonieta.
		@param filename nazwa katalogu lub pliku do usuniecia
		*/
		void del(const std::string& filename);
		/**
		Konczy polaczenie z serwerem FTP.
		Ta metoda pochodzi z interfejsu IFTP i zostala przeslonieta.
		*/
		void disconnect();

		//tymczasowe i pomocnicze
		static const std::string ftps_addr;
		static const std::string ftps_usr;
		static const std::string ftps_pswd;
	};

	/**
	@author Marek Daniluk
	@brief Struktura dla plikow pobieranych lub wyslanych przy pomocy bibliotek LibCurl. Zawiera informacje o nazwie
	pliku i wskaznik na plik.
	*/

	struct FtpFile {
		/**
		Nazwa pliku.
		*/
		const std::string& filename;
		/**
		Wskaznik na plik.
		*/
		FILE* stream;
	};
}
#endif
