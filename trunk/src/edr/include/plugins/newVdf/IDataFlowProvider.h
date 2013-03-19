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
