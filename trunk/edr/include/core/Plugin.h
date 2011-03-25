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
#include <boost/range.hpp>
#include <core/Log.h>

#include "SmartPtr.h"
#include "IService.h"
#include "IParser.h"
#include "IVisualizer.h"
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
DEFINE_DEFAULT_LOGGER("edr." name)                                      \
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
#define CORE_PLUGIN_ADD_PARSER(className)                               \
    instance->addParser( core::IParserPtr(new className) );

//! Dodaje wizualizator zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_VISUALIZER(className)                           \
    instance->addVisualizer( core::IVisualizerPtr(new className) );


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
    //! Typ listy parserów.
    typedef std::vector<IParserPtr> Parsers;
    //! Typ listy wizualizatorów.
    typedef std::vector<IVisualizerPtr> Visualizers;

private:
    //! Lista us³ug pluginu.
    Services services;
    //! Lista parserów pluginu.
	Parsers parsers;
    //! Lista wizualizatorów pluginu.
    Visualizers visualizers;

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

    //! \service Us³uga do dodania do pluginu.
    void addService(IServicePtr service)
    {
        services.push_back(service);
    }
    //! \return Liczba us³ug dostarczanych przez plugin.
    int getNumServices() const
    {
        return static_cast<int>(services.size());
    }
    //! \param i
    IServicePtr getService(int i)
    {
        return services[i];
    }
	
    //! \parser Parser do dodania do pluginu.
	void addParser(IParserPtr parser)
    {
        this->parsers.push_back(parser);
    }
    //! \return Liczba parserów dostarczanych przez plugin.
    int getNumParsers() const
    {
		return static_cast<int>(this->parsers.size());
    }
    //! \param i
	IParserPtr getParser(int i)
    {
        return this->parsers[i];
    }

    //! \visualizer Visualizer do dodania do pluginu.
    void addVisualizer(IVisualizerPtr visualizer)
    {
        this->visualizers.push_back(visualizer);
    }
    //! \return Liczba visualizerów dostarczanych przez plugin.
    int getNumVisualizers() const
    {
        return static_cast<int>(this->visualizers.size());
    }
    //! \param i
    IVisualizerPtr getVisualizer(int i)
    {
        return this->visualizers[i];
    }
};

//! Definicja wskaŸnika.
typedef shared_ptr<Plugin> PluginPtr;
typedef core::shared_ptr<const Plugin> PluginConstPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CORE_PLUGIN_H__