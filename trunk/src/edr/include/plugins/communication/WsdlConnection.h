/**
@author Marek Daniluk
@brief Klasa WsdlConnection implementuje technologię WSDL przy pomocy biblioteki wsdlpull.
*/

#ifndef HEADER_GUARD_COMMUNICATION_WSDL_WSDLPULL_H__
#define HEADER_GUARD_COMMUNICATION_WSDL_WSDLPULL_H__

#include <wsdlparser/WsdlInvoker.h>

namespace communication 
{
    class WsdlConnection
    {
    protected:
        /**
        Pole klasy przechowujące nazwę użytkownika.
        */
        std::string usr;
        /**
        Pole klasy przechowujące hasło użytkownika.
        */
        std::string pswd;
        /**
        Pole klasy przechowujące URI do definicji operacji.
        */
        std::string uri;

        /**
        Obiekt klasy WsdlInvoker biblioteki wsdlpull
        */
        core::shared_ptr<WsdlPull::WsdlInvoker> invoker;
        /**
        Metoda konfigurująca obiekt invoker przed wykonaniem operacji web serwisowych.
        */
        void initializeInvoker();
    public:
        /**
        Konstruktor klasy WsdlConnection.
        */
        WsdlConnection();
        /**
        Konstruktor klasy WsdlConnection.
        @param uri URI do dokumentu definicji operacji sieciowych
        @param usr nazwa użytkownika
        @param pswd hasło użytkownika
        */
        WsdlConnection(const std::string& uri, const std::string& usr, const std::string& pswd/*, bool verifyPeer = false, const std::string & caPath = std::string(), WsdlPull::WsdlInvoker::HostVerification verifyHost = WsdlPull::WsdlInvoker::CNAny*/);
        /**
        Wirtualny destruktor klasy WsdlConnection.
        */
        virtual ~WsdlConnection();
        /**
        Metoda setUri ustala URI do dokumentu definicji operacji sieciowych
        @param uri URI do definicji operacji sieciowych
        */
        virtual void setUri(const std::string& uri);

        //void setSecurity(bool verifyPeer, const std::string & caPath, WsdlPull::WsdlInvoker::HostVerification verifyHost);
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
        @param uri adres zasobu
        @param usr nazwa użytkownika
        @param pswd hasło użytkownika
        */
        virtual void setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd);
        /**
        Metoda zwraca nazwę hosta lub adres ip (w zaleznosci co zostalo podane) uzywana dla danego polaczenia.
        @return URI do definicji operacji sieciowych
        */
        virtual const std::string& getUri() const;
        /**
        Metoda zwraca login użytkownika używany dla danego połączenia.
        @return login użytkownika
        */
        virtual const std::string& getUser() const;
        /**
        Metoda zwraca hasło użytkownika używane dla danego połączenia.
        @return hasło użytkownika
        */
        virtual const std::string& getPassword() const;
        /**
        Metoda ustala operacje web serwisową jaka ma zostać wykonana.
        @param name nazwa operacji która ma zostać wykonana
        */
        void setOperation(const std::string& name);
        /**
        Metoda podaje wartość parametru operacji.
        @param param nazwa parametru operacji
        @param value wartość parametru operacji
        */
        void setValue(const std::string& param, const std::string& value);
        /**
        Metoda wywołuje operacje web serwisową.
        */
        void invokeOperation();
        /**
        Metoda zwraca odpowiedź serwera w formie XML.
        @return odpowiedź serwera na operacje w formie XML
        */
        std::string getXMLResponse();
        /**
        Metoda zamieniająca liczbę na string
        @param value liczba do zamiany na string
        @return wartość value zapisana jako string
        */
        template <typename T> static std::string toString(T& value) 
        {
            std::ostringstream sstream;
            sstream << value;
            return sstream.str();
        }
    };
}
#endif
