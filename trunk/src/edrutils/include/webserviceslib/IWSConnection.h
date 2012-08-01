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
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

namespace webservices
{

	//! WA�NA UWAGA DO WYJ�TK�W !! POWINNY BY RZUCANE TYLKO I WY��CZNIE PODCZAS WYWO�YWANIA INVOKE W ZALE�NO�CI CO SI� NIE POWIEDZIE !!
	//! MA TO NA CELU WPROWADZENIE LENIWEJ INICJALIZACJI US�UG - PO��CZENIE JEST NAWI�ZYWANE FAKTYCZNIE W MOMENCIE U�YCIA

	//! Wyj�tek zwi�zany z wo�aniem us�ug (invoke na connection) do kt�rych nie mamy praw
	class WSConnectionSecurityException : public std::runtime_error
	{
	public:
		WSConnectionSecurityException() throw() : std::runtime_error("") {}
		WSConnectionSecurityException(const WSConnectionSecurityException & exception) throw() : std::runtime_error(exception) {}
		WSConnectionSecurityException(const char * exception) throw() : std::runtime_error(exception) {}
	};

	//! Wyj�tek zwi�zany z ustawianiem us�ugi - us�uga nie oferuje takiej operacji
	class WSConnectionOperationException : public std::runtime_error
	{
	public:
		WSConnectionOperationException() throw() : std::runtime_error("") {}
		WSConnectionOperationException(const WSConnectionOperationException & exception) throw() : std::runtime_error(exception) {}
		WSConnectionOperationException(const char * exception) throw() : std::runtime_error(exception) {}
	};

	//! Wyj�tek zwi�zany z ustawianiem us�ugi - metoda us�ugi nie przyjmuje warto�ci o zadanej nazwie
	class WSConnectionOperationValueException : public std::runtime_error
	{
	public:
		WSConnectionOperationValueException() throw() : std::runtime_error("") {}
		WSConnectionOperationValueException(const WSConnectionOperationValueException & exception) throw() : std::runtime_error(exception) {}
		WSConnectionOperationValueException(const char * exception) throw() : std::runtime_error(exception) {}
	};

	//! Wyj�tek zwi�zany z inicjalizacj� us�ugi - mo�e si� pojawi� kiedy �ci�gamy definicj� us�ugi i j� parsujemy
	class WSConnectionInitializationException : public std::runtime_error
	{
	public:
		WSConnectionInitializationException() throw() : std::runtime_error("") {}
		WSConnectionInitializationException(const WSConnectionInitializationException & exception) throw() : std::runtime_error(exception) {}
		WSConnectionInitializationException(const char * exception) throw() : std::runtime_error(exception) {}
	};

	//! Wyj�tek zwi�zany z realizacj� us�ugi przez web serwis
	class WSConnectionResponseException : public std::runtime_error
	{
	public:
		WSConnectionResponseException() throw() : std::runtime_error("") {}
		WSConnectionResponseException(const WSConnectionResponseException & exception) throw() : std::runtime_error(exception) {}
		WSConnectionResponseException(const char * exception) throw() : std::runtime_error(exception) {}
	};

	//! Wyj�tek zwi�zany z wo�aniem us�ugi - kiedy pr�bujemy ju� faktycznie wywo�a� us�ug�, kiedy parsujemy jej odpowied� i co� idzie nie tak
	class WSConnectionInvokeException : public std::runtime_error
	{
	public:
		WSConnectionInvokeException() throw() : std::runtime_error("") {}
		WSConnectionInvokeException(const WSConnectionInvokeException & exception) throw() : std::runtime_error(exception) {}
		WSConnectionInvokeException(const char * exception) throw() : std::runtime_error(exception) {}
	};

    //! Interfejs realizuj�cy po��czenie z serwisami webowymi - metody powinny rzucac wyj�tkiem w razie jakiegokolwiek niepowodzenia
    class IWSConnection : public IWSCredentials
    {
    public:
        //! Wirtualny destruktor
        virtual ~IWSConnection() {}

        //! \param url Adres serwisu
        virtual void setUrl(const std::string & url) = 0;
        //! \return adres serwisu
        virtual const std::string & url() const = 0;

        //! \param operation Metoda serwisu do wywo�ania
        virtual void setOperation(const std::string & operation) = 0;
        //! \param name Nazwa warto�ci do ustawienia
        //! \param value Warto�� zmiennej
        virtual void setValue(const std::string & name, const std::string & value) = 0;
        //! Wykonuje operacj� na serwisie webowym
		//! \param process Czy odpowied� ma by� przetworzona i mo�na o rezultaty pyta� metodami getValue
		//! W przeciwnym wypadku nale�y samemu parsowa� odpowied�
        virtual void invoke(bool process = false) = 0;
        //! \param name Nazwa warto�ci kt�r� chcemy pobra�
        //! \return Wska�nik do warto�ci, nullptr je�li nie ma takiej warto�ci, wskaxnik pozostaje pod kontrol� implementacji IWSConnection
        virtual void * getValue(const std::string & name) = 0;

		//! \param param Nazwa parametru dla kt�rego ustawiamy warto�� wywo�ania
		//! \param value Warto�c dla zadanego parametru
        template<class T>
        void setValue(const std::string& param, const T& value)
        {
            setValue(param, boost::lexical_cast<std::string>(value));
        }

		//! \param param Nazwa parametru dla kt�rego pobieramy warto�� je�li wykonywali�my zapytanie z parsowanie
		//! \param value [out] Warto�c dla zadanego parametru
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

        //! \return Pe�na odpowied� serwisu webowego w formacie html/xml
        virtual const std::string xmlResponse() = 0;
    };

    typedef boost::shared_ptr<IWSConnection> WSConnectionPtr;
    typedef boost::shared_ptr<const IWSConnection> WSConnectionConstPtr;
}

#endif	//	HEADER_GUARD___IWSCONNECTION_H__
