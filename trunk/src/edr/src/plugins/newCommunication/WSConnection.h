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

//! Interfejs rozszerzaj�cy podstawowe po��czenie webservices o zabezpieczenia
class ISecureWSConnection : public webservices::IWSConnection
{
public:
	//! Destruktor wirtualny
	virtual ~ISecureWSConnection() {}
	//! \param caPath �cie�ka certyfikatu, kt�rym weryfikujemy hosta
	virtual void setCAPath(const std::string & caPath) = 0;
	//! \return �cie�ka certyfikatu, kt�rym weryfikujemy hosta
	virtual const std::string CAPath() const = 0;

	//! \param hostVeryfication Mechanizm weryfikacji hosta po SSL wzgl�dem ceryfikat�w
	virtual void setHostVerification(WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification) = 0;
	//! \return Mechanizm weryfikacji hosta po SSL wzgl�dem ceryfikat�w
	virtual WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification() const = 0;
};

typedef core::shared_ptr<ISecureWSConnection> WSConnectionPtr;

//! Implementacja po��czenia po stronie communication
class WSConnection : public ISecureWSConnection
{
public:

	typedef OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> ScopedLock;

public:
	//! Domyslny konstruktor
	WSConnection();

	//! Wirtualny destruktor
	virtual ~WSConnection() {}

	//! \param caPath �cie�ka certyfikatu, kt�rym weryfikujemy hosta
	virtual void setCAPath(const std::string & caPath);
	//! \return �cie�ka certyfikatu, kt�rym weryfikujemy hosta
	virtual const std::string CAPath() const;

	//! \param hostVeryfication Mechanizm weryfikacji hosta po SSL wzgl�dem ceryfikat�w
	virtual void setHostVerification(WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification);
	//! \return Mechanizm weryfikacji hosta po SSL wzgl�dem ceryfikat�w
	virtual WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification() const;

	//! \param name Nazwa u�ytkownika wywo�uj�cego us�ug�
	virtual void setUser(const std::string & user);
	//! \param password Has�o u�ytkownika wywo�uj�cego us�ug�
	virtual void setPassword(const std::string & password);
	//! \param name Nazwa u�ytkownika wywo�uj�cego us�ug�
	//! \param password Has�o u�ytkownika wywo�uj�cego us�ug�
	virtual void setCredentials(const std::string & user, const std::string & password);

	//! \return Nazwa u�ytkownika kt�ry wywo�uje us�ug�
	virtual const std::string user() const;
	//! \return Has�o u�ytkownika kt�ry wywo�uje us�ug�
	virtual const std::string password() const;

	//! \param url Adres serwisu
	virtual void setUrl(const std::string & url);
	//! \return adres serwisu
	virtual const std::string & url() const;

	//! \param operation Metoda serwisu do wywo�ania
	virtual void setOperation(const std::string & operation);
	//! \param name Nazwa warto�ci do ustawienia
	//! \param value Warto�� zmiennej
	virtual void setValue(const std::string & name, const std::string & value);
	//! Wykonuje operacj� na serwisie webowym
	virtual void invoke(bool process = false);
	//! \param name Nazwa warto�ci kt�r� chcemy pobra�
	//! \return Wska�nik do warto�ci, nullptr je�li nie ma takiej warto�ci, wskaxnik pozostaje pod kontrol� implementacji IWSConnection
	virtual void * getValue(const std::string & name);

	//! \return Pe�na odpowied� serwisu webowego w formacie html/xml
	virtual const std::string xmlResponse();

private:
	//! Wewn�trzne po��czenie realizuj�ce faktycznie wszystkie zapytania
	WSConnectionPtr connection_;

};

#endif	//	HEADER_GUARD___WSCONNECTION_H__
