/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/WSDL_Wsdlpull.h>

using namespace communication;

const std::string WSDL_Wsdlpull::wsdl_domain = "db-bdr";
const std::string WSDL_Wsdlpull::wsdl_usr = "applet_user";
const std::string WSDL_Wsdlpull::wsdl_pswd = "aplet4Motion";

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

void WSDL_Wsdlpull::displayOperations() 
{
	initializeInvoker();
	if(invoker.status()) 
	{
		std::vector<std::string> operations;
		invoker.getOperations(operations);
		std::cout << "Operations number: " << operations.size() << std::endl;
		std::cout << "Input headers: " << invoker.nInputHeaders() << std::endl;
		for(unsigned int i = 0; i < operations.size(); i++) 
		{
			std::cout << i + 1 << ". " << operations[i] << std::endl;
		}
	}
	else 
	{
		throw std::runtime_error(invoker.errors().c_str());
	}
}

void WSDL_Wsdlpull::displayPortTypes() 
{
	initializeInvoker();
	if(invoker.status()) 
	{
		WsdlPull::WsdlParser wp(uri, std::cout);
		while(wp.getEventType() != WsdlPull::WsdlParser::END) 
		{
			if(wp.getNextElement() == WsdlPull::WsdlParser::PORT_TYPE) 
			{
				const WsdlPull::PortType* p = wp.getPortType();
				std::cout << "Port Type: " << p->getName() << " has " << p->getNumOps() << " operations" << std::endl;
				WsdlPull::Operation::cOpIterator from, to;
				p->getOperations(from, to);
				while(from != to) 
				{
					const WsdlPull::Message* in = (*from)->getMessage(WsdlPull::Input); 
					const WsdlPull::Message* out = (*from)->getMessage(WsdlPull::Output); 
					WsdlPull::MessageList* faults = (*from)->getFaults();
					std::cout << (*from)->getName() << std::endl;
					std::cout << "\tInput Message: " << in->getName() << std::endl;
					if(out) 
					{
						std::cout << "\tOutput Message: " << out->getName() << std::endl;
					}
					if(faults) 
					{
						for(WsdlPull::MessageList::iterator mli = faults->begin(); mli != faults->end(); mli++) 
						{
							std::cout << "\tFault: " << (*mli)->getName() << std::endl;
						}
					}
					from++;
				}
			}
		}
	}
	else 
	{
		throw std::runtime_error(invoker.errors().c_str());
	}
}
