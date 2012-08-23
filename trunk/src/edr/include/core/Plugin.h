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

#include <boost/version.hpp>
#include <QtCore/QtGlobal>
#include <utils/Macros.h>

#include <core/PluginCommon.h>
#include <core/ILog.h>
#include <core/IPath.h>
#include <core/SmartPtr.h>
#include <core/IService.h> 
#include <core/ISource.h>
#include <core/IParser.h>
#include <core/IVisualizer.h>
#include <core/IDataProcessor.h>
#include <core/IDataSource.h>
#include <core/Export.h>
#include <core/ObjectWrapperFactory.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

//! Deklaracja wersji nag��wka w Pluginie
//! NIEZWYKLE ISTOTNE - przy ka�dej zmianie w Pluginach wersja ta b�dzie si� zmienia�, pluginy nieprzebudowane z nowym nag�owiekm
//! nie b�d� �adowane do aplikacji!!

//! Sami musimy modyfikowa� ta wersj�!!
#define CORE_PLUGIN_INTERFACE_VERSION 5

//! Weryfikacja typu bilda pluginu
#ifdef _DEBUG
    #define CORE_PLUGIN_BUILD_TYPE 0
#else
    #define CORE_PLUGIN_BUILD_TYPE 1
#endif

#ifdef _CPPLIB_VER
#define CORE_CPPLIB_VER _CPPLIB_VER
#else
#define CORE_CPPLIB_VER -1
#endif

//! Nazwa funkcji pobieraj�cej numer wersji pluginu.
#define CORE_GET_PLUGIN_VERSION_FUNCTION_NAME CoreGetPluginInterfaceVersion

#define CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME CoreGetPluginBuildType
//! Nazwa funkcji tworz�cej plugin.
#define CORE_CREATE_PLUGIN_FUNCTION_NAME CoreCreatePluginInstance
//! 
#define CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME CoreGetLibrariesVersions

//! Rozpoczyna rejestracj� pluginu.
//! \param name Nazwa pluginu.
//! \param id ID pluginu.
#define CORE_PLUGIN_BEGIN(name, id)                                     \
CORE_DEFINE_INSTANCE_INFO                                               \
DEFINE_DEFAULT_LOGGER("edr." name)                                      \
extern "C" CORE_EXPORT unsigned CORE_GET_PLUGIN_VERSION_FUNCTION_NAME() \
{                                                                       \
    return CORE_PLUGIN_INTERFACE_VERSION;                               \
}                                                                       \
extern "C" CORE_EXPORT unsigned CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME() \
{                                                                       \
    return CORE_PLUGIN_BUILD_TYPE;                                      \
}                                                                       \
extern "C" CORE_EXPORT void CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME(  \
    int* boostVersion, int* qtVersion, int* stlVersion)                 \
{                                                                       \
    *boostVersion = BOOST_VERSION;                                      \
    *qtVersion = QT_VERSION;                                            \
    *stlVersion = CORE_CPPLIB_VER;                                      \
}                                                                       \
extern "C" CORE_EXPORT core::Plugin* CORE_CREATE_PLUGIN_FUNCTION_NAME(core::InstanceInfo* data) \
{                                                                       \
    core::__instanceInfo = *data;                                       \
    core::Plugin* instance = new core::Plugin((name), (id));            

//! Ko�czy rejestracj� pluginu.
#define CORE_PLUGIN_END                                                 \
    return instance;                                                    \
}

//! Dodaje us�ug� zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_SERVICE(className)                              \
    instance->addService( core::IServicePtr(new className) );

    //! Dodaje �r�d�o danych DM zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_SOURCE(className)                              \
    instance->addSource( core::ISourcePtr(new className) );

//! Dodaje parser zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_PARSER(className)                               \
    instance->addParser( core::IParserPtr(new className) );

//! Dodaje wizualizator zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_VISUALIZER(className)                           \
    instance->addVisualizer( core::IVisualizerPtr(new className) );

//! Dodaje elementu przetwarzaj�cego zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_DATA_PROCESSOR(className)                           \
    instance->addDataProcessor( core::IDataProcessorPtr(new className) );

//! Dodaje �r�d�o zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_DATA_SOURCE(className)                           \
    instance->addDataSource( core::IDataSourcePtr(new className) );

//! Dodanie nowego typu domenowego poprzez utworzenie dla niego ObjectWrapperFactory
#define CORE_PLUGIN_ADD_OBJECT_WRAPPER(className)               \
    instance->addObjectWrapperFactory<className>();


//! Interfejs pluginu przez kt�ry dostarczane s� us�ugi (serwisy) i prototypy element�w przetwarzaj�cych dane
class IPlugin : IIdentifiable
{
public:
    //! Pusty polimorficzny destruktor.
    virtual ~IPlugin() {}
    //! \return Liczba us�ug.
    virtual int getNumServices() const = 0;
    //! \return Us�uga.
    virtual IService* getService(int idx) = 0;
    //! \return Liczba �r�de� danych DM.
    virtual int getNumSources() const = 0;
    //! \return �r�d�o danych DM.
    virtual ISource* getSource(int idx) = 0;
    //! \return Liczba parser�w.
    virtual int getNumParsers() const = 0;
    //! \return Parser.
    virtual IParser* getParser(int idx) = 0;
    //! \return Liczba wizualizator�w.
    virtual int getNumVisualizers() const = 0;
    //! \return Wizualizator.
    virtual IVisualizer* getVisualizer(int idx) = 0;
    //! \return Liczba element�w przetwarzaj�cych.
    virtual int getNumDataProcesors() const = 0;
    //! \return Element przetwarzaj�cy.
    virtual IDataProcessor* getDataProcessor(int idx) = 0;
    //! \return Liczba �r�de� danych workflow.
    virtual int getNumDataSources() const = 0;
    //! \return �r�d�o danych workflow.
    virtual IDataSource* getDataSource(int idx) = 0;
};

/**
 *	Kontener na us�ugi.
 */
class Plugin : IIdentifiable
{
public:
    //! Typ funkcji tworz�cej plugin.
    typedef Plugin* (*CreateFunction)(InstanceInfo* data);
    //! Typ funkcji pobieraj�cej wersj� pluginu.
    typedef int (*GetVersionFunction)();
    //! Typ funkcji pobieraj�cej typ builda pluginu
    typedef int (*GetBuildTypeFunction)();
    //!
    typedef int (*GetLibrariesVersionFunction)(int* boostVersion, int* qtVersion, int* stlVersion);
    //! Typ listy us�ug.
    typedef std::vector<IServicePtr> Services;
    //! Typ listy �r�de� danych DM.
    typedef std::vector<ISourcePtr> Sources;
    //! Typ listy parser�w.
    typedef std::vector<IParserPtr> Parsers;
    //! Typ listy wizualizator�w.
    typedef std::vector<IVisualizerPtr> Visualizers;
    //! Typ listy element�w przetwarzaj�cych.
    typedef std::vector<IDataProcessorPtr> DataProcessors;
    //! Typ listy �r�de� danych workflow.
    typedef std::vector<IDataSourcePtr> DataSources;
    //! Typ listy wrapper�w.
    typedef std::vector<IObjectWrapperFactoryPtr> ObjectWrapperFactories;

private:
    //! Lista us�ug pluginu.
    Services services;
    //! Lista �r�d� danych DM pluginu.
    Sources sources;
    //! Lista parser�w pluginu.
	Parsers parsers;
    //! Lista wizualizator�w pluginu.
    Visualizers visualizers;
    //! Lista element�w przetwarzaj�cych pluginu.
    DataProcessors dataProcessors;
    //! Lista �r�de� danych workflow pluginu.
    DataSources dataSources;
    //! Lista fabryk wrapper�w.
    ObjectWrapperFactories factories;

    //! Nazwa pluginu.
    std::string name;
    //! ID pluginu.
    UniqueID id;
    //! �cie�ka do pluginu.
    std::string path;

public:
    //! \param name Nazwa pluginu.
    Plugin(const std::string& name, UniqueID id) 
        : name(name), path("UNKNOWN"), id(id)
    {
    }

public:
    //! \return ID pluginu.
    virtual UniqueID getID() const
    {
        return id;
    }     

    //! \return nazwa pluginu
    virtual const std::string & getDescription() const
    {
        return name;
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

    //! \service Us�uga do dodania do pluginu.
    void addService(const IServicePtr & service)
    {
        services.push_back(service);
    }
    //! \return Liczba us�ug dostarczanych przez plugin.
    int getNumServices() const
    {
        return static_cast<int>(services.size());
    }
    //! \param i
    const IServicePtr & getService(int i)
    {
        return services[i];
    }

    //! \service �r�d�o DM do dodania do pluginu.
    void addSource(const ISourcePtr & source)
    {
        sources.push_back(source);
    }
    //! \return Liczba �r�de� DM dostarczanych przez plugin.
    int getNumSources() const
    {
        return static_cast<int>(sources.size());
    }
    //! \param i
    const ISourcePtr & getSource(int i)
    {
        return sources[i];
    }
	
    //! \parser Parser do dodania do pluginu.
	void addParser(const IParserPtr & parser)
    {
        this->parsers.push_back(parser);
    }
    //! \return Liczba parser�w dostarczanych przez plugin.
    int getNumParsers() const
    {
		return static_cast<int>(this->parsers.size());
    }
    //! \param i
	const IParserPtr & getParser(int i)
    {
        return this->parsers[i];
    }

    //! \visualizer Visualizer do dodania do pluginu.
    void addVisualizer(const IVisualizerPtr & visualizer)
    {
        this->visualizers.push_back(visualizer);
    }
    //! \return Liczba visualizer�w dostarczanych przez plugin.
    int getNumVisualizers() const
    {
        return static_cast<int>(this->visualizers.size());
    }
    //! \param i
    const IVisualizerPtr & getVisualizer(int i)
    {
        return this->visualizers[i];
    }
    //! \param dataProcessor IDataProcessor do dodania do pluginu.
    void addDataProcessor(const IDataProcessorPtr & dataProcessor)
    {
        this->dataProcessors.push_back(dataProcessor);
    }
    //! \return Liczba element�w przetwarzaj�cych dostarczanych przez plugin.
    int getNumDataProcessors() const
    {
        return static_cast<int>(this->dataProcessors.size());
    }
    //! \param i
    const IDataProcessorPtr & getDataProcessor(int i)
    {
        return this->dataProcessors[i];
    }

    //! \param dataSource �r�d�o danych workflow do dodania do pluginu.
    void addDataSource(const IDataSourcePtr & dataSource)
    {
        this->dataSources.push_back(dataSource);
    }
    //! \return Liczba �r�de� danych workflow dostarczanych przez plugin.
    int getNumDataSources() const
    {
        return static_cast<int>(this->dataSources.size());
    }
    //! \param i
    const IDataSourcePtr & getDataSource(int i)
    {
        return this->dataSources[i];
    }

    //! \param factory
    template<class T>
    void addObjectWrapperFactory(const T * dummy = nullptr/*const IObjectWrapperFactoryPtr & factory*/)
    {
        //this->factories.push_back(factory);
        this->factories.push_back(IObjectWrapperFactoryPtr(new ObjectWrapperFactory<T>()));
    }

	void addObjectWrapperFactory(const IObjectWrapperFactoryPtr & factory)
	{
		this->factories.push_back(factory);
	}

    //! \return
    int getNumWrapperFactories() const
    {
        return static_cast<int>(this->factories.size());
    }
    //! \param i
    const IObjectWrapperFactoryPtr & getWrapperFactory(int i)
    {
        return this->factories[i];
    }
};

//! Definicja wska�nika.
typedef shared_ptr<Plugin> PluginPtr;
typedef shared_ptr<const Plugin> PluginConstPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CORE_PLUGIN_H__
