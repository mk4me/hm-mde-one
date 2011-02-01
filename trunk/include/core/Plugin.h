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

#include "SmartPtr.h"
#include "IService.h"
#include "IParser.h"
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
    instance->addService( IServicePtr(new className) );

//! Dodaje parser zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_PARSER(className)                              \
	instance->addParser( IParserPtr(new className) );

/**
 *	Kontener na us³ugi.
 */
class Plugin : IIdentifiable
{
public:
    //! Typ funkcji tworz¹cej plugin.
    typedef core::Plugin* (*CreateFunction)();
    //! Typ listy us³ug.
    typedef std::vector<IServicePtr> Services;
    //!
    typedef Services::iterator iterator;
    //!
    typedef Services::const_iterator const_iterator;

    //! Typ listy parserów.
    typedef std::vector<IParserPtr> Parsers;

private:
    //! Lista us³ug pluginu.
    Services services;

    //! Lista parserów pluginu.
	Parsers parsers;

    //! Nazwa pluginu.
    std::string name;
    //! ID pluginu.
    UniqueID id;
    //! Œcie¿ka do pluginu.
    std::string path;

public:
    //! \param name Nazwa pluginu.
    Plugin(const std::string& name, UniqueID id) 
        : name(name), path("UNKNOWN")
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
    void addService(IServicePtr service)
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
        parsers.push_back(parser);
    }
    //! \return Liczba parserów dostarczanych przez plugin.
    size_t getNumParsers() const
    {
		return parsers.size();
    }
    //! 
    //! \param i
	IParserPtr getParser(size_t i)
    {
        return parsers[i];
    }
    //! 
    //! \param i
	IParserConstPtr getParser(size_t i) const
    {
        return parsers[i];
    }
};

//! Definicja wskaŸnika.
typedef CORE_SHARED_PTR(Plugin) PluginPtr;
typedef CORE_CONST_SHARED_PTR(Plugin) PluginConstPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CORE_PLUGIN_H__