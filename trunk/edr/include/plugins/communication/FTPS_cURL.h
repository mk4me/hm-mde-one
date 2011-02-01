/**
@author Marek Daniluk
@brief Klasa FTPS_cURL opiera siê o bibliotekê LibCurl i protokó³ FTPS. Metody czytania i odbierania pakietów
zosta³y zaimplementowane w sposób zapobiegaj¹cy blokowania siê nawzajem klienta i serwera. Klasa implementuje
interfejs ISendable.
*/

#ifndef _FTPS_CURL_H_
#define _FTPS_CURL_H_

#include "IFTP.h"

namespace communication
{


    /**
    @author Marek Daniluk
    @brief Struktura dla plików pobieranych lub wysy³anych przy pomocy bibliotek LibCurl. Zawiera informacje o nazwie
    pliku i wskaŸnik na plik.
    */
    struct FtpFile {
        /**
        Nazwa pliku.
        */
        const std::string& filename;
        /**
        WskaŸnik na plik.
        */
        FILE* stream;
    };

    /**
    @author Marek Daniluk
    @brief Struktura pozwalaj¹ca anulowanie operacji. Przechowuje te¿ informacjê o postêpie operacji.
    */
    struct Progress {
        /**
        Postêp
        */
        int progress;
        /**
        czy anulowaæ?
        */
        bool abort;
    };
    class FTPS_cURL : public IFTP
    {
    private:
        /**
        Metoda statyczna (wymagana przez curla) typu callback wymagana przy przesy³aniu danych na serwer.
        Daje nam informacjê zwrotn¹ o iloœci danych wys³anych na serwer.
        @param buffer wskaŸnik do bloku pamiêci o rozmiarze size*nmemb
        @param size rozmiar w bajtach elementu do odczytania
        @param nmemb liczba elementów do odczytania
        @param stream wskaŸnik na strumieñ danych
        @return iloœæ bajtów przetworzonych przez funkcjê
        */
        static size_t read(void* buffer, size_t size, size_t nmemb, void* stream);
        /**
        Metoda statyczna (wymagana przez curla) typu callback wymagana przy odbieraniu danych z serwera.
        Daje nam informacjê zwrotn¹ o iloœci danych pobranych z serwera.
        @param buffer wskaŸnik do bloku pamiêci o rozmiarze size*nmemb
        @param size rozmiar w bajtach elementu do odczytania
        @param nmemb liczba elementów do odczytania
        @param stream wskaŸnik na strumieñ danych
        @return iloœæ bajtów przetworzonych przez funkcjê
        */
        static size_t write(void *buffer, size_t size, size_t nmemb, void *stream);
        /**
        Metoda statyczna (wymagana przez curla) typu callback wymagana przy aktualizacji postêpu.
        Daje nam informacje zwrotna o procentowym postêpie aktualnej operacji.
        @param progress wskaŸnik na strukturê typu Progress
        @param t wieloœæ ca³kowita pliku pobieranego
        @param d obecna iloœæ pobranych bajtów
        @param ultotal wielkoœæ ca³kowita pliku wysy³anego
        @param ulnow obecna iloœæ wys³anych bajtów
        @return iloœæ bajtów przetworzonych przez funkcjê
        */
        static size_t setProgress(Progress* progress, double t, /* dltotal */ double d, /* dlnow */ double ultotal, double ulnow);
    protected:
        /**
        Pole klasy przechowuj¹ce uri do serwera FTPS.
        */
        std::string uri;
        /**
        Pole klasy przechowuj¹ce nazwê u¿ytkownika.
        */
        std::string usr;
        /**
        Pole klasy przechowuj¹ce has³o u¿ytkownika.
        */
        std::string pswd;
        /**
        WskaŸnik na obiekt typu CURL potrzebny do operacji ftpowych.
        */
        CURL* curl;
        /**
        Pole klasy przechowuj¹ce wyniki dzia³ania funkcji biblioteki LibCurl.
        */
        CURLcode res;
        /**
        Pole klasy przechowuj¹ce informacje o procentowym postêpie operacji. S³u¿y te¿ do przerwañ operacji.
        */
        Progress progress;
    public:
        /**
        Konstruktor klasy FTPS_cURL.
        */
        FTPS_cURL();
        /**
        Konstruktor klasy FTPS_cURL.
        @param uri adres serwera FTPS
        @param usr nazwa u¿ytkownika
        @param pswd has³o u¿ytkownika
        */
        FTPS_cURL(const std::string& uri, const std::string& usr, const std::string& pswd);
        /**
        Wirtualny destruktor klasy FTPS_cURL.
        */
        virtual ~FTPS_cURL();
        /**
        Metoda setUri ustala adres ip lub alias hosta z którym zostanie nawi¹zane po³¹czenie.
        @param uri adres serwera FTPS
        */
        virtual void setUri(const std::string& uri);
        /**
        Metoda setUser ustala nazwê u¿ytkownika wykorzystywana do logowania.
        Ta metoda pochodzi z interfejsu IAuthenticable i zosta³a przes³oniêta.
        @param usr nazwa u¿ytkownika
        */
        virtual void setUser(const std::string& usr);
        /**
        Metoda setPassword ustala has³o u¿ytkownika wykorzystywane do logowania.
        Ta metoda pochodzi z interfejsu IAuthenticable i zosta³a przes³oniêta.
        @param pswd has³o u¿ytkownika
        */
        virtual void setPassword(const std::string& pswd);
        /**
        Metoda setCredentials ustala dane u¿ytkownika wykorzystywane do logowania takie jak host,
        nazwa i has³o u¿ytkownika.
        Ta metoda pochodzi z interfejsu IAuthenticable i zosta³a przes³oniêta.
        @param uri adres serwera FTPS
        @param usr nazwa u¿ytkownika
        @param pswd has³o u¿ytkownika
        */
        virtual void setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd);
        /**
        Metoda zwraca nazwê hosta lub adres ip (w zale¿nosci co zosta³o podane) u¿ywana dla danego po³¹czenia.
        Ta metoda pochodzi z interfejsu IAuthenticable i zosta³a przes³oniêta.
        @return uri do serwera FTPS
        */
        virtual const std::string& getUri() const;
        /**
        Metoda zwraca login u¿ytkownika u¿ywany dla danego po³¹czenia.
        Ta metoda pochodzi z interfejsu IAuthenticable i zosta³a przes³oniêta.
        @return login uzytkownika
        */
        virtual const std::string& getUser() const;
        /**
        Metoda zwraca has³o u¿ytkownika u¿ywane dla danego po³¹czenia.
        Ta metoda pochodzi z interfejsu IAuthenticable i zosta³a przes³oniêta.
        @return has³o u¿ytkownika
        */
        virtual const std::string& getPassword() const;
        /**
        Metoda wysy³aj¹ca na serwer plik. Nazwa wys³anego pliku przechowywana jest w zmiennej filename.
        Ta metoda pochodzi z interfejsu ITransportable i zosta³a przes³oniêta.
        @param filename nazwa pliku który ma wzi¹æ udzia³ w operacji ftp
        */
        virtual void put(const std::string& filename);
        /**
        Metoda pobieraj¹ca plik z serwera. Nazwa pobranego pliku przechowywana jest w zmiennej filename.
        Ta metoda pochodzi z interfejsu ITransportable i zosta³a przes³oniêta.
        @param filename nazwa pliku który ma wzi¹c udzia³ w operacji ftp
        */
        virtual void get(const std::string& filename);
        /**
        Przejœcie do katalogu direcotry na serwerze FTP.
        Ta metoda pochodzi z interfejsu IFTP i zosta³a przes³oniêta.
        @param directory katalog do którego chcemy przejœæ
        */
        virtual void cd(const std::string& directory);
        /**
        Zwraca nazwe katalogu w którym aktualnie przebywa klient.
        Ta metoda pochodzi z interfejsu IFTP i zosta³a przes³oniêta.
        @return nazwa zdalnego katalogu w którym znajduje siê klient
        */
        virtual const std::string& pwd() const;
        /**
        Wylistowanie plików i katalogów w zdalnym katalogu w którym aktualnie przebywa klient.
        Ta metoda pochodzi z interfejsu IFTP i zosta³a przes³oniêta.
        @return lista katalogów i plików ze zdalnego katalogu w którym znajduje siê klient
        */
        virtual std::vector<const std::string&>& ls() const;
        /**
        Tworzy katalog na serwerze w zdalnym katalogu w którym aktualnie przebywa klient.
        Ta metoda pochodzi z interfejsu IFTP i zosta³a przes³oniêta.
        @param dirname nazwa katalogu do utworzenia
        */
        virtual void make(const std::string& dirname);
        /**
        Usuwa katalog lub plik na serwerze w zdalnym katalogu w którym aktualnie przebywa klient.
        Ta metoda pochodzi z interfejsu IFTP i zosta³a przes³oniêta.
        @param filename nazwa katalogu lub pliku do usuniêcia
        */
        virtual void del(const std::string& filename);
        /**
        Konczy po³¹czenie z serwerem FTP.
        Ta metoda pochodzi z interfejsu IFTP i zosta³a przes³oniêta.
        */
        virtual void disconnect();
        /**
        Postêp operacji przesy³ania.
        Ta metoda pochodzi z interfejsu IFTP i zosta³a przes³oniêta.
        @return postêp wyra¿ony w procentach
        */
        virtual int getProgress() const;
        /**
        Anuluje obecnie wykonywan¹ operacjê.
        Ta metoda pochodzi z interfejsu IFTP i zosta³a przes³oniêta.
        */
        virtual void abort();
    };
}
#endif
