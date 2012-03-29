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

        template<class T>
        void setValue(const std::string& param, const T& value)
        {
            setValue(param, boost::lexical_cast<std::string>(value));
        }

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
