/********************************************************************
	created:	2013/02/28
	created:	28:2:2013   18:30
	filename: 	HmmVdf.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMVDF_H__
#define HEADER_GUARD_HMM__HMMVDF_H__

#include <plugins/newVdf/NewVdfService.h>
#include <corelib/IServiceManager.h>
#include <dflib/INode.h>
#include "HmmMainWindow.h"
#include "FilterCommand.h"
#include "Vector3DFilterCommand.h"


#define _HMM_BEGIN_REGISTER \
    do {\
    core::IServiceManager* _manager = plugin::getServiceManager();\
    vdf::NewVdfServicePtr _vdfService = utils::dynamic_pointer_cast<vdf::NewVdfService>(_manager->getService(vdf::NewVdfService::getClassID()));

#define _HMM_END_REGISTER \
    } while(0);

#define HMM_REGISTER_DATA_SOURCE(name, uid, icon, constructed, initializer) \
    _HMM_BEGIN_REGISTER\
    auto _source = vdf::IDataSourcePtr(new vdf::DataSource(constructed, core::UID::GenerateUniqueID(uid),  name, initializer));\
    _source->setIcon(icon);\
    _vdfService->registerDataSource(_source); \
    _HMM_END_REGISTER

#define HMM_REGISTER_DATA_SINK(name, uid, icon, constructed, initializer) \
    _HMM_BEGIN_REGISTER\
    auto _sink = vdf::IDataSinkPtr(new vdf::DataSink(constructed, core::UID::GenerateUniqueID(uid),  name, initializer));\
    _sink->setIcon(icon);\
    _vdfService->registerDataSink(_sink); \
    _HMM_END_REGISTER

#define HMM_REGISTER_DATA_PROCESSOR(name, uid, icon, constructed, initializer) \
    _HMM_BEGIN_REGISTER\
    auto _processing = vdf::IDataProcessorPtr(new vdf::DataProcessor(constructed, core::UID::GenerateUniqueID(uid), name, initializer));\
    _processing->setIcon(icon);\
    _vdfService->registerDataProcessor(_processing); \
    _HMM_END_REGISTER


#define HMM_REGISTER_DATA_SOURCE_SIMPLE(type, name, uid, icon)    \
    do {                                                          \
    auto proto = new type();                                      \
    auto init =                                                   \
    [&](const df::ISourceNode* prototype)                         \
    {                                                             \
        return new type();                                        \
    };                                                            \
    HMM_REGISTER_DATA_SOURCE(name, uid, icon, proto, init);       \
    } while(0);                                                     

#define HMM_REGISTER_VECTOR_DATA_SOURCE(type, name, uid, icon, hmm)            \
    do {                                                                       \
    auto proto = new VectorSource(hmm, TreeBuilder::create##type##Branch,           \
    TreeBuilder::getRoot##type##Icon(), TreeBuilder::get##type##Icon());       \
                                                                               \
    auto init =                                                                \
    [&](const df::ISourceNode* prototype) -> VectorSource*                          \
    {                                                                          \
        auto proto = dynamic_cast<const VectorSource*>(prototype);                  \
        auto source = new VectorSource(proto->getHmm(), proto->getBranchFunction(), \
            proto->getRootIcon(), proto->getLeafIcon());                       \
        return source;                                                         \
    };                                                                         \
    HMM_REGISTER_DATA_SOURCE(name, uid, icon, proto, init);                    \
    } while(0);                        

#define HMM_REGISTER_DATA_SINK_SIMPLE(type, uid, name, icon)      \
    do {                                                          \
    auto proto = new type();                                      \
    auto init =                                                   \
    [&](const df::ISinkNode* prototype)                           \
    {                                                             \
        return new type();                                        \
    };                                                            \
    HMM_REGISTER_DATA_SINK(name, uid, icon, proto, init);         \
    } while(0);   

#define HMM_REGISTER_DATA_PROCESSOR_SIMPLE(type, name, uid, icon) \
    do {                                                          \
    auto proto = new type();                                      \
    auto init =                                                   \
    [&](const df::IProcessingNode* prototype)                     \
    {                                                             \
        return new type();                                        \
    };                                                            \
    HMM_REGISTER_DATA_PROCESSOR(name, uid, icon, proto, init);    \
    } while(0);   


class HmmMainWindow;

class HmmVdf
{
public:
    static void registerNodes(HmmMainWindow* hmm);
};


#endif
