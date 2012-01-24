/**
@author Marek Daniluk
@brief Klasa WsdlConnection implementuje technologiê WSDL przy pomocy biblioteki wsdlpull.
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
        Pole klasy przechowuj¹ce nazwê u¿ytkownika.
        */
        std::string usr;
        /**
        Pole klasy przechowuj¹ce has³o u¿ytkownika.
        */
        std::string pswd;
        /**
        Pole klasy przechowuj¹ce URI do definicji operacji.
        */
        std::string uri;

        bool verifyPeer;
        WsdlPull::WsdlInvoker::HostVerification verifyHost;
        std::string caPath;
        /**
        Obiekt klasy WsdlInvoker biblioteki wsdlpull
        */
        core::shared_ptr<WsdlPull::WsdlInvoker> invoker;
        /**
        Metoda konfiguruj¹ca obiekt invoker przed wykonaniem operacji web serwisowych.
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
        @param usr nazwa u¿ytkownika
        @param pswd has³o u¿ytkownika
        */
        WsdlConnection(const std::string& uri, const std::string& usr, const std::string& pswd, bool verifyPeer = false, const std::string & caPath = std::string(), WsdlPull::WsdlInvoker::HostVerification verifyHost = WsdlPull::WsdlInvoker::CNAny);
        /**
        Wirtualny destruktor klasy WsdlConnection.
        */
        virtual ~WsdlConnection();
        /**
        Metoda setUri ustala URI do dokumentu definicji operacji sieciowych
        @param uri URI do definicji operacji sieciowych
        */
        virtual void setUri(const std::string& uri);

        void setSecurity(bool verifyPeer, const std::string & caPath, WsdlPull::WsdlInvoker::HostVerification verifyHost);
        /**
        Metoda setUser ustala nazwê u¿ytkownika wykorzystywana do logowania.
        @param usr nazwa u¿ytkownika
        */
        virtual void setUser(const std::string& usr);
        /**
        Metoda setPassword ustala has³o u¿ytkownika wykorzystywane do logowania.
        @param pswd has³o u¿ytkownika 
        */
        virtual void setPassword(const std::string& pswd);
        /**
        Metoda setCredentials ustala dane u¿ytkownika wykorzystywane do logowania takie jak host,
        nazwa i has³o u¿ytkownika.
        @param uri adres zasobu
        @param usr nazwa u¿ytkownika
        @param pswd has³o u¿ytkownika
        */
        virtual void setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd);
        /**
        Metoda zwraca nazwê hosta lub adres ip (w zaleznosci co zostalo podane) uzywana dla danego polaczenia.
        @return URI do definicji operacji sieciowych
        */
        virtual const std::string& getUri() const;
        /**
        Metoda zwraca login u¿ytkownika u¿ywany dla danego po³¹czenia.
        @return login u¿ytkownika
        */
        virtual const std::string& getUser() const;
        /**
        Metoda zwraca has³o u¿ytkownika u¿ywane dla danego po³¹czenia.
        @return has³o u¿ytkownika
        */
        virtual const std::string& getPassword() const;
        /**
        Metoda ustala operacje web serwisow¹ jaka ma zostaæ wykonana.
        @param name nazwa operacji która ma zostaæ wykonana
        */
        void setOperation(const std::string& name);
        /**
        Metoda podaje wartoœæ parametru operacji.
        @param param nazwa parametru operacji
        @param value wartoœæ parametru operacji
        */
        void setValue(const std::string& param, const std::string& value);
        /**
        Metoda wywo³uje operacje web serwisow¹.
        */
        void invokeOperation();
        /**
        Metoda zwraca odpowiedŸ serwera w formie XML.
        @return odpowiedŸ serwera na operacje w formie XML
        */
        std::string getXMLResponse();
        /**
        Metoda zamieniaj¹ca liczbê na string
        @param value liczba do zamiany na string
        @return wartoœæ value zapisana jako string
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
