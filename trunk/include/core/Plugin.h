/********************************************************************
	created:  2010/10/04
	created:  4:10:2010   15:30
	filename: Plugin.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CORE_PLUGIN_H__
#define __HEADER_GUARD__CORE_PLUGIN_H__

#include <vector>
#include <string>
#include "IService.h"
#include "BaseDataTypes.h"
#include "Export.h"
#include "IIdentifiable.h"

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

//! Nazwa funkcji tworz¹cej plugin.
#define CORE_CREATE_PLUGIN_FUNCTION_NAME CoreCreatePluginInstance

//! Rozpoczyna rejestracjê pluginu.
//! \param name Nazwa pluginu.
//! \param id ID pluginu.
#define CORE_PLUGIN_BEGIN(name, id)                                     \
extern "C" CORE_EXPORT core::Plugin* CORE_CREATE_PLUGIN_FUNCTION_NAME() \
{                                                                       \
    core::Plugin* instance = new core::Plugin((name), (id));    

//! Koñczy rejestracjê pluginu.
#define CORE_PLUGIN_END                                                 \
    return instance;                                                    \
}

//! Dodaje us³ugê zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_SERVICE(className)                              \
    instance->addService(new className );

/**
 *	Kontener na us³ugi.
 */
class Plugin : IIdentifiable
{
public:
    //! Typ funkcji tworz¹cej plugin.
    typedef core::Plugin* (*CreateFunction)();
    //! Typ listy us³ug.
    typedef std::vector<IService*> Services;
    //!
    typedef Services::iterator iterator;
    //!
    typedef Services::const_iterator const_iterator;

private:
    //! Lista us³ug pluginu.
    Services services;
    //! Nazwa pluginu.
    std::string name;
    //! ID pluginu.
    UniqueID id;
    
public:
    //! \param name Nazwa pluginu.
    Plugin(const std::string& name, UniqueID id) 
        : name(name) 
    {}

public:
    //! \return ID pluginu.
    virtual UniqueID getID() const
    {
        return id;
    }     
    //! \return Nazwa pluginu.
    const std::string& getName() const
    {
        return name;
    }
    //! \service Us³uga do dodania do pluginu.
    void addService(IService* service)
    {
        services.push_back(service);
    }
    //! \return Liczba us³ug dostarczanych przez plugin.
    size_t getNumServices() const
    {
        return services.size();
    }
    //! 
    //! \param i
    IService* getService(size_t i)
    {
        return services[i];
    }
    //! 
    //! \param i
    const IService* getService(size_t i) const
    {
        return services[i];
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CORE_PLUGIN_H__