BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	BaseDataTypes.h
	DataAccessors.h
	Filesystem.h
	StringTools.h
	Visualizer.h
	# SubjectDataFilters.h
	ObjectWrapperCollection.h
	PluginCommon.h
	#IConverter.h
	#IConverterManager.h
	IPlugin.h
	IVisualizerManager.h
	IVisualizer.h
	ITransaction.h
	IStreamManagerReader.h
	IStreamDataManager.h
	ISourceManager.h
	ISource.h
	IServiceManager.h
	IService.h
	ISerializable.h
	IPath.h
	IParserManagerReader.h
	IParser.h
	IMemoryDataManager.h
	ILog.h
	IIdentifiable.h
	IFileManagerReader.h
	IFileDataManager.h
	IDataManagerReader.h
	IDataHierarchyManagerReader.h
	IClonable.h
	IApplication.h
	HierarchyDataItem.h
	HierarchyHelper.h
	HierarchyItem.h
	IHierarchyDataItem.h
	IHierarchyItem.h
	IFilterProvider.h
	IFilterCommand.h
	IFilterBundle.h
	FilterBundle.h
	Export.h
)

SET_SOURCE_FILES(
	Filesystem.cpp
	Visualizer.cpp
	FilterBundle.cpp
	# SubjectDataFilters.cpp
	CoreLibPCH.cpp
	ObjectWrapperCollection.cpp
	HierarchyDataItem.cpp
	HierarchyHelper.cpp
	HierarchyItem.cpp
)

SET_PRIVATE_HEADERS(
	CoreLibPCH.h
)

SET_MOC_FILES(
	Visualizer.h
)

SET_PRECOMPILED_HEADER (CoreLibPCH.h CoreLibPCH.cpp)

END_PROJECT()
