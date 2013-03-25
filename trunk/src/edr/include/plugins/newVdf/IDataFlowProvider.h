/********************************************************************
	created:	2013/02/22
	created:	22:2:2013   10:42
	filename: 	IDataFlowProvider.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IDATAFLOWPROVIDER_H__
#define HEADER_GUARD_NEWVDF__IDATAFLOWPROVIDER_H__

#include <list>
#include <plugins/newVdf/IWorkflowItemBase.h>
#include <plugins/newVdf/DataProcessor.h>
#include <plugins/newVdf/DataSource.h>
#include <plugins/newVdf/DataSink.h>

namespace df { class IProcessingNode; class ISourceNode; class ISinkNode; };

#define VDF_BEGIN public: virtual void provideDataFlowItems(std::list<vdf::IWorkflowItemBasePtr>& items) {
#define VDF_END }

//! Dodaje elementu przetwarzającego zadanego typu do pluginu.
#define VDF_ADD_DATA_PROCESSOR(className, uid)                           \
    items.push_back( vdf::IDataProcessorPtr(new vdf::DataProcessor(new (className), (uid), (#className), [&](const df::IProcessingNode* prototype) { return new className(); })) );

//! Dodaje źródło zadanego typu do pluginu.
#define VDF_ADD_DATA_SOURCE(className, uid)                           \
	items.push_back( vdf::IDataSourcePtr(new vdf::DataSource(new (className), (uid), (#className), [&](const df::ISourceNode* prototype) { return new className(); })) );

//! Dodaje sink zadanego typu do pluginu.
#define VDF_ADD_DATA_SINK(className, uid)                           \
	items.push_back( vdf::IDataSinkPtr(new vdf::DataSink(new (className), (uid), (#className), [&](const df::ISinkNode* prototype) { return new className(); })) );

// Ułatwia dodawanie elementów Data Flow. Tworzony jest serwis, którego 
// jedynym celem jest dostarczenie elementów przetwarzających. Parametrami
// są: unikalna nazwa oraz unikalny identyfikator. 
// Przykład użycia:
// VDF_SERVICE_BEGIN(UnikalnaNazwaSerwisu, "{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}")
// 		VDF_ADD_DATA_PROCESSOR(NazwaZdefiniowanegoProcesora, core::UID::GenerateUniqueID("{YYYYYYYY-YYYY-YYYY-YYYY-YYYYYYYYYYYY}"))
// VDF_SERVICE_END(UnikalnaNazwaSerwisu)
#define VDF_SERVICE_BEGIN(ServiceName, uid) 								  \
class ServiceName : public plugin::IService, public vdf::IDataFlowProvider	  \
{																			  \
	UNIQUE_ID(uid)															  \
	CLASS_DESCRIPTION(#ServiceName, #ServiceName);							  \
	VDF_BEGIN																  

//! Kontynuacja makra VDF_SERVICE_BEGIN
#define VDF_SERVICE_END(ServiceName)										  \
	VDF_END																	  \
public:																		  \
	virtual void init( 														  \
		core::ISourceManager * sourceManager, 								  \
		core::IVisualizerManager * visualizerManager,						  \
		core::IMemoryDataManager * memoryDataManager, 						  \
		core::IStreamDataManager * streamDataManager, 						  \
		core::IFileDataManager * fileDataManager ) {}						  \
	virtual const bool lateInit()  { return true; } 						  \
	virtual void finalize() {} 												  \
	virtual void update( double deltaTime ) {} 								  \
	virtual QWidget* getWidget() { return nullptr; } 						  \
	virtual QWidget* getControlWidget() { return nullptr; } 				  \
	virtual QWidget* getSettingsWidget() { return nullptr; } 				  \
};																			  \
CORE_PLUGIN_ADD_SERVICE(ServiceName);


namespace vdf {

class IDataFlowProvider
{
public:
	virtual ~IDataFlowProvider() {}
	virtual void provideDataFlowItems(std::list<IWorkflowItemBasePtr>& items) = 0;
};
typedef core::shared_ptr<IDataFlowProvider> IDataFlowProviderPtr;
typedef core::shared_ptr<const IDataFlowProvider> IDataFlowProviderConstPtr;
}

#endif
