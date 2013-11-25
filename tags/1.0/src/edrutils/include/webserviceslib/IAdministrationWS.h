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

	//! \param groupName Nazwa grupy atrybut�w
	//! \param entity Typ encji
    virtual void defineAttributeGroup(const std::string & groupName, const std::string & entity) = 0;
    
	//! \param groupName Nazwa grupy
	//! \param entity Typ encji
    virtual void removeAttributeGroup(const std::string & groupName, const std::string & entity) = 0;
    
	//! TODO - weryfikacja atrybut�w i ich znaczenia!!
	//! \param attributName Nazwa atrybutu
	//! \param groupName Nazwa grupy atrybut�w
	//! \param Typ encji
	//! \param isEnum Czy jest to atrybut wyliczeniowy
	//! \param pluginDescriptor Opis pluginu
	//! \param type Typ atrybutu
	//! \param unit Jednostka
    virtual void defineAttribute(const std::string & attributeName, const std::string & groupName,
        const std::string & entity, const bool isEnum, const std::string & pluginDescriptor,
        const std::string & type, const std::string & unit) = 0;
    
	//! \param attributName Nazwa atrybutu
	//! \param groupName Nazwa grupy
	//! \param entity Typ encji
    virtual void removeAttribute(const std::string & attributeName, const std::string & groupName,
        const std::string & entity) = 0;

	//! \param attributeName Nazwa atrybutu
	//! \param groupName Nazwa grupy
	//! \param entity Typ encji
	//! \param value Warto�� atrybutu
	//! \param clearExisting Czy ma usuwa� warto�� ju� istniej�c�?
    virtual void addAttributeEnumValue(const std::string & attributeName, const std::string & groupName,
        const std::string & entity, const std::string & value, const bool clearExisting) = 0;
        
	//! \param olderThanMinutes Czas w minutach dla kt�rego usuwa si� pliki z obszaru �ci�gania po stronie bazy danych
    virtual void downloadAreaCleanup(const int olderThanMinutes) = 0;

};

typedef boost::shared_ptr<IAdministrationWS> AdministrationWSPtr;
typedef boost::shared_ptr<const IAdministrationWS> AdministrationWSConstPtr;

}

#endif	//	HEADER_GUARD___IADMINISTRATIONWS_H__
