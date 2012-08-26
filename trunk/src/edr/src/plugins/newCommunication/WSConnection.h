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

//! Interfejs rozszerzający podstawowe połączenie webservices o zabezpieczenia
class ISecureWSConnection : public webservices::IWSConnection
{
public:
	//! Destruktor wirtualny
	virtual ~ISecureWSConnection() {}
	//! \param caPath Ścieżka certyfikatu, którym weryfikujemy hosta
	virtual void setCAPath(const std::string & caPath) = 0;
	//! \return Ścieżka certyfikatu, którym weryfikujemy hosta
	virtual const std::string CAPath() const = 0;

	//! \param hostVeryfication Mechanizm weryfikacji hosta po SSL względem ceryfikatów
	virtual void setHostVerification(WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification) = 0;
	//! \return Mechanizm weryfikacji hosta po SSL względem ceryfikatów
	virtual WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification() const = 0;
};

typedef core::shared_ptr<ISecureWSConnection> WSConnectionPtr;

//! Implementacja połączenia po stronie communication
class WSConnection : public ISecureWSConnection
{
public:

	typedef OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> ScopedLock;

public:
	//! domyślny konstruktor
	WSConnection();

	//! Wirtualny destruktor
	virtual ~WSConnection() {}

	//! \param caPath Ścieżka certyfikatu, którym weryfikujemy hosta
	virtual void setCAPath(const std::string & caPath);
	//! \return Ścieżka certyfikatu, którym weryfikujemy hosta
	virtual const std::string CAPath() const;

	//! \param hostVeryfication Mechanizm weryfikacji hosta po SSL względem ceryfikatów
	virtual void setHostVerification(WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification);
	//! \return Mechanizm weryfikacji hosta po SSL względem ceryfikatów
	virtual WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification() const;

	//! \param name Nazwa użytkownika wywołującego usługę
	virtual void setUser(const std::string & user);
	//! \param password Hasło użytkownika wywołującego usługę
	virtual void setPassword(const std::string & password);
	//! \param name Nazwa użytkownika wywołującego usługę
	//! \param password Hasło użytkownika wywołującego usługę
	virtual void setCredentials(const std::string & user, const std::string & password);

	//! \return Nazwa użytkownika który wywołuje usługę
	virtual const std::string user() const;
	//! \return Hasło użytkownika który wywołuje usługę
	virtual const std::string password() const;

	//! \param url Adres serwisu
	virtual void setUrl(const std::string & url);
	//! \return adres serwisu
	virtual const std::string & url() const;

	//! \param operation Metoda serwisu do wywołania
	virtual void setOperation(const std::string & operation);
	//! \param name Nazwa wartości do ustawienia
	//! \param value Wartość zmiennej
	virtual void setValue(const std::string & name, const std::string & value);
	//! Wykonuje operację na serwisie webowym
	virtual void invoke(bool process = false);
	//! \param name Nazwa wartości którą chcemy pobrać
	//! \return Wskaźnik do wartości, nullptr jeśli nie ma takiej wartości, wskaxnik pozostaje pod kontrolą implementacji IWSConnection
	virtual void * getValue(const std::string & name);

	//! \return Pełna odpowiedź serwisu webowego w formacie html/xml
	virtual const std::string xmlResponse();

private:
	//! Wewnętrzne połączenie realizujące faktycznie wszystkie zapytania
	WSConnectionPtr connection_;

};

#endif	//	HEADER_GUARD___WSCONNECTION_H__
