BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	AbstractFilterCommand.h
	AbstractSerie.h
	BaseDataTypes.h
	DataAccessors.h
	Filesystem.h
	StringTools.h
	Visualizer.h
	ObjectWrapperCollection.h
	PluginCommon.h
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
	IJobManager.h
	IThreadPool.h
	IJob.h
	IThread.h
	Version.h
)

SET_SOURCE_FILES(
	AbstractFilterCommand.cpp
	AbstractSerie.cpp
	Filesystem.cpp
	Visualizer.cpp
	FilterBundle.cpp
	CoreLibPCH.cpp
	ObjectWrapperCollection.cpp	
	HierarchyDataItem.cpp
	HierarchyHelper.cpp
	HierarchyItem.cpp
	Version.cpp
)

SET_PRIVATE_HEADERS(
	CoreLibPCH.h
)

SET_MOC_FILES(
	Visualizer.h
)

SET_PRECOMPILED_HEADER (CoreLibPCH.h CoreLibPCH.cpp)

END_PROJECT()
