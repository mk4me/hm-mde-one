/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/WsdlConnection.h>

using namespace communication;

WsdlConnection::WsdlConnection() 
{
    this->usr = "";
    this->pswd = "";
    this->uri = "";
    /*this->verifyPeer = false;
    this->verifyHost = WsdlPull::WsdlInvoker::CNAny;
    this->caPath = "";
    */
}

WsdlConnection::WsdlConnection(const std::string& uri, const std::string& usr, const std::string& pswd/*, bool verifyPeer, const std::string & caPath, WsdlPull::WsdlInvoker::HostVerification verifyHost*/) 
{
    this->usr = usr;
    this->pswd = pswd;
    this->uri = uri;
    /*this->verifyPeer = verifyPeer;
    this->verifyHost = verifyHost;
    this->caPath = caPath;
    */
}

WsdlConnection::~WsdlConnection() { }

void WsdlConnection::setUri(const std::string& uri) 
{
    this->uri = uri;
}

/*
void WsdlConnection::setSecurity(bool verifyPeer, const std::string & caPath, WsdlPull::WsdlInvoker::HostVerification verifyHost)
{
    this->verifyPeer = verifyPeer;
    this->verifyHost = verifyHost;
    this->caPath = caPath;
}
*/

void WsdlConnection::setUser(const std::string& usr) 
{
    this->usr = usr;
}

void WsdlConnection::setPassword(const std::string& pswd) 
{
    this->pswd = pswd;
}

void WsdlConnection::setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd) 
{
    this->uri = uri;
    this->usr = usr;
    this->pswd = pswd;
}

const std::string& WsdlConnection::getUri() const 
{
    return this->uri;
}

const std::string& WsdlConnection::getUser() const 
{
    return this->usr;
}

const std::string& WsdlConnection::getPassword() const 
{
    return this->pswd;
}

void WsdlConnection::initializeInvoker() 
{
    invoker.reset(new WsdlPull::WsdlInvoker());
    //invoker->setProxy("127.0.0.1", 8888);
    if(this->usr.length() > 0) {
        invoker->setAuth(usr, pswd);
    }
    if(this->uri.length() > 0) {
        if(!invoker->setWSDLUri(uri, std::string()/*, verifyPeer, caPath, verifyHost*/)) {
            throw std::runtime_error(invoker->errors().c_str());
        }
    } else {
        throw std::runtime_error("No specified URI.");
    }
}

void WsdlConnection::setOperation(const std::string& name) 
{
    initializeInvoker();
    if(!invoker->setOperation(name)) {
        throw std::runtime_error("Cannot find operation.");
    }
}

void WsdlConnection::setValue(const std::string& param, const std::string& value)
{
    if(!invoker->setValue(param, value)) {
        throw std::runtime_error("Bad param or value.");
    }
}

void WsdlConnection::invokeOperation() 
{
    if(invoker->status())  {
        //invoke operation
        //invoker->setVerifyPeer(verifyPeer);
        //invoker->setVerifyHost(verifyHost);
        //invoker->setCAPath(caPath);
        invoker->setVerbose(true);
        //invoker->setProxy("127.0.0.1", 8888);
        bool succseed = invoker->invoke(0, false);
        LOG_DEBUG_STATIC_NAMED("wsdlpull", invoker->getXMLResponse().c_str());
        if(!succseed)  {
            throw std::runtime_error(invoker->errors().c_str());
        }
    } else 
    {
        throw std::runtime_error(invoker->errors().c_str());
    }
}

std::string WsdlConnection::getXMLResponse()
{
    return invoker->getXMLResponse();
}
