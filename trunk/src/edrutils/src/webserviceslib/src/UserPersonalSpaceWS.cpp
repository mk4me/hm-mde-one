#include <webserviceslib/UserPersonalSpaceWS.h>

namespace webservices
{

UserPersonalSpaceWS::UserPersonalSpaceWS(const WSConnectionPtr & connection)
	: WebServiceT<IUserPersonalSpaceWS>(connection)
{
	
}

UserPersonalSpaceWS::~UserPersonalSpaceWS()
{

}

}
