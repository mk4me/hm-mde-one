/********************************************************************
    created:  2012/03/02
    created:  2:3:2012   14:30
    filename: AdministrationWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ADMINISTRATIONWS_H__
#define HEADER_GUARD___ADMINISTRATIONWS_H__

#include <webserviceslib/IAdministrationWS.h>

namespace webservices
{
    class AdministrationWS : public WebServiceT<IAdministrationWS>
    {
    public:

		AdministrationWS(const WSConnectionPtr & connection = WSConnectionPtr());
		virtual ~AdministrationWS();

        virtual void defineAttributeGroup(const std::string & groupName, const std::string & entity);

        virtual void removeAttributeGroup(const std::string & groupName, const std::string & entity);

        virtual void defineAttribute(const std::string & attributeName, const std::string & groupName,
            const std::string & entity, const bool isEnum, const std::string & pluginDescriptor,
            const std::string & type, const std::string & unit);

        virtual void removeAttribute(const std::string & attributeName, const std::string & groupName,
            const std::string & entity);

        virtual void addAttributeEnumValue(const std::string & attributeName, const std::string & groupName,
            const std::string & entity, const std::string & value, const bool clearExisting);

        virtual void downloadAreaCleanup(const int olderThanMinutes);
    };
}

#endif	//	HEADER_GUARD___ADMINISTRATIONWS_H__
