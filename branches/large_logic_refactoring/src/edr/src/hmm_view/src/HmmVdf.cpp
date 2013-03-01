#include "hmmPCH.h"
#include "HmmVdf.h"
#include "HmmSources.h"
#include "HmmSinks.h"
#include "HmmPins.h"
#include "HmmProcessors.h"


void HmmVdf::registerNodes(HmmMainWindow* hmm)
{
    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        XProcessor, 
        "Vector Diff",
        "{A6AFDF0D-1467-492D-BA07-AD7CB1773103}",
        "QIcon()");
    
    HMM_REGISTER_DATA_SOURCE_SIMPLE(
        XSource,
        "GRFs",
        "{13186456-BBAA-4AC0-A42F-75CC7717395D}",
        QIcon());

    auto xsink = new XSink(hmm);
    auto xlambda = 
    [&](const df::ISinkNode* prototype) -> XSink*
    { 
        auto sink = new XSink();
        auto proto = dynamic_cast<const XSink*>(prototype);
        sink->setHmm(proto->getHmm());
        return sink; 
    };
    HMM_REGISTER_DATA_SINK(
        "Vector -> Tree", 
        "{57A82EDD-E890-4A6E-8DA9-03CF74F91E32}",
        "icon path", 
        xsink, 
        xlambda);


    //    vdfService->registerDataSource(vdf::IDataSourcePtr(new vdf::DataSource(new XSource(item), core::UniqueID(random_uuid()), wrapped->getText().toStdString(), 
    //    [&](const df::ISourceNode* prototype) -> XSource*
    //{ 
    //    auto source = new XSource();
    //    auto proto = dynamic_cast<const XSource*>(prototype);
    //    source->setChannel(proto->getChannel());
    //    return source; 
    //})));

    //vdfService->registerDataSink(vdf::IDataSinkPtr(new vdf::DataSink(new XSink(this), core::UniqueID(random_uuid()), "Vector -> Tree", 
    //    [&](const df::ISinkNode* prototype) -> XSink*
    //{ 
    //    auto sink = new XSink();
    //    auto proto = dynamic_cast<const XSink*>(prototype);
    //    sink->setHmm(proto->getHmm());
    //    return sink; 
    //})));

    //vdfService->registerDataProcessor(vdf::IDataProcessorPtr(new vdf::DataProcessor(new XProcessor(), core::UniqueID(random_uuid()), "Vector Differ", 
    //    [&](const df::IProcessingNode* prototype) -> XProcessor*
    //{ 
    //    auto p = new XProcessor();
    //    //auto proto = dynamic_cast<const XProcessor*>(prototype);
    //    //source->setChannel(proto->getChannel());
    //    return p; 
    //})));
}