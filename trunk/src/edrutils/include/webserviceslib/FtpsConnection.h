/**
@author Marek Daniluk
@brief Klasa FtpsConnection opiera si� o bibliotek� LibCurl i protok� FTPS. Metody czytania i odbierania pakiet�w
zosta�y zaimplementowane w spos�b zapobiegaj�cy blokowania si� nawzajem klienta i serwera. Klasa implementuje
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

    //! Klasa realizuj�ca po�aczenia FTP i FTPS
    class FtpsConnection : public IFtpsConnection
    {

    private:
        //! Obiekt implementuj�cy funkcjonalno��
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
        @param user nazwa u�ytkownika
        @param password has�o u�ytkownika
        */
        FtpsConnection(const std::string & url, const std::string & user, const std::string & password);
        /**
        Destruktor klasy FtpsConnection.
        */
        ~FtpsConnection();
        /**
        Metoda setUri ustala adres ip lub alias hosta z kt�rym zostanie nawi�zane po��czenie.
        @param url adres serwera FTPS
        */
        virtual void setUrl(const std::string & url);
        /**
        Metoda setUser ustala nazw� u�ytkownika wykorzystywana do logowania.
        @param user nazwa u�ytkownika
        */
        virtual void setUser(const std::string & user);
        /**
        Metoda setPassword ustala has�o u�ytkownika wykorzystywane do logowania.
        @param password has�o u�ytkownika
        */
        virtual void setPassword(const std::string & password);
        /**
        Metoda setCredentials ustala dane u�ytkownika wykorzystywane do logowania takie jak host,
        nazwa i has�o u�ytkownika.
        @param url adres serwera FTPS
        @param user nazwa u�ytkownika
        @param password has�o u�ytkownika
        */
        virtual void setCredentials(const std::string & user, const std::string & password);
        /**
        Metoda zwraca nazw� hosta lub adres ip (w zale�nosci co zosta�o podane) u�ywana dla danego po��czenia.
        @return uri do serwera FTPS
        */
        virtual const std::string url() const;
        /**
        Metoda zwraca login u�ytkownika u�ywany dla danego po��czenia.
        @return login uzytkownika
        */
        virtual const std::string user() const;
        /**
        Metoda zwraca has�o u�ytkownika u�ywane dla danego po��czenia.
        @return has�o u�ytkownika
        */
        virtual const std::string password() const;
        /**
        Metoda wysy�aj�ca na serwer plik. Nazwa wys�anego pliku przechowywana jest w zmiennej filename.
        @param localSource nazwa pliku kt�ry wysy�amy na serwer ftp
        @param remoteDestination miejsce zapisu naszego pliku na ftp
        @param customProgress customowy obiekt obs�uguj�cy post�p operacji
        */
        virtual const OperationStatus put(const std::string & localSource, const std::string & remoteDestination, IProgress * customProgress = nullptr);
        /**
        Metoda pobieraj�ca plik z serwera. Nazwa pobranego pliku przechowywana jest w zmiennej filename.
        @param remoteSource �cia�ka pliku na serwerze ftp kt�ry chcemy �ci�gn��
        @param localDestination �cia�ka w kt�rej chcemy zpaisa� �ci�gany plik
        @param customProgress customowy obiekt obs�uguj�cy post�p operacji
        */
        virtual const OperationStatus get(const std::string& remoteSource, const std::string & localDestination, IProgress * customProgress = nullptr);
        /**
        Post�p operacji przesy�ania.
        @return post�p wyra�ony w procentach dla pojedynczego transferu
        */
        virtual const int progress() const;
        /**
        Anuluje obecnie wykonywan� operacj�. Czy�ci zasoby uzyte na jej potrzeby (np. utworzone pliki)
        */
        virtual void abort();
        //! \return Informacja o ostatnim b��dzie
        virtual const std::string errorMessage() const;
    };
}
#endif //HEADER_GUARD_COMMUNICATION_FTPSCONNECTION_H__
