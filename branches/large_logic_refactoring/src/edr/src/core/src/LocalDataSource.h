/********************************************************************
    created:  2011/05/27
    created:  27:5:2011   10:55
    filename: LocalDataSource.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___LOCALDATASOURCE_H__
#define HEADER_GUARD___LOCALDATASOURCE_H__

#include <map>
#include <core/IDataSource.h>
#include <core/ObjectWrapperCollection.h>
#include <core/IObjectOutput.h>
#include "OutputDescription.h"

namespace core {

class LocalDataSourceWidget;

class LocalDataSource : public plugin::IDataSource
{
    friend class LocalDataSourceWidget;

    UNIQUE_ID("{54792DC1-D78C-4126-9CA8-643F9F6EF35A}", "LocalDataSource", "Local Data Source");

public:
    typedef std::vector<plugin::IOutputDescription::OutputInfo> OutputDescription;

private:
    typedef std::map<TypeInfo, ObjectWrapperCollectionPtr> Data;

public:
    LocalDataSource();
    ~LocalDataSource();

    virtual const std::string & getName() const;

    virtual void serialize(std::ostream& output);
    virtual void deserialize(std::istream& istream);

    virtual LocalDataSource* createClone() const;

    virtual void getOutputInfo( OutputDescription& info );
	virtual void generate(plugin::IObjectOutput* output);

    virtual QWidget* getConfigurationWidget();

    virtual QDialog * getOutputConfigurationDialog();
    virtual const bool empty() const;
    virtual QIcon* getIcon() const;

    virtual void reset();

private:
    static std::string name;
    std::string trialPath;

    OutputDescription outputDescription;

    Data data;

    mutable bool hasMoreData;
};

}

#endif  //  HEADER_GUARD___LOCALDATASOURCE_H__
