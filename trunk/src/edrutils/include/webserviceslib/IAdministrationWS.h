/********************************************************************
    created:  2012/02/28
    created:  28:2:2012   22:32
    filename: IAdministrationWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IADMINISTRATIONWS_H__
#define HEADER_GUARD___IADMINISTRATIONWS_H__

#include <webserviceslib/IWebService.h>

namespace webservices
{

//! Interfejs serwisu z operacjami administracyjnymi
class IAdministrationWS : public IWebService
{
public:
    //! Destruktor wirtualny
    virtual ~IAdministrationWS() {}

    virtual void defineAttributeGroup(const std::string & groupName, const std::string & entity) = 0;
    
    virtual void removeAttributeGroup(const std::string & groupName, const std::string & entity) = 0;
     
    virtual void defineAttribute(const std::string & attributeName, const std::string & groupName,
        const std::string & entity, bool isEnum, const std::string & pluginDescriptor,
        const std::string & type, const std::string & unit) = 0;
     
    virtual void removeAttribute(const std::string & attributeName, const std::string & groupName,
        const std::string & entity) = 0;
        
    virtual void addAttributeEnumValue(const std::string & attributeName, const std::string & groupName,
        const std::string & entity, const std::string & value, bool clearExisting) = 0;
        
    virtual void downloadAreaCleanup(int olderThanMinutes) = 0;

};

typedef boost::shared_ptr<IAdministrationWS> AdministrationWSPtr;
typedef boost::shared_ptr<const IAdministrationWS> AdministrationWSConstPtr;

}

#endif	//	HEADER_GUARD___IADMINISTRATIONWS_H__
