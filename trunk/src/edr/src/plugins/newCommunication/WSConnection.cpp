#include "CommunicationPCH.h"
#include <plugins/newCommunication/WSConnection.h>
#include <wsdlparser/WsdlInvoker.h>
#include <boost/type_traits.hpp>

template<class Invoker>
class _TWSConnection : public ISecureWSConnection
{
public:
	_TWSConnection() : invoker_(new Invoker())
	{

	}

	virtual~_TWSConnection()
	{

	}

	//! \param caPath �cie�ka certyfikatu, kt�rym weryfikujemy hosta
	virtual void setCAPath(const std::string & caPath)
	{
		caPath_ = caPath;
	}

	//! \return �cie�ka certyfikatu, kt�rym weryfikujemy hosta
	virtual const std::string CAPath() const
	{
		return caPath_;
	}

	//! \param hostVeryfication Mechanizm weryfikacji hosta po SSL wzgl�dem ceryfikat�w
	virtual void setHostVerification(WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification)
	{
		hostVerification_ = hostVerification;
	}

	//! \return Mechanizm weryfikacji hosta po SSL wzgl�dem ceryfikat�w
	virtual WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification() const
	{
		return hostVerification_;
	}

	//! \param name Nazwa u�ytkownika wywo�uj�cego us�ug�
	virtual void setUser(const std::string & user)
	{
		user_ = user;
	}

	//! \param password Has�o u�ytkownika wywo�uj�cego us�ug�
	virtual void setPassword(const std::string & password)
	{
		password_ = password;
	}

	//! \param name Nazwa u�ytkownika wywo�uj�cego us�ug�
	//! \param password Has�o u�ytkownika wywo�uj�cego us�ug�
	virtual void setCredentials(const std::string & user, const std::string & password)
	{
		user_ = user;
		password_ = password;
	}

	//! \return Nazwa u�ytkownika kt�ry wywo�uje us�ug�
	virtual const std::string user() const
	{
		return user_;
	}

	//! \return Has�o u�ytkownika kt�ry wywo�uje us�ug�
	virtual const std::string password() const
	{
		return password_;
	}

	//! \param url Adres serwisu
	virtual void setUrl(const std::string & url)
	{
		url_ = url;
		resetInvoker();
	}

	//! \return adres serwisu
	virtual const std::string & url() const
	{
		return url_;
	}

	//! \param operation Metoda serwisu do wywo�ania
	virtual void setOperation(const std::string & operation)
	{
		operationName_ = operation;
		invoker_->setOperation(operation);
	}

	//! \param name Nazwa warto�ci do ustawienia
	//! \param value Warto�� zmiennej
	virtual void setValue(const std::string & name, const std::string & value)
	{
		invoker_->setValue(name, value);
	}

	//! Wykonuje operacj� na serwisie webowym
	virtual void invoke(bool process)
	{
		invoker_->setAuth(user_, password_);
		invoker_->invoke(0, process);
	}
	//! \param name Nazwa warto�ci kt�r� chcemy pobra�
	//! \return Wska�nik do warto�ci, nullptr je�li nie ma takiej warto�ci, wskaxnik pozostaje pod kontrol� implementacji IWSConnection
	virtual void * getValue(const std::string & name)
	{
		Schema::Type type;
		return invoker_->getValue(name, type);
	}

	//! \return Pe�na odpowied� serwisu webowego w formacie html/xml
	virtual const std::string xmlResponse()
	{
		//musze przetworzy� odpowied� - chc� w niej mie� tylko to co istotne bez zb�dnych nag��wk�w SOAP i bezpiecze�stwa
		std::string ret(invoker_->getXMLResponse());

		auto s = ret.find("<" + operationName_ + "Result", 0);
		auto e = s;
		if(s == std::string::npos){

			s = ret.find("<faultstring", 0);

			if(s == std::string::npos){
				s = 0;
				e = ret.size();
			}else{

				e = ret.find("</s:Fault>", s + 12);

			}

		}else{

			e = ret.find("</" + operationName_ + "Result>", s + 1) + 9 + operationName_.size();

		}

		try{
			ret = ret.substr(s, e-s);
		}catch(...){

		}

		return ret;
	}

private:

	void resetInvoker()
	{
		invoker_.reset(new Invoker());
		setInvoker(boost::is_base_of<WsdlPull::CustomSSLWsdlInvoker, Invoker>());
	}

	void setInvoker(boost::true_type)
	{
		invoker_->setWSDLUri(url_, caPath_, hostVerification_);
	}

	void setInvoker(boost::false_type)
	{
		invoker_->setWSDLUri(url_);
	}

private:

	typedef core::shared_ptr<Invoker> InvokerPtr;

private:
	std::string operationName_;
	InvokerPtr invoker_;
	std::string user_;
	std::string password_;
	std::string url_;
	std::string caPath_;
	WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification_;
};


WSConnection::WSConnection() : connection_(new _TWSConnection<WsdlPull::WsdlInvoker>())
{

}

void WSConnection::setCAPath(const std::string & caPath)
{
	connection_->setCAPath(caPath);
}

const std::string WSConnection::CAPath() const
{
	return connection_->CAPath();
}

void WSConnection::setHostVerification(WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification)
{
	connection_->setHostVerification(hostVerification);
}

WsdlPull::CustomSSLWsdlInvoker::HostVerification WSConnection::hostVerification() const
{
	return connection_->hostVerification();
}

void WSConnection::setUser(const std::string & user)
{
	connection_->setUser(user);
}

void WSConnection::setPassword(const std::string & password)
{
	connection_->setPassword(password);
}

void WSConnection::setCredentials(const std::string & user, const std::string & password)
{
	connection_->setCredentials(user, password);
}

const std::string WSConnection::user() const
{
	return connection_->user();
}

const std::string WSConnection::password() const
{
	return connection_->password();
}

void WSConnection::setUrl(const std::string & url)
{
	//metoda fabryka dla naszych po��cze�!!
	//automatycznie kopiuje wszystkie ustawienia

	auto pos = url.find("https");

	if(pos != std::string::npos && pos != connection_->url().find("https")){
		core::shared_ptr<ISecureWSConnection> newConnection(new _TWSConnection<WsdlPull::CustomSSLWsdlInvoker>());
		newConnection->setCredentials(connection_->user(), connection_->password());
		newConnection->setCAPath(connection_->CAPath());
		newConnection->setHostVerification(connection_->hostVerification());
		connection_.swap(newConnection);
	}

	connection_->setUrl(url);
}

const std::string & WSConnection::url() const
{
	return connection_->url();
}

void WSConnection::setOperation(const std::string & operation)
{
	connection_->setOperation(operation);
}

void WSConnection::setValue(const std::string & name, const std::string & value)
{
	connection_->setValue(name, value);
}

void WSConnection::invoke(bool process)
{
	connection_->invoke(process);
}

void * WSConnection::getValue(const std::string & name)
{
	return connection_->getValue(name);
}

const std::string WSConnection::xmlResponse()
{
	return connection_->xmlResponse();
}