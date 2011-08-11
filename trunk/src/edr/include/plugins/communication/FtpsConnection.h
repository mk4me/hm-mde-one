/**
@author Marek Daniluk
@brief Klasa FtpsConnection opiera si� o bibliotek� LibCurl i protok� FTPS. Metody czytania i odbierania pakiet�w
zosta�y zaimplementowane w spos�b zapobiegaj�cy blokowania si� nawzajem klienta i serwera. Klasa implementuje
interfejs ISendable.
*/

#ifndef HEADER_GUARD_COMMUNICATION_FTPSCONNECTION_H__
#define HEADER_GUARD_COMMUNICATION_FTPSCONNECTION_H__

#include <curl/curl.h>

namespace communication
{
    class FtpsConnection
    {
    private:
        /**
        @author Marek Daniluk
        @brief Struktura dla plik�w pobieranych lub wysy�anych przy pomocy bibliotek LibCurl. Zawiera informacje o nazwie
        pliku i wska�nik na plik.
        */
        struct FtpFile
        {
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
        struct Progress
        {
            /**
            Post�p
            */
            int progress;
            /**
            czy anulowa�?
            */
            bool abort;
        };
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
        Pole klasy przechowuj�ce informacje o procentowym post�pie operacji. S�u�y te� do przerwa� operacji.
        */
        Progress progress;
    private:
        /**
        Wska�nik na obiekt typu CURL potrzebny do operacji ftpowych.
        */
        CURL* curl;
        /**
        Pole klasy przechowuj�ce wyniki dzia�ania funkcji biblioteki LibCurl.
        */
        CURLcode res;
    public:
        /**
        Konstruktor klasy FtpsConnection.
        */
        FtpsConnection();
        /**
        Konstruktor klasy FtpsConnection.
        @param uri adres serwera FTPS
        @param usr nazwa u�ytkownika
        @param pswd has�o u�ytkownika
        */
        FtpsConnection(const std::string& uri, const std::string& usr, const std::string& pswd);
        /**
        Wirtualny destruktor klasy FtpsConnection.
        */
        virtual ~FtpsConnection();
        /**
        Metoda setUri ustala adres ip lub alias hosta z kt�rym zostanie nawi�zane po��czenie.
        @param uri adres serwera FTPS
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
        @param uri adres serwera FTPS
        @param usr nazwa u�ytkownika
        @param pswd has�o u�ytkownika
        */
        virtual void setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd);
        /**
        Metoda zwraca nazw� hosta lub adres ip (w zale�nosci co zosta�o podane) u�ywana dla danego po��czenia.
        @return uri do serwera FTPS
        */
        virtual const std::string& getUri() const;
        /**
        Metoda zwraca login u�ytkownika u�ywany dla danego po��czenia.
        @return login uzytkownika
        */
        virtual const std::string& getUser() const;
        /**
        Metoda zwraca has�o u�ytkownika u�ywane dla danego po��czenia.
        @return has�o u�ytkownika
        */
        virtual const std::string& getPassword() const;
        /**
        Metoda wysy�aj�ca na serwer plik. Nazwa wys�anego pliku przechowywana jest w zmiennej filename.
        @param filename nazwa pliku kt�ry ma wzi�� udzia� w operacji ftp
        */
        virtual void put(const std::string& filename);
        /**
        Metoda pobieraj�ca plik z serwera. Nazwa pobranego pliku przechowywana jest w zmiennej filename.
        @param filename nazwa pliku kt�ry ma wzi�c udzia� w operacji ftp
        */
        virtual void get(const std::string& filename);
        /**
        Post�p operacji przesy�ania.
        @return post�p wyra�ony w procentach
        */
        virtual int getProgress() const;
        /**
        Anuluje obecnie wykonywan� operacj�.
        */
        virtual void abort();
    };
}
#endif //HEADER_GUARD_COMMUNICATION_FTPSCONNECTION_H__
