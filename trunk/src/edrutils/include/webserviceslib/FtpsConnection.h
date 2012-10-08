/**
@author Marek Daniluk
@brief Klasa FtpsConnection opiera się o bibliotekę LibCurl i protokół FTPS. Metody czytania i odbierania pakietów
zostały zaimplementowane w sposób zapobiegający blokowania się nawzajem klienta i serwera. Klasa implementuje
interfejs ISendable.
*/

#ifndef HEADER_GUARD_COMMUNICATION_FTPSCONNECTION_H__
#define HEADER_GUARD_COMMUNICATION_FTPSCONNECTION_H__

#include <webserviceslib/IFtpsConnection.h>
#include <OpenThreads/ReentrantMutex>
#include <OpenThreads/ScopedLock>
#include <boost/shared_ptr.hpp>

namespace webservices
{
    //! Forward declaration pimpl
    class FtpsConnectionImpl;

    //! Klasa realizująca połaczenia FTP i FTPS
    class FtpsConnection : public IFtpsConnection
    {

    private:
        //! Obiekt implementujący funkcjonalność
        boost::shared_ptr<FtpsConnectionImpl> impl;

		OpenThreads::ReentrantMutex * mutex;

    public:
        /**
        Konstruktor klasy FtpsConnection.
        */
        FtpsConnection();
        /**
        Konstruktor klasy FtpsConnection.
        @param url adres serwera FTPS
        @param user nazwa użytkownika
        @param password hasło użytkownika
        */
        FtpsConnection(const std::string & url, const std::string & user, const std::string & password);
        /**
        Destruktor klasy FtpsConnection.
        */
        ~FtpsConnection();
        /**
        Metoda setUri ustala adres ip lub alias hosta z którym zostanie nawiązane połączenie.
        @param url adres serwera FTPS
        */
        virtual void setUrl(const std::string & url);
        /**
        Metoda setUser ustala nazwę użytkownika wykorzystywana do logowania.
        @param user nazwa użytkownika
        */
        virtual void setUser(const std::string & user);
        /**
        Metoda setPassword ustala hasło użytkownika wykorzystywane do logowania.
        @param password hasło użytkownika
        */
        virtual void setPassword(const std::string & password);
        /**
        Metoda setCredentials ustala dane użytkownika wykorzystywane do logowania takie jak host,
        nazwa i hasło użytkownika.
        @param url adres serwera FTPS
        @param user nazwa użytkownika
        @param password hasło użytkownika
        */
        virtual void setCredentials(const std::string & user, const std::string & password);
        /**
        Metoda zwraca nazwę hosta lub adres ip (w zależności co zostało podane) używana dla danego połączenia.
        @return uri do serwera FTPS
        */
        virtual const std::string url() const;
        /**
        Metoda zwraca login użytkownika używany dla danego połączenia.
        @return login użytkownika
        */
        virtual const std::string user() const;
        /**
        Metoda zwraca hasło użytkownika używane dla danego połączenia.
        @return hasło użytkownika
        */
        virtual const std::string password() const;
        /**
        Metoda wysyłająca na serwer plik. Nazwa wysłanego pliku przechowywana jest w zmiennej filename.
        @param localSource nazwa pliku który wysyłamy na serwer ftp
        @param remoteDestination miejsce zapisu naszego pliku na ftp
        @param customProgress customowy obiekt obsługujący postęp operacji
        */
        virtual const OperationStatus put(const std::string & localSource, const std::string & remoteDestination, IProgress * customProgress = nullptr);
        /**
        Metoda pobierająca plik z serwera. Nazwa pobranego pliku przechowywana jest w zmiennej filename.
        @param remoteSource ściażka pliku na serwerze ftp który chcemy ściągnąć
        @param localDestination ściażka w której chcemy zpaisać ściągany plik
        @param customProgress customowy obiekt obsługujący postęp operacji
        */
        virtual const OperationStatus get(const std::string& remoteSource, const std::string & localDestination, IProgress * customProgress = nullptr);
        /**
        Postęp operacji przesyłania.
        @return postęp wyrażony w procentach dla pojedynczego transferu
        */
        virtual const int progress() const;
        /**
        Anuluje obecnie wykonywaną operację. Czyści zasoby użyte na jej potrzeby (np. utworzone pliki)
        */
        virtual void abort();
        //! \return Informacja o ostatnim błędzie
        virtual const std::string errorMessage() const;
    };
}
#endif //HEADER_GUARD_COMMUNICATION_FTPSCONNECTION_H__
