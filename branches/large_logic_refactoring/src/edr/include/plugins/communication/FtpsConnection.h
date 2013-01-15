/**
@author Marek Daniluk
@brief Klasa FtpsConnection opiera się o bibliotekę LibCurl i protokół FTPS. Metody czytania i odbierania pakietów
zostały zaimplementowane w sposób zapobiegający blokowania się nawzajem klienta i serwera. Klasa implementuje
interfejs ISendable.
*/

#ifndef HEADER_GUARD_COMMUNICATION_FTPSCONNECTION_H__
#define HEADER_GUARD_COMMUNICATION_FTPSCONNECTION_H__

#include <curl/curl.h>

namespace communication
{
    class FtpsConnection
    {
    public:

        enum OperationStatus {Complete, Error, Cancelled};

        class IProgress
        {
        public:
            virtual ~IProgress() {}

            virtual void setProgress(double progress) = 0;

        };

    private:
        /**
        @author Marek Daniluk
        @brief Struktura pozwalająca na anulowanie operacji. Przechowuje też informację o postępie operacji.
        */
        struct Progress
        {
            /**
            Postęp
            */
            int progress;

            /**
            Postep szczegolowy wg klienta
            */
            IProgress * customProgress;

            /**
            czy anulować?
            */
            bool abort;
        };
        /**
        Metoda statyczna (wymagana przez curla) typu callback wymagana przy przesyłaniu danych na serwer.
        Daje nam informację zwrotną o ilości danych wysłanych na serwer.
        @param buffer wskaźnik do bloku pamięci o rozmiarze size*nmemb
        @param size rozmiar w bajtach elementu do odczytania
        @param nmemb liczba elementów do odczytania
        @param stream wskaźnik na strumień danych
        @return ilość bajtów przetworzonych przez funkcję
        */
        static size_t read(void* buffer, size_t size, size_t nmemb, void* stream);
        /**
        Metoda statyczna (wymagana przez curla) typu callback wymagana przy odbieraniu danych z serwera.
        Daje nam informację zwrotną o ilości danych pobranych z serwera.
        @param buffer wskaźnik do bloku pamięci o rozmiarze size*nmemb
        @param size rozmiar w bajtach elementu do odczytania
        @param nmemb liczba elementów do odczytania
        @param stream wskaźnik na strumień danych
        @return ilość bajtów przetworzonych przez funkcję
        */
        static size_t write(void *buffer, size_t size, size_t nmemb, void *stream);
        /**
        Metoda statyczna (wymagana przez curla) typu callback wymagana przy aktualizacji postępu.
        Daje nam informacje zwrotna o procentowym postępie aktualnej operacji.
        @param progress wskaźnik na strukturę typu Progress
        @param t wielość całkowita pliku pobieranego
        @param d obecna ilość pobranych bajtów
        @param ultotal wielkość całkowita pliku wysyłanego
        @param ulnow obecna ilość wysłanych bajtów
        @return ilość bajtów przetworzonych przez funkcję
        */
        static size_t setProgress(Progress* progress, double t, /* dltotal */ double d, /* dlnow */ double ultotal, double ulnow);
        static size_t setCustomProgress(Progress* progress, double t, /* dltotal */ double d, /* dlnow */ double ultotal, double ulnow);
    protected:
        /**
        Pole klasy przechowujące uri do serwera FTPS.
        */
        std::string uri;
        /**
        Pole klasy przechowujące nazwę użytkownika.
        */
        std::string usr;
        /**
        Pole klasy przechowujące hasło użytkownika.
        */
        std::string pswd;
        /**
        Pole klasy przechowujące informacje o procentowym postępie operacji. Służy też do przerwań operacji.
        */
        Progress progress;

        std::string errorMessage;

    private:
        /**
        Wskaźnik na obiekt typu CURL potrzebny do operacji ftpowych.
        */
        CURL* curl;
        /**
        Pole klasy przechowujące wyniki działania funkcji biblioteki LibCurl.
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
        @param usr nazwa użytkownika
        @param pswd hasło użytkownika
        */
        FtpsConnection(const std::string& uri, const std::string& usr, const std::string& pswd);
        /**
        Wirtualny destruktor klasy FtpsConnection.
        */
        virtual ~FtpsConnection();
        /**
        Metoda setUri ustala adres ip lub alias hosta z którym zostanie nawiązane połączenie.
        @param uri adres serwera FTPS
        */
        virtual void setUri(const std::string& uri);
        /**
        Metoda setUser ustala nazwę użytkownika wykorzystywana do logowania.
        @param usr nazwa użytkownika
        */
        virtual void setUser(const std::string& usr);
        /**
        Metoda setPassword ustala hasło użytkownika wykorzystywane do logowania.
        @param pswd hasło użytkownika
        */
        virtual void setPassword(const std::string& pswd);
        /**
        Metoda setCredentials ustala dane użytkownika wykorzystywane do logowania takie jak host,
        nazwa i hasło użytkownika.
        @param uri adres serwera FTPS
        @param usr nazwa użytkownika
        @param pswd hasło użytkownika
        */
        virtual void setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd);
        /**
        Metoda zwraca nazwę hosta lub adres ip (w zależnosci co zostało podane) używana dla danego połączenia.
        @return uri do serwera FTPS
        */
        virtual const std::string& getUri() const;
        /**
        Metoda zwraca login użytkownika używany dla danego połączenia.
        @return login uzytkownika
        */
        virtual const std::string& getUser() const;
        /**
        Metoda zwraca hasło użytkownika używane dla danego połączenia.
        @return hasło użytkownika
        */
        virtual const std::string& getPassword() const;
        /**
        Metoda wysyłająca na serwer plik. Nazwa wysłanego pliku przechowywana jest w zmiennej filename.
        @param filename nazwa pliku który ma wziąć udział w operacji ftp
        */
        virtual OperationStatus put(const std::string& localSource, const std::string & remoteDestination);
        /**
        Metoda pobierająca plik z serwera. Nazwa pobranego pliku przechowywana jest w zmiennej filename.
        @param filename nazwa pliku który ma wziąc udział w operacji ftp
        */
        virtual OperationStatus get(const std::string& remoteSource, const std::string & localDestination, IProgress * customProgress = nullptr);
        /**
        Postęp operacji przesyłania.
        @return postęp wyrażony w procentach dla pojedynczego transferu
        */
        virtual int getProgress() const;
        /**
        Anuluje obecnie wykonywaną operację. Czyści zasoby uzyte na jej potrzeby (np. utworzone pliki)
        */
        virtual void abort();

        virtual const std::string & getLastError() const;
    };
}
#endif //HEADER_GUARD_COMMUNICATION_FTPSCONNECTION_H__
