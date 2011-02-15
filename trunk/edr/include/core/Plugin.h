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

#include <core/Log.h>

#include "SmartPtr.h"
#include "IService.h"
#include "IParser.h"
#include "BaseDataTypes.h"
#include "Export.h"
#include "IIdentifiable.h"

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

//! Nazwa funkcji tworz�cej plugin.
#define CORE_CREATE_PLUGIN_FUNCTION_NAME CoreCreatePluginInstance

//! Rozpoczyna rejestracj� pluginu.
//! \param name Nazwa pluginu.
//! \param id ID pluginu.
#define CORE_PLUGIN_BEGIN(name, id)                                     \
DEFINE_DEFAULT_LOGGER("edr." name)                                      \
extern "C" CORE_EXPORT core::Plugin* CORE_CREATE_PLUGIN_FUNCTION_NAME() \
{                                                                       \
    core::Plugin* instance = new core::Plugin((name), (id));    

//! Ko�czy rejestracj� pluginu.
#define CORE_PLUGIN_END                                                 \
    return instance;                                                    \
}

//! Dodaje us�ug� zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_SERVICE(className)                              \
    instance->addService( IServicePtr(new className) );

//! Dodaje parser zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_PARSER(className)                              \
    instance->addParser( core::IParserPtr(new className) );

/**
 *	Kontener na us�ugi.
 */
class Plugin : IIdentifiable
{
public:
    //! Typ funkcji tworz�cej plugin.
    typedef core::Plugin* (*CreateFunction)();
    //! Typ listy us�ug.
    typedef std::vector<IServicePtr> Services;
    //!
    typedef Services::iterator iterator;
    //!
    typedef Services::const_iterator const_iterator;

    //! Typ listy parser�w.
    typedef std::vector<IParserPtr> Parsers;

private:
    //! Lista us�ug pluginu.
    Services services;

    //! Lista parser�w pluginu.
	Parsers parsers;

    //! Nazwa pluginu.
    std::string name;
    //! ID pluginu.
    UniqueID id;
    //! �cie�ka do pluginu.
    std::string path;

public:
    //! \param name Nazwa pluginu.
    Plugin(const std::string& name, UniqueID id) 
        : name(name), path("UNKNOWN")
    {
    }

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
    //! \service Us�uga do dodania do pluginu.
    void addService(IServicePtr service)
    {
        services.push_back(service);
    }
    //! \return Liczba us�ug dostarczanych przez plugin.
    size_t getNumServices() const
    {
        return services.size();
    }
    //! 
    //! \param i
    IServicePtr getService(size_t i)
    {
        return services[i];
    }
    //! 
    //! \param i
    IServiceConstPtr getService(size_t i) const
    {
        return services[i];
    }
    //! \return
    const std::string& getPath() const
    { 
        return path;
    }
    //! \param path
    void setPath(const std::string& path) 
    { 
        this->path = path; 
    }
	
    //! \parser Parser do dodania do pluginu.
	void addParser(IParserPtr parser)
    {
        this->parsers.push_back(parser);
    }
    //! \return Liczba parser�w dostarczanych przez plugin.
    size_t getNumParsers() const
    {
		return this->parsers.size();
    }
    //! 
    //! \param i
	IParserPtr getParser(size_t i)
    {
        return this->parsers[i];
    }
    //! 
    //! \param i
	IParserConstPtr getParser(size_t i) const
    {
        return this->parsers[i];
    }
};

//! Definicja wska�nika.
typedef shared_ptr<Plugin> PluginPtr;
typedef core::shared_ptr<const Plugin> PluginConstPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CORE_PLUGIN_H__