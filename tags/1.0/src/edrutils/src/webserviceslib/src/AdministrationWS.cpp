#include <webserviceslib/AdministrationWS.h>

namespace webservices
{

AdministrationWS::AdministrationWS(const WSConnectionPtr & connection)
	: WebServiceT<IAdministrationWS>(connection)
{
	
}

AdministrationWS::~AdministrationWS()
{

}

void AdministrationWS::defineAttributeGroup(const std::string & groupName,
	const std::string & entity)
{
	connection()->setOperation("DefineAttributeGroup");
	connection()->setValue("groupName", groupName);
	connection()->setValue("entity", entity);	
	connection()->invoke();	
}

void AdministrationWS::removeAttributeGroup(const std::string & groupName,
	const std::string & entity)
{
	connection()->setOperation("RemoveAttributeGroup");
	connection()->setValue("groupName", groupName);
	connection()->setValue("entity", entity);	
	connection()->invoke();	
}

void AdministrationWS::defineAttribute(const std::string & attributeName,
	const std::string & groupName, const std::string & entity, bool isEnum,
	const std::string & pluginDescriptor, const std::string & type,
	const std::string & unit)
{
	connection()->setOperation("DefineAttribute");
	connection()->setValue("attributeName", attributeName);
	connection()->setValue("groupName", groupName);
	connection()->setValue("entity", entity);
	connection()->setValue("isEnum", isEnum);
	connection()->setValue("pluginDescriptor", pluginDescriptor);
	connection()->setValue("type", type);
	connection()->setValue("unit", unit);
	connection()->invoke();
}

void AdministrationWS::removeAttribute(const std::string & attributeName,
	const std::string & groupName, const std::string & entity)
{
	connection()->setOperation("RemoveAttribute");
	connection()->setValue("attributeName", attributeName);
	connection()->setValue("entity", entity);
	connection()->invoke();	
}

void AdministrationWS::addAttributeEnumValue(const std::string & attributeName,
	const std::string & groupName, const std::string & entity,
	const std::string & value, const bool clearExisting)
{
	connection()->setOperation("AddAttributeEnumValue");
	connection()->setValue("attributeName", attributeName);
	connection()->setValue("groupName", groupName);
	connection()->setValue("entity", entity);
	connection()->setValue("value", value);
	connection()->setValue("clearExisting", clearExisting);
	connection()->invoke();	
}

void AdministrationWS::downloadAreaCleanup(const int olderThanMinutes)
{
	connection()->setOperation("DownloadAreaCleanup");
	connection()->setValue("olderThanMinutes", olderThanMinutes);
	connection()->invoke();	
}

}
