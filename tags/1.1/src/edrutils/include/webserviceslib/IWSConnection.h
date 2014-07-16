/********************************************************************
    created:  2012/03/02
    created:  2:3:2012   13:40
    filename: IWSConnection.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___IWSCONNECTION_H__
#define HEADER_GUARD___IWSCONNECTION_H__

#include <stdexcept>
#include <webserviceslib/IWSCredentials.h>
#include <utils/SmartPtr.h>
#include <boost/lexical_cast.hpp>

namespace webservices
{

	//! WAŻNA UWAGA DO WYJĄTKÓW !! POWINNY BY RZUCANE TYLKO I WYŁĄCZNIE PODCZAS WYWOŁYWANIA INVOKE W ZALEŻNOŚCI CO SIĘ NIE POWIEDZIE !!
	//! MA TO NA CELU WPROWADZENIE LENIWEJ INICJALIZACJI USŁUG - POŁĄCZENIE JEST NAWIĄZYWANE FAKTYCZNIE W MOMENCIE UŻYCIA

	//! Wyjątek związany z wołaniem usług (invoke na connection) do których nie mamy praw
	class WSConnectionSecurityException : public std::runtime_error
	{
	public:
		WSConnectionSecurityException() throw() : std::runtime_error("") {}
		WSConnectionSecurityException(const WSConnectionSecurityException & exception) throw() : std::runtime_error(exception) {}
		WSConnectionSecurityException(const char * exception) throw() : std::runtime_error(exception) {}
	};

	//! Wyjątek związany z ustawianiem usługi - usługa nie oferuje takiej operacji
	class WSConnectionOperationException : public std::runtime_error
	{
	public:
		WSConnectionOperationException() throw() : std::runtime_error("") {}
		WSConnectionOperationException(const WSConnectionOperationException & exception) throw() : std::runtime_error(exception) {}
		WSConnectionOperationException(const char * exception) throw() : std::runtime_error(exception) {}
	};

	//! Wyjątek związany z ustawianiem usługi - metoda usługi nie przyjmuje wartości o zadanej nazwie
	class WSConnectionOperationValueException : public std::runtime_error
	{
	public:
		WSConnectionOperationValueException() throw() : std::runtime_error("") {}
		WSConnectionOperationValueException(const WSConnectionOperationValueException & exception) throw() : std::runtime_error(exception) {}
		WSConnectionOperationValueException(const char * exception) throw() : std::runtime_error(exception) {}
	};

	//! Wyjątek związany z inicjalizacją usługi - może się pojawić kiedy ściągamy definicję usługi i ją parsujemy
	class WSConnectionInitializationException : public std::runtime_error
	{
	public:
		WSConnectionInitializationException() throw() : std::runtime_error("") {}
		WSConnectionInitializationException(const WSConnectionInitializationException & exception) throw() : std::runtime_error(exception) {}
		WSConnectionInitializationException(const char * exception) throw() : std::runtime_error(exception) {}
	};

	//! Wyjątek związany z realizacją usługi przez web serwis
	class WSConnectionResponseException : public std::runtime_error
	{
	public:
		WSConnectionResponseException() throw() : std::runtime_error("") {}
		WSConnectionResponseException(const WSConnectionResponseException & exception) throw() : std::runtime_error(exception) {}
		WSConnectionResponseException(const char * exception) throw() : std::runtime_error(exception) {}
	};

	//! Wyjątek związany z wołaniem usługi - kiedy próbujemy już faktycznie wywołać usługę, kiedy parsujemy jej odpowiedź i coś idzie nie tak
	class WSConnectionInvokeException : public std::runtime_error
	{
	public:
		WSConnectionInvokeException() throw() : std::runtime_error("") {}
		WSConnectionInvokeException(const WSConnectionInvokeException & exception) throw() : std::runtime_error(exception) {}
		WSConnectionInvokeException(const char * exception) throw() : std::runtime_error(exception) {}
	};

    //! Interfejs realizujący połączenie z serwisami webowymi - metody powinny rzucać wyjątkiem w razie jakiegokolwiek niepowodzenia
    class IWSConnection : public IWSCredentials
    {
    public:
        //! Wirtualny destruktor
        virtual ~IWSConnection() {}

        //! \param url Adres serwisu
        virtual void setUrl(const std::string & url) = 0;
        //! \return adres serwisu
        virtual const std::string & url() const = 0;

        //! \param operation Metoda serwisu do wywołania
        virtual void setOperation(const std::string & operation) = 0;
        //! \param name Nazwa wartości do ustawienia
        //! \param value Wartość zmiennej
        virtual void setValue(const std::string & name, const std::string & value) = 0;
        //! Wykonuje operację na serwisie webowym
		//! \param process Czy odpowiedź ma być przetworzona i można o rezultaty pytać metodami getValue
		//! W przeciwnym wypadku należy samemu parsować odpowiedź
        virtual void invoke(bool process = false) = 0;
        //! \param name Nazwa wartości którą chcemy pobrać
        //! \return Wskaźnik do wartości, nullptr jeśli nie ma takiej wartości, wskaxnik pozostaje pod kontrolą implementacji IWSConnection
        virtual const void * getValue(const std::string & name) const = 0;

		//! \param param Nazwa parametru dla którego ustawiamy wartość wywołania
		//! \param value Wartośc dla zadanego parametru
        template<class T>
        void setValue(const std::string& param, const T& value)
        {
            setValue(param, boost::lexical_cast<std::string>(value));
        }

		//! \param param Nazwa parametru dla którego pobieramy wartość jeśli wykonywaliśmy zapytanie z parsowanie
		//! \param value [out] Wartośc dla zadanego parametru
        template<class T>
        void getValue(const std::string & name, T & value) const
        {
			const void * ret = getValue(name);
			if(ret != nullptr){
				value = *((T*)ret);
			}else{
				throw std::runtime_error("Could not find requested value in response.");
			}
        }

        //! \return Pełna odpowiedź serwisu webowego w formacie html/xml
        virtual const std::string xmlResponse() const = 0;
    };

    typedef utils::shared_ptr<IWSConnection> WSConnectionPtr;
    typedef utils::shared_ptr<const IWSConnection> WSConnectionConstPtr;

	//! Interfejs rozszerzający podstawowe połączenie webservices o zabezpieczenia
	class ISecureWSConnection : public IWSConnection
	{
	public:
		//! Mechanizm weryfikacji hosta dla bezpiecznych połączeń przez SSL za pomoca curla i openssl
		enum HostVerification {
			HVNone = 0,     //! Brak weryfikacji
			HVExist =  1,   //! Wpis o CN przynajmniej istnieje
			HVMatch = 2,    //! Wpis o CN istnieje i jest zgodny
		};

	public:
		//! Destruktor wirtualny
		virtual ~ISecureWSConnection() {}
		//! \param caPath Ścieżka certyfikatu, którym weryfikujemy hosta
		virtual void setCAPath(const std::string & caPath) = 0;
		//! \return Ścieżka certyfikatu, którym weryfikujemy hosta
		virtual const std::string CAPath() const = 0;

		//! \param hostVeryfication Mechanizm weryfikacji hosta po SSL względem ceryfikatów
		virtual void setHostVerification(HostVerification hostVerification) = 0;
		//! \return Mechanizm weryfikacji hosta po SSL względem ceryfikatów
		virtual const HostVerification hostVerification() const = 0;
	};

	typedef utils::shared_ptr<ISecureWSConnection> WSSecureConnectionPtr;
	typedef utils::shared_ptr<const ISecureWSConnection> WSSecureConnectionConstPtr;
}

#endif	//	HEADER_GUARD___IWSCONNECTION_H__
