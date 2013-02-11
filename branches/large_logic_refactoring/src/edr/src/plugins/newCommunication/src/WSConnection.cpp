#include "CommunicationPCH.h"
#include <corelib/ILog.h>
#include <corelib/PluginCommon.h>
#include "WSConnection.h"
#include <wsdlparser/WsdlInvoker.h>
#include <boost/type_traits.hpp>

template<class Invoker>
class _TWSConnection : public ISecureWSConnection
{
public:
	_TWSConnection() : invoker_(new Invoker()), ready(false), exception_(false), resetRequired(false)
	{

	}

	virtual~_TWSConnection()
	{

	}

	//! \param caPath Ścieżka certyfikatu, którym weryfikujemy hosta
	virtual void setCAPath(const std::string & caPath)
	{
		caPath_ = caPath;
	}

	//! \return Ścieżka certyfikatu, którym weryfikujemy hosta
	virtual const std::string CAPath() const
	{
		return caPath_;
	}

	//! \param hostVeryfication Mechanizm weryfikacji hosta po SSL względem ceryfikatów
	virtual void setHostVerification(WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification)
	{
		hostVerification_ = hostVerification;
	}

	//! \return Mechanizm weryfikacji hosta po SSL względem ceryfikatów
	virtual WsdlPull::CustomSSLWsdlInvoker::HostVerification hostVerification() const
	{
		return hostVerification_;
	}

	//! \param name Nazwa użytkownika wywołującego usługę
	virtual void setUser(const std::string & user)
	{
		user_ = user;
	}

	//! \param password Hasło użytkownika wywołującego usługę
	virtual void setPassword(const std::string & password)
	{
		password_ = password;
	}

	//! \param name Nazwa użytkownika wywołującego usługę
	//! \param password Hasło użytkownika wywołującego usługę
	virtual void setCredentials(const std::string & user, const std::string & password)
	{
		user_ = user;
		password_ = password;
	}

	//! \return Nazwa użytkownika który wywołuje usługę
	virtual const std::string user() const
	{
		return user_;
	}

	//! \return Hasło użytkownika który wywołuje usługę
	virtual const std::string password() const
	{
		return password_;
	}

	//! \param url Adres serwisu
	virtual void setUrl(const std::string & url)
	{
		url_ = url;
		if(url != lastUrl){
			resetRequired = true;
		}
	}

	//! \return adres serwisu
	virtual const std::string & url() const
	{
		return url_;
	}

	//! \param operation Metoda serwisu do wywołania
	virtual void setOperation(const std::string & operation)
	{
		operationName_ = operation;
	}

	//! \param name Nazwa wartości do ustawienia
	//! \param value Wartość zmiennej
	virtual void setValue(const std::string & name, const std::string & value)
	{
		values[name] = value;
	}

	//! Wykonuje operację na serwisie webowym
	virtual void invoke(bool process)
	{
		if(resetRequired == true){
			resetRequired = false;
			resetInvoker();
			lastUrl = url_;
		}

		try{
			invoker_->setOperation(operationName_);
		}catch(std::exception & e){
			throw webservices::WSConnectionOperationException(e.what());
		}catch(...){
			throw webservices::WSConnectionOperationException("Unknown connection operation error");
		}

		auto it = values.begin();

		try{
			for( ; it != values.end(); ++it){
				invoker_->setValue(it->first, it->second);
			}

			std::map<std::string, std::string>().swap(values);
		}catch(std::exception & e){
			std::map<std::string, std::string>().swap(values);
			throw webservices::WSConnectionOperationValueException(e.what());
		}catch(...){
			std::map<std::string, std::string>().swap(values);
			std::string str("Unknown connection operation value error for value name: ");
			str += it->first + " with value: " + it->second;
			throw webservices::WSConnectionOperationValueException(str.c_str());
		}

		try{
			exception_ = false;
			s = e = std::string::npos;
			invoker_->setAuth(user_, password_);

			invoker_->invoke(0, process);
		}catch(std::exception & e){
			resetRequired = true;
			throw webservices::WSConnectionInvokeException(e.what());
		}catch(...){
			resetRequired = true;
			throw webservices::WSConnectionInvokeException("Unknown connection invoke error");
		}

		//tutaj weryfikuję czy był jakis wyjątek - jeśli tak to go zgłoszę
		//pobieram pełną odpowiedź
		auto const & resp = invoker_->getXMLResponse();

		s = resp.find("<faultstring", 0);
		if(s != std::string::npos){
			exception_ = true;
			e = invoker_->getXMLResponse().find("</s:Fault>", s + 12);
			if(e == std::string::npos){
				e = invoker_->getXMLResponse().size();
			}

			resetRequired = true;

			if(resp.find("a:InvalidSecurityToken", 0) != std::string::npos){
				throw webservices::WSConnectionSecurityException(resp.substr(s + 11, e-s - 11).c_str());
			}else{
				throw webservices::WSConnectionResponseException(resp.substr(s + 11, e-s - 11).c_str());
			}
		}
	}
	//! \param name Nazwa wartości którą chcemy pobrać
	//! \return Wskaźnik do wartości, nullptr jeśli nie ma takiej wartości, wskaxnik pozostaje pod kontrolą implementacji IWSConnection
	virtual void * getValue(const std::string & name)
	{
		Schema::Type type;
		return invoker_->getValue(name, type);
	}

	//! \return Pełna odpowiedź serwisu webowego w formacie html/xml
	virtual const std::string xmlResponse()
	{
		//musze przetworzyć odpowiedź - chcę w niej mieć tylko to co istotne bez zbędnych nagłówków SOAP i bezpieczeństwa
		std::string ret(invoker_->getXMLResponse());

		if(exception_ != true){

			s = ret.find("<" + operationName_ + "Result", 0);

			if(s != std::string::npos){
				e = ret.find("</" + operationName_ + "Result>", s + 1) + 9 + operationName_.size();
				if(e == std::string::npos){
					e = ret.size();
				}
			}else{
				s = 0;
				e = ret.size();
			}
		}

		return ret.substr(s, e-s);
	}

private:

	void resetInvoker()
	{
		try{
			invoker_.reset(new Invoker());
			invoker_->setVerbose(true);
			setInvoker(boost::is_base_of<WsdlPull::CustomSSLWsdlInvoker, Invoker>());
			PLUGIN_LOG_INFO("Created Invoker " << typeid(Invoker).name());
		}catch(std::exception & e){
			PLUGIN_LOG_INFO("Could not create proper Invoker: " << typeid(Invoker).name() << " with error: " << e.what());
			throw webservices::WSConnectionInitializationException(e.what());
		}catch(...){
			PLUGIN_LOG_INFO("Could not create proper Invoker: " << typeid(Invoker).name() << " with unknown error");
			throw webservices::WSConnectionInitializationException("Unknown connection initialization error");
		}
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
	std::string::size_type s;
	std::string::size_type e;
	bool exception_;
	bool ready;
	std::string operationName_;
	InvokerPtr invoker_;
	std::string user_;
	std::string password_;
	std::string caPath_;
	std::string url_;

	std::map<std::string, std::string> values;

	std::string lastUrl;
	bool resetRequired;
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
	//metoda fabryka dla naszych połączeń!!
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
