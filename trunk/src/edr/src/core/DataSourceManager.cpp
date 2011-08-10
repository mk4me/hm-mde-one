#include "CorePCH.h"
#include "DataSourceManager.h"
#include <boost/foreach.hpp>

using namespace core;

DataSourceManager::DataSourceManager()
{

}

DataSourceManager::~DataSourceManager()
{
    UTILS_ASSERT(dataSources.empty(), "Wszystkie Ÿród³a powinny byæ zniszczone.");
    while (prototypes.size()) {
        prototypes.pop_back();
    }
}

IDataSourceConstPtr DataSourceManager::getPrototype( UniqueID id ) const
{
    IDataSources::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(), 
        [=](const IDataSourceConstPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return IDataSourceConstPtr(*found);
    } else {
        return IDataSourceConstPtr();
    }
}

DataSourcePtr DataSourceManager::createDataSource( UniqueID id )
{
    IDataSourceConstPtr dataSource = getPrototype(id);
    if ( dataSource ) {
        return createDataSource(dataSource);
    } else {
        throw std::runtime_error("DataSource not registered.");
    }
}

int DataSourceManager::getNumInstances( UniqueID id )
{
    return std::count_if( dataSources.begin(), dataSources.end(), [=](DataSource* ptr) {
        return ptr->getID() == id;
    });
}

DataSourcePtr DataSourceManager::createDataSource( const IDataSourceConstPtr& prototype )
{
    IDataSource* src = dynamic_cast<IDataSource*>(prototype->createClone());
    QDialog * dialog = src->getOutputConfigurationDialog();
    if(dialog != nullptr){
        QMessageBox::StandardButton ret = (QMessageBox::StandardButton)dialog->exec();
        if(ret == QMessageBox::Cancel){
            DataSourcePtr();
        }
    }

    //test czy jakieœ dane wyjsciowe dostepne
    std::vector<core::IOutputDescription::OutputInfo> output;
    src->getOutputInfo(output);

    if(output.empty() == true){
        //throw std::runtime_error(std::string("Source: ") + src->getName() + std::string(" ID: ") + boost::lexical_cast<std::string>(src->getID()) + std::string(" created without any outputs!"));
        return DataSourcePtr();
    }
    
    DataSourcePtr result(new DataSource(src));
    return result;
}

DataSourcePtr DataSourceManager::createDataSource( const DataSource& prototype )
{
    DataSourcePtr result(new DataSource(prototype));
    return result;
}

void DataSourceManager::registerDataSource(const IDataSourcePtr & dataProcesor )
{
    if (!getPrototype(dataProcesor->getID())) {
        prototypes.push_back(dataProcesor);
        notify();
    } else {
        throw std::runtime_error("DataSource with this ID already registered.");
    }
}

int DataSourceManager::getPrototypeIdx( UniqueID id ) const
{
    IDataSources::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(), 
        [=](const IDataSourcePtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return static_cast<int>(std::distance(prototypes.begin(), found));
    } else {
        return -1;
    }
}

void DataSourceManager::notifyCreated( DataSource* dataSource )
{
    LOG_DEBUG("DataSource " << dataSource->getName() << " created");
    dataSources.push_back(dataSource);
}

void DataSourceManager::notifyDestroyed( DataSource* dataSource )
{
    LOG_DEBUG("DataSource " << dataSource->getName() << " destroyed.");
    dataSources.remove(dataSource);
}

