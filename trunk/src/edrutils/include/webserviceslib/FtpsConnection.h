/**
@author Marek Daniluk
@brief Klasa FtpsConnection opiera siê o bibliotekê LibCurl i protokó³ FTPS. Metody czytania i odbierania pakietów
zosta³y zaimplementowane w sposób zapobiegaj¹cy blokowania siê nawzajem klienta i serwera. Klasa implementuje
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

    //! Klasa realizuj¹ca po³aczenia FTP i FTPS
    class FtpsConnection : public IFtpsConnection
    {

    private:
        //! Obiekt implementuj¹cy funkcjonalnoœæ
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
        @param user nazwa u¿ytkownika
        @param password has³o u¿ytkownika
        */
        FtpsConnection(const std::string & url, const std::string & user, const std::string & password);
        /**
        Destruktor klasy FtpsConnection.
        */
        ~FtpsConnection();
        /**
        Metoda setUri ustala adres ip lub alias hosta z którym zostanie nawi¹zane po³¹czenie.
        @param url adres serwera FTPS
        */
        virtual void setUrl(const std::string & url);
        /**
        Metoda setUser ustala nazwê u¿ytkownika wykorzystywana do logowania.
        @param user nazwa u¿ytkownika
        */
        virtual void setUser(const std::string & user);
        /**
        Metoda setPassword ustala has³o u¿ytkownika wykorzystywane do logowania.
        @param password has³o u¿ytkownika
        */
        virtual void setPassword(const std::string & password);
        /**
        Metoda setCredentials ustala dane u¿ytkownika wykorzystywane do logowania takie jak host,
        nazwa i has³o u¿ytkownika.
        @param url adres serwera FTPS
        @param user nazwa u¿ytkownika
        @param password has³o u¿ytkownika
        */
        virtual void setCredentials(const std::string & user, const std::string & password);
        /**
        Metoda zwraca nazwê hosta lub adres ip (w zale¿nosci co zosta³o podane) u¿ywana dla danego po³¹czenia.
        @return uri do serwera FTPS
        */
        virtual const std::string url() const;
        /**
        Metoda zwraca login u¿ytkownika u¿ywany dla danego po³¹czenia.
        @return login uzytkownika
        */
        virtual const std::string user() const;
        /**
        Metoda zwraca has³o u¿ytkownika u¿ywane dla danego po³¹czenia.
        @return has³o u¿ytkownika
        */
        virtual const std::string password() const;
        /**
        Metoda wysy³aj¹ca na serwer plik. Nazwa wys³anego pliku przechowywana jest w zmiennej filename.
        @param localSource nazwa pliku który wysy³amy na serwer ftp
        @param remoteDestination miejsce zapisu naszego pliku na ftp
        @param customProgress customowy obiekt obs³uguj¹cy postêp operacji
        */
        virtual const OperationStatus put(const std::string & localSource, const std::string & remoteDestination, IProgress * customProgress = nullptr);
        /**
        Metoda pobieraj¹ca plik z serwera. Nazwa pobranego pliku przechowywana jest w zmiennej filename.
        @param remoteSource œcia¿ka pliku na serwerze ftp który chcemy œci¹gn¹æ
        @param localDestination œcia¿ka w której chcemy zpaisaæ œci¹gany plik
        @param customProgress customowy obiekt obs³uguj¹cy postêp operacji
        */
        virtual const OperationStatus get(const std::string& remoteSource, const std::string & localDestination, IProgress * customProgress = nullptr);
        /**
        Postêp operacji przesy³ania.
        @return postêp wyra¿ony w procentach dla pojedynczego transferu
        */
        virtual const int progress() const;
        /**
        Anuluje obecnie wykonywan¹ operacjê. Czyœci zasoby uzyte na jej potrzeby (np. utworzone pliki)
        */
        virtual void abort();
        //! \return Informacja o ostatnim b³êdzie
        virtual const std::string errorMessage() const;
    };
}
#endif //HEADER_GUARD_COMMUNICATION_FTPSCONNECTION_H__
