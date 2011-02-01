/**
@author Marek Daniluk
@brief Klasa FTPS_cURL opiera si� o bibliotek� LibCurl i protok� FTPS. Metody czytania i odbierania pakiet�w
zosta�y zaimplementowane w spos�b zapobiegaj�cy blokowania si� nawzajem klienta i serwera. Klasa implementuje
interfejs ISendable.
*/

#ifndef _FTPS_CURL_H_
#define _FTPS_CURL_H_

#include "IFTP.h"

namespace communication
{


    /**
    @author Marek Daniluk
    @brief Struktura dla plik�w pobieranych lub wysy�anych przy pomocy bibliotek LibCurl. Zawiera informacje o nazwie
    pliku i wska�nik na plik.
    */
    struct FtpFile {
        /**
        Nazwa pliku.
        */
        const std::string& filename;
        /**
        Wska�nik na plik.
        */
        FILE* stream;
    };

    /**
    @author Marek Daniluk
    @brief Struktura pozwalaj�ca anulowanie operacji. Przechowuje te� informacj� o post�pie operacji.
    */
    struct Progress {
        /**
        Post�p
        */
        int progress;
        /**
        czy anulowa�?
        */
        bool abort;
    };
    class FTPS_cURL : public IFTP
    {
    private:
        /**
        Metoda statyczna (wymagana przez curla) typu callback wymagana przy przesy�aniu danych na serwer.
        Daje nam informacj� zwrotn� o ilo�ci danych wys�anych na serwer.
        @param buffer wska�nik do bloku pami�ci o rozmiarze size*nmemb
        @param size rozmiar w bajtach elementu do odczytania
        @param nmemb liczba element�w do odczytania
        @param stream wska�nik na strumie� danych
        @return ilo�� bajt�w przetworzonych przez funkcj�
        */
        static size_t read(void* buffer, size_t size, size_t nmemb, void* stream);
        /**
        Metoda statyczna (wymagana przez curla) typu callback wymagana przy odbieraniu danych z serwera.
        Daje nam informacj� zwrotn� o ilo�ci danych pobranych z serwera.
        @param buffer wska�nik do bloku pami�ci o rozmiarze size*nmemb
        @param size rozmiar w bajtach elementu do odczytania
        @param nmemb liczba element�w do odczytania
        @param stream wska�nik na strumie� danych
        @return ilo�� bajt�w przetworzonych przez funkcj�
        */
        static size_t write(void *buffer, size_t size, size_t nmemb, void *stream);
        /**
        Metoda statyczna (wymagana przez curla) typu callback wymagana przy aktualizacji post�pu.
        Daje nam informacje zwrotna o procentowym post�pie aktualnej operacji.
        @param progress wska�nik na struktur� typu Progress
        @param t wielo�� ca�kowita pliku pobieranego
        @param d obecna ilo�� pobranych bajt�w
        @param ultotal wielko�� ca�kowita pliku wysy�anego
        @param ulnow obecna ilo�� wys�anych bajt�w
        @return ilo�� bajt�w przetworzonych przez funkcj�
        */
        static size_t setProgress(Progress* progress, double t, /* dltotal */ double d, /* dlnow */ double ultotal, double ulnow);
    protected:
        /**
        Pole klasy przechowuj�ce uri do serwera FTPS.
        */
        std::string uri;
        /**
        Pole klasy przechowuj�ce nazw� u�ytkownika.
        */
        std::string usr;
        /**
        Pole klasy przechowuj�ce has�o u�ytkownika.
        */
        std::string pswd;
        /**
        Wska�nik na obiekt typu CURL potrzebny do operacji ftpowych.
        */
        CURL* curl;
        /**
        Pole klasy przechowuj�ce wyniki dzia�ania funkcji biblioteki LibCurl.
        */
        CURLcode res;
        /**
        Pole klasy przechowuj�ce informacje o procentowym post�pie operacji. S�u�y te� do przerwa� operacji.
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
        @param usr nazwa u�ytkownika
        @param pswd has�o u�ytkownika
        */
        FTPS_cURL(const std::string& uri, const std::string& usr, const std::string& pswd);
        /**
        Wirtualny destruktor klasy FTPS_cURL.
        */
        virtual ~FTPS_cURL();
        /**
        Metoda setUri ustala adres ip lub alias hosta z kt�rym zostanie nawi�zane po��czenie.
        @param uri adres serwera FTPS
        */
        virtual void setUri(const std::string& uri);
        /**
        Metoda setUser ustala nazw� u�ytkownika wykorzystywana do logowania.
        Ta metoda pochodzi z interfejsu IAuthenticable i zosta�a przes�oni�ta.
        @param usr nazwa u�ytkownika
        */
        virtual void setUser(const std::string& usr);
        /**
        Metoda setPassword ustala has�o u�ytkownika wykorzystywane do logowania.
        Ta metoda pochodzi z interfejsu IAuthenticable i zosta�a przes�oni�ta.
        @param pswd has�o u�ytkownika
        */
        virtual void setPassword(const std::string& pswd);
        /**
        Metoda setCredentials ustala dane u�ytkownika wykorzystywane do logowania takie jak host,
        nazwa i has�o u�ytkownika.
        Ta metoda pochodzi z interfejsu IAuthenticable i zosta�a przes�oni�ta.
        @param uri adres serwera FTPS
        @param usr nazwa u�ytkownika
        @param pswd has�o u�ytkownika
        */
        virtual void setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd);
        /**
        Metoda zwraca nazw� hosta lub adres ip (w zale�nosci co zosta�o podane) u�ywana dla danego po��czenia.
        Ta metoda pochodzi z interfejsu IAuthenticable i zosta�a przes�oni�ta.
        @return uri do serwera FTPS
        */
        virtual const std::string& getUri() const;
        /**
        Metoda zwraca login u�ytkownika u�ywany dla danego po��czenia.
        Ta metoda pochodzi z interfejsu IAuthenticable i zosta�a przes�oni�ta.
        @return login uzytkownika
        */
        virtual const std::string& getUser() const;
        /**
        Metoda zwraca has�o u�ytkownika u�ywane dla danego po��czenia.
        Ta metoda pochodzi z interfejsu IAuthenticable i zosta�a przes�oni�ta.
        @return has�o u�ytkownika
        */
        virtual const std::string& getPassword() const;
        /**
        Metoda wysy�aj�ca na serwer plik. Nazwa wys�anego pliku przechowywana jest w zmiennej filename.
        Ta metoda pochodzi z interfejsu ITransportable i zosta�a przes�oni�ta.
        @param filename nazwa pliku kt�ry ma wzi�� udzia� w operacji ftp
        */
        virtual void put(const std::string& filename);
        /**
        Metoda pobieraj�ca plik z serwera. Nazwa pobranego pliku przechowywana jest w zmiennej filename.
        Ta metoda pochodzi z interfejsu ITransportable i zosta�a przes�oni�ta.
        @param filename nazwa pliku kt�ry ma wzi�c udzia� w operacji ftp
        */
        virtual void get(const std::string& filename);
        /**
        Przej�cie do katalogu direcotry na serwerze FTP.
        Ta metoda pochodzi z interfejsu IFTP i zosta�a przes�oni�ta.
        @param directory katalog do kt�rego chcemy przej��
        */
        virtual void cd(const std::string& directory);
        /**
        Zwraca nazwe katalogu w kt�rym aktualnie przebywa klient.
        Ta metoda pochodzi z interfejsu IFTP i zosta�a przes�oni�ta.
        @return nazwa zdalnego katalogu w kt�rym znajduje si� klient
        */
        virtual const std::string& pwd() const;
        /**
        Wylistowanie plik�w i katalog�w w zdalnym katalogu w kt�rym aktualnie przebywa klient.
        Ta metoda pochodzi z interfejsu IFTP i zosta�a przes�oni�ta.
        @return lista katalog�w i plik�w ze zdalnego katalogu w kt�rym znajduje si� klient
        */
        virtual std::vector<const std::string&>& ls() const;
        /**
        Tworzy katalog na serwerze w zdalnym katalogu w kt�rym aktualnie przebywa klient.
        Ta metoda pochodzi z interfejsu IFTP i zosta�a przes�oni�ta.
        @param dirname nazwa katalogu do utworzenia
        */
        virtual void make(const std::string& dirname);
        /**
        Usuwa katalog lub plik na serwerze w zdalnym katalogu w kt�rym aktualnie przebywa klient.
        Ta metoda pochodzi z interfejsu IFTP i zosta�a przes�oni�ta.
        @param filename nazwa katalogu lub pliku do usuni�cia
        */
        virtual void del(const std::string& filename);
        /**
        Konczy po��czenie z serwerem FTP.
        Ta metoda pochodzi z interfejsu IFTP i zosta�a przes�oni�ta.
        */
        virtual void disconnect();
        /**
        Post�p operacji przesy�ania.
        Ta metoda pochodzi z interfejsu IFTP i zosta�a przes�oni�ta.
        @return post�p wyra�ony w procentach
        */
        virtual int getProgress() const;
        /**
        Anuluje obecnie wykonywan� operacj�.
        Ta metoda pochodzi z interfejsu IFTP i zosta�a przes�oni�ta.
        */
        virtual void abort();
    };
}
#endif
