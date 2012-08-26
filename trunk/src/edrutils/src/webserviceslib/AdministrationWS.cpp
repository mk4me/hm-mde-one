#include <webserviceslib/AdministrationWS.h>

namespace webservices
{

AdministrationWS::AdministrationWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection)
{
	mutex = this;
}

AdministrationWS::~AdministrationWS()
{

}

void AdministrationWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & AdministrationWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & AdministrationWS::connection() const
{
	return constConnection_;
}

void AdministrationWS::defineAttributeGroup(const std::string & groupName, const std::string & entity)
{

}

void AdministrationWS::removeAttributeGroup(const std::string & groupName, const std::string & entity)
{

}

void AdministrationWS::defineAttribute(const std::string & attributeName, const std::string & groupName,
	const std::string & entity, bool isEnum, const std::string & pluginDescriptor,
	const std::string & type, const std::string & unit)
{

}

void AdministrationWS::removeAttribute(const std::string & attributeName, const std::string & groupName,
	const std::string & entity)
{

}

void AdministrationWS::addAttributeEnumValue(const std::string & attributeName, const std::string & groupName,
	const std::string & entity, const std::string & value, bool clearExisting)
{

}

void AdministrationWS::downloadAreaCleanup(int olderThanMinutes)
{

}

}
