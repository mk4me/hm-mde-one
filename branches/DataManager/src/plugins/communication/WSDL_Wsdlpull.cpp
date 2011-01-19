/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/WSDL_Wsdlpull.h>

using namespace communication;

WSDL_Wsdlpull::WSDL_Wsdlpull() 
{
	this->usr = "";
	this->pswd = "";
	this->uri = "";
}

WSDL_Wsdlpull::WSDL_Wsdlpull(const std::string& uri, const std::string& usr, const std::string& pswd) 
{
	this->usr = usr;
	this->pswd = pswd;
	this->uri = uri;
	//if(this->uri[this->uri.size() - 1] != '/') 
	//{
	//	this->uri.append("/");
	//}
}

WSDL_Wsdlpull::~WSDL_Wsdlpull() 
{ }

void WSDL_Wsdlpull::setUri(const std::string& uri) 
{
	this->uri = uri;
	//if(this->uri[this->uri.size() - 1] != '/') 
	//{
	//	this->uri.append("/");
	//}
}

void WSDL_Wsdlpull::setUser(const std::string& usr) 
{
	this->usr = usr;
}

void WSDL_Wsdlpull::setPassword(const std::string& pswd) 
{
	this->pswd = pswd;
}

void WSDL_Wsdlpull::setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd) 
{
	this->uri = uri;
	this->usr = usr;
	this->pswd = pswd;
}

const std::string& WSDL_Wsdlpull::getUri() const 
{
	return this->uri;
}

const std::string& WSDL_Wsdlpull::getUser() const 
{
	return this->usr;
}

const std::string& WSDL_Wsdlpull::getPassword() const 
{
	return this->pswd;
}

void WSDL_Wsdlpull::initializeInvoker() 
{
#ifdef _DEBUG
	invoker.setVerbose(true);
#endif
	if(this->usr.length() > 0)
	{
		invoker.setAuth(usr, pswd);
	}
	if(this->uri.length() > 0)
	{
		if(!invoker.setWSDLUri(uri)) 
		{
			throw std::runtime_error(invoker.errors().c_str());
		}
	}
	else
	{
		throw std::runtime_error("No specified URI.");
	}
}

void WSDL_Wsdlpull::setOperation(const std::string& name) 
{
	initializeInvoker();
	if(!invoker.setOperation(name)) 
	{
		throw std::runtime_error("Cannot find operation.");
	}
}

void WSDL_Wsdlpull::setValue(const std::string& param, const std::string& value)
{
	if(!invoker.setValue(param, value))
	{
		throw std::runtime_error("Bad param or value.");
	}
}

void WSDL_Wsdlpull::invokeOperation() 
{
	if(invoker.status()) 
	{
		//invoke operation
		if(!invoker.invoke()) 
		{
			throw std::runtime_error(invoker.errors().c_str());
			//throw std::string(this->getXMLResponse().c_str());
		}
	}
	else 
	{
		throw std::runtime_error(invoker.errors().c_str());
	}
}

std::string WSDL_Wsdlpull::getXMLResponse()
{
	return invoker.getXMLResponse();
}
