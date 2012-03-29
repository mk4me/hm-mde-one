#include <webserviceslib/AuthorizationWS.h>
#include <utils/Debug.h>
#include <tinyxml.h>

namespace webservices
{

AuthorizationWS::AuthorizationWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection)
{
	mutex = this;	
}

AuthorizationWS::~AuthorizationWS()
{

}

void AuthorizationWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & AuthorizationWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & AuthorizationWS::connection() const
{
	return constConnection_;
}

const bool AuthorizationWS::checkMyLogin() const
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lock_(*connection_);
	connection_->setOperation("CheckMyLogin");
	connection_->invoke(true);

	bool ret = false;

	try{
		connection_->getValue<bool>("CheckMyLoginResult", ret);
	}catch(...){

	}

	//

	//
	////muszê pobraæ listê u¿ytkowników i sprawdziæ czy ktoœ tam jest
	//TiXmlDocument document;

	//if(document.Parse(connection_->xmlResponse().c_str(), nullptr, TiXmlEncoding::TIXML_ENCODING_UTF8)) {
	//	UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	//}else{

	//	TiXmlHandle hDocument(&document);
	//	TiXmlElement* _element;
	//	TiXmlHandle hParent(0);

	//	_element = hDocument.FirstChildElement().Element();
	//	if(!_element) {
	//		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	//	}

	//	if(std::string(_element->GetText()) == "true"){
	//		ret = true;
	//	}
	//}

	return ret;
}

void AuthorizationWS::createUserAccount(const std::string & name, const std::string & surname)
{

}

void AuthorizationWS::listUsers() const
{

}

void AuthorizationWS::listSessionPrivileges(int sessionID) const
{

}

void AuthorizationWS::grantSessionPrivileges(const std::string & grantedUserLogin, int sessionID, bool write)
{

}

void AuthorizationWS::removeSessionPrivileges(const std::string & grantedUserLogin, int sessionID)
{

}

void AuthorizationWS::alterSessionVisibility(int sessionID, bool isPublic, bool isWritable)
{

}

}