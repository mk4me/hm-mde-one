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
        "Vector Differ",
        "{A6AFDF0D-1467-492D-BA07-AD7CB1773103}",
        QIcon(":/resources/icons/vdf/processor.png"));
    
    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        VectorAdder, 
        "Vector Adder",
        "{C47836AC-2B12-48AC-BB81-90B1DCA05CE3}",
        QIcon(":/resources/icons/vdf/processor.png"));

    HMM_REGISTER_VECTOR_DATA_SOURCE(
        Forces,
        "Forces",
        "{0300FAAE-056A-451E-AF2D-3EF3EA00D3F6}",
        QIcon(":/resources/icons/vdf/force.png"), 
        hmm);

    HMM_REGISTER_VECTOR_DATA_SOURCE(
        Powers,
        "Powers",
        "{B868ABAB-8F23-4383-8649-4B467527D9B1}",
        QIcon(":/resources/icons/vdf/power.png"), 
        hmm);

    HMM_REGISTER_VECTOR_DATA_SOURCE(
        Moments,
        "Moments",
        "{06533E53-98A8-46B1-B768-051B374C355D}",
        QIcon(":/resources/icons/vdf/moment.png"), 
        hmm);

    HMM_REGISTER_VECTOR_DATA_SOURCE(
        Markers,
        "Markers",
        "{13186456-BBAA-4AC0-A42F-75CC7717395D}",
        QIcon(":/resources/icons/vdf/marker.png"), 
        hmm);

    HMM_REGISTER_VECTOR_DATA_SOURCE(
        Joints,
        "Joints",
        "{A23986A8-147D-4A53-9559-33ACBA4CE174}",
        QIcon(":/resources/icons/vdf/joint.png"), 
        hmm);

    HMM_REGISTER_VECTOR_DATA_SOURCE(
        GRF,
        "GRF",
        "{FC67A0E3-FED2-44EA-9F61-4E492AF12E2D}",
        QIcon(":/resources/icons/vdf/grf.png"), 
        hmm);

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
        QIcon(":/resources/icons/vdf/sink.png"), 
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