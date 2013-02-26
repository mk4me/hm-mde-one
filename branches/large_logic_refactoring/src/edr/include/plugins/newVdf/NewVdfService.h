/********************************************************************
	created:	2012/12/13
	created:	13:12:2012   11:59
	filename: 	NewVdfService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__NEWVDFSERVICE_H__
#define HEADER_GUARD_NEWVDF__NEWVDFSERVICE_H__

#include <corelib/IService.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/BaseDataTypes.h>
#include <plugins/newVdf/IDataProcessor.h>
#include <plugins/newVdf/IDataSource.h>
#include <plugins/newVdf/IDataSink.h>
#include <plugins/newVdf/IDataProcessorManager.h>
#include <plugins/newVdf/IDataSourceManager.h>
#include <plugins/newVdf/IDataSinkManager.h>
#include <plugins/newVdf/IDataFlowProvider.h>
#include <plugins/newVdf/ICommand.h>
#include <plugins/newVdf/ExampleItems.h>
#include <plugins/newVdf/Export.h>

namespace vdf {

class NewVdfWidget;
class CanvasStyleEditorWidget;
class TypesWindow;

class PLUGIN_NEWVDF_EXPORT NewVdfService : public plugin::IService, public IDataFlowProvider
{
    UNIQUE_ID("{DF5B5B15-C591-4BCF-A205-FD995D2398DB}")
	CLASS_DESCRIPTION("Data Flow Service", "Data Flow Service");
	VDF_BEGIN
		VDF_ADD_DATA_SOURCE(IntSource, core::UID::GenerateUniqueID("{EF393C1F-2202-47DA-A1B9-D5DE868FDFFA}"))
		VDF_ADD_DATA_PROCESSOR(IntProcessor, core::UID::GenerateUniqueID("{511A32C7-E82D-42FF-9BAA-2A74F83A5103}"))
		VDF_ADD_DATA_SINK(IntSink, core::UID::GenerateUniqueID("{B4F05E79-80E7-46E7-97AE-FD81212C9AF7}"))
	VDF_END
	     
public:
    NewVdfService();
    virtual ~NewVdfService();

public:
    //! 
    //! \param managersAccessor 
	virtual void init(core::ISourceManager * sourceManager,
		core::IVisualizerManager * visualizerManager,
		core::IMemoryDataManager * memoryDataManager,
		core::IStreamDataManager * streamDataManager,
		core::IFileDataManager * fileDataManager);
    //! 
    //! \param actions 
    virtual QWidget* getWidget();
    //! 
    //! \param actions 
    virtual QWidget* getSettingsWidget();
    //! 
    //! \param actions 
    virtual QWidget* getControlWidget();

	virtual const bool lateInit();
	virtual void finalize();
	virtual void update(double time);

    void registerDataProcessor(const IDataProcessorPtr & dataProcessor);
    void registerDataSink(const IDataSinkPtr & dataSink);
    void registerDataSource(const IDataSourcePtr & dataSource);

private:
    std::string name;
    NewVdfWidget* newVdfWidget;
    CanvasStyleEditorWidget* canvasStyleEditorWidget;
    TypesWindow* typesWindow;
	//QListWidget* commandStackDebug;
	CommandStackPtr commandStack;
	IDataProcessorManagerPtr dataProcessorManager;
    IDataSourceManagerPtr dataSourceManager;
    IDataSinkManagerPtr dataSinkManager;
};
typedef core::shared_ptr<NewVdfService> NewVdfServicePtr;
typedef core::shared_ptr<const NewVdfService> NewVdfServiceConstPtr;
}

#endif  //HEADER_GUARD___VDFSERVICE_H__

