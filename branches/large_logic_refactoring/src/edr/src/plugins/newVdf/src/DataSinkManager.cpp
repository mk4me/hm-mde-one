#include "NewVdfPCH.h"
#include <QtGui/QMessageBox>
#include "DataSinkManager.h"

//using namespace core;
using namespace vdf;

DataSinkManager::DataSinkManager()
{

}

DataSinkManager::~DataSinkManager()
{
    UTILS_ASSERT(dataSinks.empty(), "Wszystkie źródła powinny być zniszczone.");
}

IDataSinkConstPtr DataSinkManager::getPrototype( core::UniqueID id ) const
{
    IDataSinks::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(),
        [=](const IDataSinkConstPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return IDataSinkConstPtr(*found);
    } else {
        return IDataSinkConstPtr();
    }
}

//DataSinkPtr DataSinkManager::createDataSink( UniqueID id )
//{
//    IDataSinkConstPtr dataSink = getPrototype(id);
//    if ( dataSink ) {
//        return createDataSink(dataSink);
//    } else {
//        throw std::runtime_error("DataSink not registered.");
//    }
//}

//int DataSinkManager::getNumInstances( UniqueID id )
//{
//    return std::count_if( dataSinks.begin(), dataSinks.end(), [=](DataSink* ptr) {
//        return ptr->getID() == id;
//    });
//}

//DataSinkPtr DataSinkManager::createDataSink( const IDataSinkConstPtr& prototype )
//{
//    IDataSink* src = dynamic_cast<IDataSink*>(prototype->createClone());
//    QDialog * dialog = src->getOutputConfigurationDialog();
//    if(dialog != nullptr){
//        QMessageBox::StandardButton ret = (QMessageBox::StandardButton)dialog->exec();
//        if(ret == QMessageBox::Cancel){
//            DataSinkPtr();
//        }
//    }
//
//    //test czy jakieś dane wyjsciowe dostępne
//    std::vector<core::IInputDescription::InputInfo> input;
//    src->getInputInfo(input);
//
//    if(input.empty() == true){
//        //throw std::runtime_error(std::string("Sink: ") + src->getName() + std::string(" ID: ") + boost::lexical_cast<std::string>(src->getID()) + std::string(" created without any outputs!"));
//        UTILS_ASSERT(false);
//        return DataSinkPtr();
//    }
//
//    DataSinkPtr result(new DataSink(src));
//    return result;
//}
//
//DataSinkPtr DataSinkManager::createDataSink( const DataSink& prototype )
//{
//    DataSinkPtr result(new DataSink(prototype));
//    return result;
//}

void DataSinkManager::registerDataSink(const IDataSinkPtr & dataProcesor )
{
    if (!getPrototype(dataProcesor->getID())) {
        prototypes.push_back(dataProcesor);
        notify();
    } else {
        throw std::runtime_error("DataSink with this ID already registered.");
    }
}

int DataSinkManager::getPrototypeIdx( core::UniqueID id ) const
{
    IDataSinks::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(),
        [=](const IDataSinkPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return static_cast<int>(std::distance(prototypes.begin(), found));
    } else {
        return -1;
    }
}

//void DataSinkManager::notifyCreated( DataSink* dataSink )
//{
//    LOG_DEBUG("DataSink " << dataSink->getName() << " created");
//    dataSinks.push_back(dataSink);
//}
//
//void DataSinkManager::notifyDestroyed( DataSink* dataSink )
//{
//    LOG_DEBUG("DataSink " << dataSink->getName() << " destroyed.");
//    dataSinks.remove(dataSink);
//}

