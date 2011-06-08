/********************************************************************
    created:  2011/05/30
    created:  30:5:2011   8:21
    filename: IDataSource.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCE_H__
#define HEADER_GUARD___IDATASOURCE_H__

#include <core/IOutputProcessItem.h>

class QIcon;

namespace core {

class IDataSource : public core::IOutputProcessItem
{
public:
    virtual QDialog * getOutputConfigurationDialog() = 0;
    virtual bool empty() const = 0;
    virtual QIcon* getIcon() const = 0;
};

typedef shared_ptr<IDataSource> IDataSourcePtr;
typedef shared_ptr<const IDataSource> IDataSourceConstPtr;

}

#endif  //  HEADER_GUARD___IDATASOURCE_H__