#include "CorePCH.h"
#include <QtGui/QMessageBox>
#include "DataSourceManager.h"

using namespace core;

template <>
DataSourceManager * ManagerHelper<DataSourceManager>::manager = nullptr;

DataSourceManager::DataSourceManager()
{

}

DataSourceManager::~DataSourceManager()
{
    UTILS_ASSERT(dataSources.empty(), "Wszystkie źródła powinny być zniszczone.");
}

plugin::IDataSourceConstPtr DataSourceManager::getPrototype( UniqueID id ) const
{
    IDataSources::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(),
        [=](const plugin::IDataSourceConstPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return plugin::IDataSourceConstPtr(*found);
    } else {
        return plugin::IDataSourceConstPtr();
    }
}

DataSourcePtr DataSourceManager::createDataSource( UniqueID id )
{
    plugin::IDataSourceConstPtr dataSource = getPrototype(id);
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

DataSourcePtr DataSourceManager::createDataSource( const plugin::IDataSourceConstPtr& prototype )
{
    plugin::IDataSource* src = dynamic_cast<plugin::IDataSource*>(prototype->create());
    QDialog * dialog = src->getOutputConfigurationDialog();
    if(dialog != nullptr){
        QMessageBox::StandardButton ret = (QMessageBox::StandardButton)dialog->exec();
        if(ret == QMessageBox::Cancel){
            DataSourcePtr();
        }
    }

    //test czy jakieś dane wyjsciowe dostępne
    std::vector<plugin::IOutputDescription::OutputInfo> output;
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

void DataSourceManager::registerDataSource(const plugin::IDataSourcePtr & dataProcesor )
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
        [=](const plugin::IDataSourcePtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return static_cast<int>(std::distance(prototypes.begin(), found));
    } else {
        return -1;
    }
}

void DataSourceManager::notifyCreated( DataSource* dataSource )
{
    CORE_LOG_DEBUG("DataSource " << dataSource->getName() << " created");
    dataSources.push_back(dataSource);
}

void DataSourceManager::notifyDestroyed( DataSource* dataSource )
{
    CORE_LOG_DEBUG("DataSource " << dataSource->getName() << " destroyed.");
    dataSources.remove(dataSource);
}

