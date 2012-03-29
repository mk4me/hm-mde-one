/********************************************************************
    created:  2012/03/06
    created:  6:3:2012   9:11
    filename: WSConnection.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___WSCONNECTION_H__
#define HEADER_GUARD___WSCONNECTION_H__

#include <webserviceslib/IWSConnection.h>
#include <wsdlparser/WsdlInvoker.h>
#include <core/SmartPtr.h>

//! Interfejs rozszerzaj¹cy podstawowe po³¹czenie webservices o zabezpieczenia
class ISecureWSConnection : public webservices::IWSConnection
{
public:
	//! Destruktor wirtualny
	virtual ~ISecureWSConnection() {}
	//! \param caPath Œcie¿ka certyfikatu, którym weryfikujemy hosta
	virtual void setCAPath(const std::string & caPath) = 0;
	//! \return Œcie¿ka certyfikatu, którym weryfikujemy hosta
	virtual const std::string CAPath() const = 0;

	//! \param hostVeryfication Mechanizm weryfikacji hosta po SSL wzglêdem ceryfikatów
	virtual void setHostVerification(WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification) = 0;
	//! \return Mechanizm weryfikacji hosta po SSL wzglêdem ceryfikatów
	virtual WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification() const = 0;
};

typedef core::shared_ptr<ISecureWSConnection> WSConnectionPtr;

//! Implementacja po³¹czenia po stronie communication
class WSConnection : public ISecureWSConnection
{
public:

	typedef OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> ScopedLock;

public:
	//! Domyslny konstruktor
	WSConnection();

	//! Wirtualny destruktor
	virtual ~WSConnection() {}

	//! \param caPath Œcie¿ka certyfikatu, którym weryfikujemy hosta
	virtual void setCAPath(const std::string & caPath);
	//! \return Œcie¿ka certyfikatu, którym weryfikujemy hosta
	virtual const std::string CAPath() const;

	//! \param hostVeryfication Mechanizm weryfikacji hosta po SSL wzglêdem ceryfikatów
	virtual void setHostVerification(WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification);
	//! \return Mechanizm weryfikacji hosta po SSL wzglêdem ceryfikatów
	virtual WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification() const;

	//! \param name Nazwa u¿ytkownika wywo³uj¹cego us³ugê
	virtual void setUser(const std::string & user);
	//! \param password Has³o u¿ytkownika wywo³uj¹cego us³ugê
	virtual void setPassword(const std::string & password);
	//! \param name Nazwa u¿ytkownika wywo³uj¹cego us³ugê
	//! \param password Has³o u¿ytkownika wywo³uj¹cego us³ugê
	virtual void setCredentials(const std::string & user, const std::string & password);

	//! \return Nazwa u¿ytkownika który wywo³uje us³ugê
	virtual const std::string user() const;
	//! \return Has³o u¿ytkownika który wywo³uje us³ugê
	virtual const std::string password() const;

	//! \param url Adres serwisu
	virtual void setUrl(const std::string & url);
	//! \return adres serwisu
	virtual const std::string & url() const;

	//! \param operation Metoda serwisu do wywo³ania
	virtual void setOperation(const std::string & operation);
	//! \param name Nazwa wartoœci do ustawienia
	//! \param value Wartoœæ zmiennej
	virtual void setValue(const std::string & name, const std::string & value);
	//! Wykonuje operacjê na serwisie webowym
	virtual void invoke(bool process = false);
	//! \param name Nazwa wartoœci któr¹ chcemy pobraæ
	//! \return WskaŸnik do wartoœci, nullptr jeœli nie ma takiej wartoœci, wskaxnik pozostaje pod kontrol¹ implementacji IWSConnection
	virtual void * getValue(const std::string & name);

	//! \return Pe³na odpowiedŸ serwisu webowego w formacie html/xml
	virtual const std::string xmlResponse();

private:
	//! Wewnêtrzne po³¹czenie realizuj¹ce faktycznie wszystkie zapytania
	WSConnectionPtr connection_;

};

#endif	//	HEADER_GUARD___WSCONNECTION_H__
