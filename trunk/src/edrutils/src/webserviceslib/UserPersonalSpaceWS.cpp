#include <webserviceslib/UserPersonalSpaceWS.h>

namespace webservices
{

UserPersonalSpaceWS::UserPersonalSpaceWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection)
{
	mutex = this;
}

UserPersonalSpaceWS::~UserPersonalSpaceWS()
{

}

void UserPersonalSpaceWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & UserPersonalSpaceWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & UserPersonalSpaceWS::connection() const
{
	return constConnection_;
}

}