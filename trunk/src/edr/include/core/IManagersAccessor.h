/********************************************************************
    created:  2011/10/23
    created:  23:10:2011   16:45
    filename: IManagersAccessor.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IMANAGERSACCESSOR_H__
#define HEADER_GUARD___IMANAGERSACCESSOR_H__

namespace core {

    class IFileDataManager;
    class IMemoryDataManager;
    class IServiceManager;
	class ISourceManager;
    class IDataProcessorManager;
    class IDataSourceManager;
    class IVisualizerManager;
    class IDataManagerReader;

    class IManagersAccessor
    {
    public:

        virtual ~IManagersAccessor() {}

        virtual IDataManagerReader * getDataManagerReader() = 0;

        virtual const IDataManagerReader * getDataManagerReader() const = 0;

        virtual IFileDataManager * getFileDataManager() = 0;

        virtual const IFileDataManager * getFileDataManager() const = 0;

        virtual IMemoryDataManager * getMemoryDataManager() = 0;

        virtual const IMemoryDataManager * getMemoryDataManager() const = 0;

        virtual IServiceManager * getServiceManager() = 0;

        virtual const IServiceManager * getServiceManager() const = 0;

		virtual ISourceManager * getSourceManager() = 0;

		virtual const ISourceManager * getSourceManager() const = 0;

        virtual IVisualizerManager * getVisualizerManager() = 0;

        virtual const IVisualizerManager * getVisualizerManager() const = 0;

        virtual IDataProcessorManager * getDataProcessorManager() = 0;

        virtual const IDataProcessorManager * getDataProcessorManager() const = 0;

        virtual IDataSourceManager * getDataSourceManager() = 0;

        virtual const IDataSourceManager * getDataSourceManager() const = 0;

    };
}

#endif HEADER_GUARD___IMANAGERSACCESSOR_H__

