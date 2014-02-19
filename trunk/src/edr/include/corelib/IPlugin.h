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

#include <corelib/Version.h>

#include <boost/version.hpp>
#include <QtCore/QtGlobal>
#include <utils/Macros.h>

#include <corelib/PluginCommon.h>
#include <corelib/ILog.h>
#include <corelib/IPath.h>
#include <utils/SmartPtr.h>
#include <corelib/IService.h> 
#include <corelib/ISource.h>
#include <corelib/IParser.h>
#include <corelib/IVisualizer.h>
#include <utils/Export.h>

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
#define CORE_GET_PLUGIN_API_VERSION_FUNCTION_NAME CoreGetPluginAPIVersion

#define CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME CoreGetPluginBuildType
//! Nazwa funkcji tworzącej plugin.
#define CORE_INITIALIZE_AND_LOAD_PLUGIN_FUNCTION_NAME CoreCreatePluginInstance
//! 
#define CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME CoreGetLibrariesVersions
//!
#define CORE_INITIALIZE_PLUGIN_DESCRIPTION_FUNCTION_NAME CoreInitializePluginDescription

//! Rozpoczyna rejestrację pluginu
//! \param name Nazwa pluginu
//! \param id ID pluginu
#define CORE_PLUGIN_BEGIN(name, id)			                           \
PLUGIN_DEFINE_CORE_APPLICATION_ACCESSOR                                 \
extern "C" UTILS_DECL_EXPORT void CORE_GET_PLUGIN_API_VERSION_FUNCTION_NAME( \
	int* major, int* minor, int* patch)									\
{                                                                       \
	*major = CORE_API_MAJOR;				                            \
	*minor = CORE_API_MINOR;			                              \
	*patch = CORE_API_PATCH;			                               \
}                                                                       \
extern "C" UTILS_DECL_EXPORT unsigned CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME() \
{                                                                       \
    return CORE_PLUGIN_BUILD_TYPE;                                      \
}                                                                       \
extern "C" UTILS_DECL_EXPORT void CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME(  \
    int* boostVersion, int* qtVersion, int* stlVersion)                 \
{                                                                       \
    *boostVersion = BOOST_VERSION;                                      \
    *qtVersion = QT_VERSION;                                            \
    *stlVersion = CORE_CPPLIB_VER;                                      \
}                                                                       \
extern "C" UTILS_DECL_EXPORT void CORE_INITIALIZE_PLUGIN_DESCRIPTION_FUNCTION_NAME(core::IPlugin * plugin) \
{																		\
	plugin->setName(name);												\
	plugin->setID(id);													\
	plugin->setDefaultLanguageCode("en");								\
}																		\
extern "C" UTILS_DECL_EXPORT void CORE_INITIALIZE_AND_LOAD_PLUGIN_FUNCTION_NAME(core::IPlugin * plugin, \
	core::IApplication* coreApplication)								\
{                                                                       \
    plugin::__coreApplication = coreApplication;						\


//! Rozpoczyna rejestrację pluginu z innym domyslnym językiem tłumaczeń niz angielski
//! \param name Nazwa pluginu
//! \param id ID pluginu
//! \param id langCode Kod domyślnego języka pluginy wg ISO639
#define CORE_EXT_PLUGIN_BEGIN(name, id, langCode)                           \
	PLUGIN_DEFINE_CORE_APPLICATION_ACCESSOR                                 \
	extern "C" UTILS_DECL_EXPORT void CORE_GET_PLUGIN_API_VERSION_FUNCTION_NAME( \
	int* major, int* minor, int* patch)									\
{                                                                       \
	*major = CORE_API_MAJOR;				                            \
	*minor = CORE_API_MINOR;			                              \
	*patch = CORE_API_PATCH;			                               \
}                                                                       \
extern "C" UTILS_DECL_EXPORT unsigned CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME() \
{                                                                       \
	return CORE_PLUGIN_BUILD_TYPE;                                      \
}                                                                       \
extern "C" UTILS_DECL_EXPORT void CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME(  \
	int* boostVersion, int* qtVersion, int* stlVersion)                 \
{                                                                       \
	*boostVersion = BOOST_VERSION;                                      \
	*qtVersion = QT_VERSION;                                            \
	*stlVersion = CORE_CPPLIB_VER;                                      \
}                                                                       \
extern "C" UTILS_DECL_EXPORT void CORE_INITIALIZE_PLUGIN_DESCRIPTION_FUNCTION_NAME(core::IPlugin * plugin) \
{																		\
	plugin->setName(name);												\
	plugin->setID(id);													\
	plugin->setDefaultLanguageCode(langCode);								\
}																		\
extern "C" UTILS_DECL_EXPORT void CORE_INITIALIZE_AND_LOAD_PLUGIN_FUNCTION_NAME(core::IPlugin * plugin, \
	core::IApplication* coreApplication)								\
{                                                                       \
	plugin::__coreApplication = coreApplication;						\



//! Kończy rejestrację pluginu.
#define CORE_PLUGIN_END                                                 \
}

//! Dodaje usługę zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_SERVICE(className)                              \
{																		\
	try { plugin->addService( plugin::IServicePtr(new className) ); }	\
	catch(std::exception & e) { PLUGIN_LOG_DEBUG("Error while loading service class " << #className << ": " << e.what()); } \
	catch(...) { PLUGIN_LOG_DEBUG("UNKNOWN error while loading service class" << #className); } \
}

//! Dodaje źródło danych DM zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_SOURCE(className)                              \
{																		\
	try { plugin->addSource( plugin::ISourcePtr(new className) ); }		\
	catch(std::exception & e) { PLUGIN_LOG_DEBUG("Error while loading source class " << #className << ": " << e.what()); } \
	catch(...) { PLUGIN_LOG_DEBUG("UNKNOWN error while loading source class " << #className); } \
}    


//! Dodaje parser zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_PARSER(className)                               \
{																		\
	try { plugin->addParserPrototype( plugin::IParserPtr(new className) ); }		\
	catch(std::exception & e) { PLUGIN_LOG_DEBUG("Error while loading parser class " << #className << ": " << e.what()); } \
	catch(...) { PLUGIN_LOG_DEBUG("UNKNOWN error while loading parser class " << #className); } \
}     


//! Dodaje wizualizator zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_VISUALIZER(className)                           \
{																		\
	try { plugin->addVisualizerPrototype( plugin::IVisualizerPtr(new className) ); }		\
	catch(std::exception & e) { PLUGIN_LOG_DEBUG("Error while loading visualizer class " << #className << ": " << e.what()); } \
	catch(...) { PLUGIN_LOG_DEBUG("UNKNOWN error while loading visualizer class " << #className); } \
} 

//! Dodanie nowego typu domenowego poprzez utworzenie dla niego ObjectWrapperFactory
#define CORE_PLUGIN_ADD_OBJECT_WRAPPER(className)               \
{																		\
	try { plugin->addObjectWrapperPrototype<className>(); }		\
	catch(std::exception & e) { PLUGIN_LOG_DEBUG("Error while loading Object Wrapper for class " << #className << ": " << e.what()); } \
	catch(...) { PLUGIN_LOG_DEBUG("UNKNOWN error while loading Object Wrapper for class " << #className); } \
}     


////////////////////////////////////////////////////////////////////////////////
namespace core {
	////////////////////////////////////////////////////////////////////////////////

//! Interfejs pluginu przez który dostarczane są usługi (serwisy) i prototypy elementów przetwarzających dane
class IPlugin : public plugin::IIdentifiable, public plugin::IDescription
{
public:
	//! Pusty destruktor wirtualny
	virtual ~IPlugin() {}

	//! \param name Nazwa pluginu
	virtual void setName(const std::string & name) = 0;
	//! \param  description Opis pluginu
	virtual void setDescription(const std::string & description) = 0;
	//! \param id Identyfikator pluginu
	virtual void setID(core::UniqueID id) = 0;
	//! \param langCode Kod domyślnego języka pluginu wg ISO639
	virtual void setDefaultLanguageCode(const std::string & langCode) = 0;

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
	//! \tparam T Typ dla którego rejestrujemy prototyp OW
	//! \param dummy Wskaźnik nie powinien być używany
	template<class T>
	void addObjectWrapperPrototype(const T * dummy = nullptr)
	{
		addObjectWrapperPrototype(core::ObjectWrapper::create<T>());
	}
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CORE_PLUGIN_H__
