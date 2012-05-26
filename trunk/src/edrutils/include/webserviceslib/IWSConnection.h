/********************************************************************
    created:  2012/03/02
    created:  2:3:2012   13:40
    filename: IWSConnection.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IWSCONNECTION_H__
#define HEADER_GUARD___IWSCONNECTION_H__

#include <webserviceslib/IWSCredentials.h>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

namespace webservices
{

	//! WA¯NA UWAGA DO WYJ¥TKÓW !! POWINNY BY RZUCANE TYLKO I WY£¥CZNIE PODCZAS WYWO£YWANIA INVOKE W ZALE¯NOŒCI CO SIÊ NIE POWIEDZIE !!
	//! MA TO NA CELU WPROWADZENIE LENIWEJ INICJALIZACJI US£UG - PO£¥CZENIE JEST NAWI¥ZYWANE FAKTYCZNIE W MOMENCIE U¯YCIA

	//! Wyj¹tek zwi¹zany z wo³aniem us³ug (invoke na connection) do których nie mamy praw
	class WSConnectionSecurityException : public std::exception
	{
	public:
		WSConnectionSecurityException() throw() : std::exception() {}
		WSConnectionSecurityException(const WSConnectionSecurityException & exception) throw() : std::exception(exception) {}
		WSConnectionSecurityException(const char * exception) throw() : std::exception(exception) {}
	};

	//! Wyj¹tek zwi¹zany z ustawianiem us³ugi - us³uga nie oferuje takiej operacji
	class WSConnectionOperationException : public std::exception
	{
	public:
		WSConnectionOperationException() throw() : std::exception() {}
		WSConnectionOperationException(const WSConnectionOperationException & exception) throw() : std::exception(exception) {}
		WSConnectionOperationException(const char * exception) throw() : std::exception(exception) {}
	};

	//! Wyj¹tek zwi¹zany z ustawianiem us³ugi - metoda us³ugi nie przyjmuje wartoœci o zadanej nazwie
	class WSConnectionOperationValueException : public std::exception
	{
	public:
		WSConnectionOperationValueException() throw() : std::exception() {}
		WSConnectionOperationValueException(const WSConnectionOperationValueException & exception) throw() : std::exception(exception) {}
		WSConnectionOperationValueException(const char * exception) throw() : std::exception(exception) {}
	};

	//! Wyj¹tek zwi¹zany z inicjalizacj¹ us³ugi - mo¿e siê pojawiæ kiedy œci¹gamy definicjê us³ugi i j¹ parsujemy
	class WSConnectionInitializationException : public std::exception
	{
	public:
		WSConnectionInitializationException() throw() : std::exception() {}
		WSConnectionInitializationException(const WSConnectionInitializationException & exception) throw() : std::exception(exception) {}
		WSConnectionInitializationException(const char * exception) throw() : std::exception(exception) {}
	};

	//! Wyj¹tek zwi¹zany z realizacj¹ us³ugi przez web serwis
	class WSConnectionResponseException : public std::exception
	{
	public:
		WSConnectionResponseException() throw() : std::exception() {}
		WSConnectionResponseException(const WSConnectionResponseException & exception) throw() : std::exception(exception) {}
		WSConnectionResponseException(const char * exception) throw() : std::exception(exception) {}
	};

	//! Wyj¹tek zwi¹zany z wo³aniem us³ugi - kiedy próbujemy ju¿ faktycznie wywo³aæ us³ugê, kiedy parsujemy jej odpowiedŸ i coœ idzie nie tak
	class WSConnectionInvokeException : public std::exception
	{
	public:
		WSConnectionInvokeException() throw() : std::exception() {}
		WSConnectionInvokeException(const WSConnectionInvokeException & exception) throw() : std::exception(exception) {}
		WSConnectionInvokeException(const char * exception) throw() : std::exception(exception) {}
	};

    //! Interfejs realizuj¹cy po³¹czenie z serwisami webowymi - metody powinny rzucac wyj¹tkiem w razie jakiegokolwiek niepowodzenia
    class IWSConnection : public IWSCredentials
    {
    public:
        //! Wirtualny destruktor
        virtual ~IWSConnection() {}

        //! \param url Adres serwisu
        virtual void setUrl(const std::string & url) = 0;
        //! \return adres serwisu
        virtual const std::string & url() const = 0;

        //! \param operation Metoda serwisu do wywo³ania
        virtual void setOperation(const std::string & operation) = 0;
        //! \param name Nazwa wartoœci do ustawienia
        //! \param value Wartoœæ zmiennej
        virtual void setValue(const std::string & name, const std::string & value) = 0;
        //! Wykonuje operacjê na serwisie webowym
		//! \param process Czy odpowiedŸ ma byæ przetworzona i mo¿na o rezultaty pytaæ metodami getValue
		//! W przeciwnym wypadku nale¿y samemu parsowaæ odpowiedŸ
        virtual void invoke(bool process = false) = 0;
        //! \param name Nazwa wartoœci któr¹ chcemy pobraæ
        //! \return WskaŸnik do wartoœci, nullptr jeœli nie ma takiej wartoœci, wskaxnik pozostaje pod kontrol¹ implementacji IWSConnection
        virtual void * getValue(const std::string & name) = 0;

		//! \param param Nazwa parametru dla którego ustawiamy wartoœæ wywo³ania
		//! \param value Wartoœc dla zadanego parametru
        template<class T>
        void setValue(const std::string& param, const T& value)
        {
            setValue(param, boost::lexical_cast<std::string>(value));
        }

		//! \param param Nazwa parametru dla którego pobieramy wartoœæ jeœli wykonywaliœmy zapytanie z parsowanie
		//! \param value [out] Wartoœc dla zadanego parametru
        template<class T>
        void getValue(const std::string & name, T & value)
        {
			void * ret = getValue(name);
			if(ret != nullptr){
				value = *((T*)ret);
			}else{
				throw std::runtime_error("Could not find requested value in response.");
			}            
        }

        //! \return Pe³na odpowiedŸ serwisu webowego w formacie html/xml
        virtual const std::string xmlResponse() = 0;
    };

    typedef boost::shared_ptr<IWSConnection> WSConnectionPtr;
    typedef boost::shared_ptr<const IWSConnection> WSConnectionConstPtr;
}

#endif	//	HEADER_GUARD___IWSCONNECTION_H__
