#include "CorePCH.h"
#include "LocalDataSource.h"
#include "LocalDataSourceWidget.h"
#include <QtGui/QVBoxLayout>

std::string LocalDataSource::name = "LocalDataSource";

LocalDataSource::LocalDataSource()
{

}

LocalDataSource::~LocalDataSource()
{

}

void LocalDataSource::serialize(std::ostream& output)
{

}

void LocalDataSource::deserialize(std::istream& istream)
{

}

LocalDataSource* LocalDataSource::createClone() const
{
    return new LocalDataSource();
}

void LocalDataSource::getOutputInfo( OutputDescription& info )
{
    info = outputDescription;
}

QWidget* LocalDataSource::getConfigurationWidget()
{
    return nullptr;
}

QDialog * LocalDataSource::getOutputConfigurationDialog()
{
    QDialog* ret = new QDialog();
    QVBoxLayout* layout = new QVBoxLayout();
    QWidget * w = new LocalDataSourceWidget(this);
    layout->addWidget(w);
    ret->setLayout(layout);
    return ret;
}

void LocalDataSource::generate(core::IObjectOutput* output)
{

}

const std::string & LocalDataSource::getName() const
{
    return name;
}

bool LocalDataSource::empty() const
{
    return false;
}

QIcon* LocalDataSource::getIcon() const
{
    return nullptr;
}