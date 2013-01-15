/********************************************************************
    created:  2011/05/30
    created:  30:5:2011   8:21
    filename: IDataSource.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCE_H__
#define HEADER_GUARD___IDATASOURCE_H__

#include <core/IIdentifiable.h>
#include <core/IOutputProcessItem.h>
#include <core/SmartPtr.h>

//! Forward declaration
class QIcon;

namespace plugin {

//! Interfejs do implemntacji źródeł danych
class IDataSource : public ICoreElement, public IOutputProcessItem
{
public:
    //! \return Dialog konfigurujący opis wyjścia źródła danych
    virtual QDialog * getOutputConfigurationDialog() = 0;
    //! \return Czy źródło może jeszcze generować kolejne dane
    virtual const bool empty() const = 0;
    //! \return Ikona źródła
    virtual QIcon* getIcon() const = 0;
};

typedef core::shared_ptr<IDataSource> IDataSourcePtr;
typedef core::shared_ptr<const IDataSource> IDataSourceConstPtr;

}

#endif  //  HEADER_GUARD___IDATASOURCE_H__
