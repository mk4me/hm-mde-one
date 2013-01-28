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

#include <corelib/PluginCommon.h>
#include <corelib/ILog.h>
#include <corelib/IPath.h>
#include <corelib/SmartPtr.h>
#include <corelib/IService.h> 
#include <corelib/ISource.h>
#include <corelib/IParser.h>
#include <corelib/IVisualizer.h>
#include <utils/Export.h>

////////////////////////////////////////////////////////////////////////////////
namespace plugin {
////////////////////////////////////////////////////////////////////////////////

//! Deklaracja wersji nagłówka w Pluginie
//! NIEZWYKLE ISTOTNE - przy każdej zmianie w Pluginach wersja ta będzie się zmieniać, pluginy nieprzebudowane z nowym nagłowiekm
//! nie będą ładowane do aplikacji!!

//! Sami musimy modyfikować ta wersję!!
#define CORE_PLUGIN_INTERFACE_VERSION 6

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

//! Nazwa funkcji pobierającej numer wersji pluginu.
#define CORE_GET_PLUGIN_INTERFACE_VERSION_FUNCTION_NAME CoreGetPluginInterfaceVersion

#define CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME CoreGetPluginBuildType
//! Nazwa funkcji tworzącej plugin.
#define CORE_FILL_PLUGIN_FUNCTION_NAME CoreCreatePluginInstance
//! 
#define CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME CoreGetLibrariesVersions
//!
#define CORE_SET_PLUGIN_ID_FUNCTION_NAME CoreSetPluginIDName

//! Rozpoczyna rejestrację pluginu.
//! \param name Nazwa pluginu.
//! \param id ID pluginu.
#define CORE_PLUGIN_BEGIN(name, id)                                     \
PLUGIN_DEFINE_CORE_APPLICATION_ACCESSOR                                 \
extern "C" UTILS_EXPORT unsigned CORE_GET_PLUGIN_INTERFACE_VERSION_FUNCTION_NAME() \
{                                                                       \
    return CORE_PLUGIN_INTERFACE_VERSION;                               \
}                                                                       \
extern "C" UTILS_EXPORT unsigned CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME() \
{                                                                       \
    return CORE_PLUGIN_BUILD_TYPE;                                      \
}                                                                       \
extern "C" UTILS_EXPORT void CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME(  \
    int* boostVersion, int* qtVersion, int* stlVersion)                 \
{                                                                       \
    *boostVersion = BOOST_VERSION;                                      \
    *qtVersion = QT_VERSION;                                            \
    *stlVersion = CORE_CPPLIB_VER;                                      \
}                                                                       \
extern "C" UTILS_EXPORT void CORE_SET_PLUGIN_ID_FUNCTION_NAME(core::IPlugin * plugin) {	\
	plugin->setName(name);												\
	plugin->setID(id);													\
}																		\
extern "C" UTILS_EXPORT void CORE_FILL_PLUGIN_FUNCTION_NAME(core::IPlugin * plugin, \
	core::IApplication* coreApplication) \
{                                                                       \
    plugin::__coreApplication = coreApplication;                                       \

//! Kończy rejestrację pluginu.
#define CORE_PLUGIN_END                                                 \
}

//! Dodaje usługę zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_SERVICE(className)                              \
    plugin->addService( plugin::IServicePtr(new className) );

    //! Dodaje źródło danych DM zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_SOURCE(className)                              \
    plugin->addSource( plugin::ISourcePtr(new className) );

//! Dodaje parser zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_PARSER(className)                               \
    plugin->addParser( plugin::IParserPtr(new className) );

//! Dodaje wizualizator zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_VISUALIZER(className)                           \
    plugin->addVisualizer( plugin::IVisualizerPtr(new className) );

//! Dodanie nowego typu domenowego poprzez utworzenie dla niego ObjectWrapperFactory
#define CORE_PLUGIN_ADD_OBJECT_WRAPPER(className)               \
    plugin->addObjectWrapperPrototype<className>();


//! Interfejs pluginu przez który dostarczane są usługi (serwisy) i prototypy elementów przetwarzających dane
class IPlugin : public IIdentifiable, public IDescription
{
public:
	//! \param name Nazwa pluginu
	virtual void setName(const std::string & name) = 0;
	//! \param  description Opis pluginu
	virtual void setDescription(const std::string & description) = 0;
	//! \param id Identyfikator pluginu
	virtual void setID(core::UniqueID id) = 0;

	//! \param service Serwis
	virtual void addService(plugin::IServicePtr service) = 0;
	//! \param source Źródło
	virtual void addSource(plugin::ISourcePtr source) = 0;
	//! \param visualizerPrototype Prototyp wizualizatora
	virtual void addVisualizerPrototype(plugin::IVisualizerPtr visualizerPrototype) = 0;
	//! \param parserPrototype Prototyp parsera
	virtual void addParserPrototype(plugin::IParserPtr parserPrototype) = 0;
	//! \param objectWrapperPrototype Prototyp obiektu domenowego
	virtual void addObjectWrapperPrototype(core::ObjectWrapperPtr objectWrapperPrototype) = 0;

	//! \param factory
	template<class T>
	void addObjectWrapperPrototype(const T * dummy = nullptr)
	{
		addObjectWrapperPrototype(core::ObjectWrapper::create<T>());
	}
};

////////////////////////////////////////////////////////////////////////////////
} // namespace plugin
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CORE_PLUGIN_H__
