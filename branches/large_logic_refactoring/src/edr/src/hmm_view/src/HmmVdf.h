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


#define _HMM_BEGIN_REGISTER \
    do {\
    core::IServiceManager* _manager = plugin::getServiceManager();\
    vdf::NewVdfServicePtr _vdfService = core::dynamic_pointer_cast<vdf::NewVdfService>(_manager->getService(vdf::NewVdfService::getClassID()));

#define _HMM_END_REGISTER \
    } while(0);

#define HMM_REGISTER_DATA_SOURCE(name, uid, icon, constructed, initializer) \
    _HMM_BEGIN_REGISTER\
    _vdfService->registerDataSource(vdf::IDataSourcePtr(new vdf::DataSource(constructed, core::UID::GenerateUniqueID(uid),  name, initializer))); \
    _HMM_END_REGISTER

#define HMM_REGISTER_DATA_SINK(name, uid, icon, constructed, initializer) \
    _HMM_BEGIN_REGISTER\
    _vdfService->registerDataSink(vdf::IDataSinkPtr(new vdf::DataSink(constructed, core::UID::GenerateUniqueID(uid),  name, initializer))); \
    _HMM_END_REGISTER

#define HMM_REGISTER_DATA_PROCESSOR(name, uid, icon, constructed, initializer) \
    _HMM_BEGIN_REGISTER\
    _vdfService->registerDataProcessor(vdf::IDataProcessorPtr(new vdf::DataProcessor(constructed, core::UID::GenerateUniqueID(uid), name, initializer))); \
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
