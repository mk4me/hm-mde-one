BEGIN_PROJECT("static")

if(CONFIG_CORE_ENABLE_LOG4CXX)
	MARK_AS_PREREQUISITES("LOG4CXX")
endif()

CONFIG_OPTION(CORE_ENABLE_LEAK_DETECTION "Enable leak detection?" OFF)

SET_CONFIGURATION_INPUT_FILES(
	"Config.h.in"
)

CONFIGURE_PRIVATE_HEADER("Config.h.in" "Config.h")

SEARCH_DEPENDENCY_CONFIGURATION_DEFINES("Config.h" "CORE_ENABLE_LEAK_DETECTION;CORE_ENABLE_LOG4CXX;CORE_ENABLE_LOGGING")

SET_PRIVATE_HEADERS(
	Path.h
	Application.h
	ParserManager.h
	ApplicationCommon.h
	DataHierarchyManager.h
	MemoryDataManager.h
	FileDataManager.h
	StreamDataManager.h
	PluginApplication.h
	PluginPath.h
	SourceManager.h
	LogInitializer.h
	ServiceManager.h
	VisualizerManager.h
	Log.h
	PluginLoader.h
	CorePCH.h
	Plugin.h
	ConverterManager.h
	Converter.h
)

SET_PUBLIC_HEADERS (
	AppInitializer.h
)

SET_SOURCE_FILES(
	ConverterManager.cpp
	Converter.cpp
	AppInitializer.cpp
	Path.cpp
	Application.cpp
	ParserManager.cpp
	Log.cpp
	DataHierarchyManager.cpp
	MemoryDataManager.cpp
	FileDataManager.cpp
	StreamDataManager.cpp
	PluginApplication.cpp
	PluginPath.cpp
	SourceManager.cpp
	LogInitializer.cpp
	PluginLoader.cpp
	ServiceManager.cpp
	VisualizerManager.cpp
	CorePCH.cpp
	Plugin.cpp
)

SET_MOC_FILES(
	Application.h
)

SET_PRECOMPILED_HEADER (CorePCH.h CorePCH.cpp)

GENERATE_PROJECT_TRANSLATIONS()

END_PROJECT()