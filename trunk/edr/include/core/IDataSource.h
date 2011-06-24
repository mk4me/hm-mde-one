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

//! Forward declaration
class QIcon;

namespace core {

//! Interfejs do implemntacji �r�de� danych
class IDataSource : public core::IOutputProcessItem
{
public:
    //! \return Dialog konfiguruj�cy opis wyj�cia �r�d�a danych
    virtual QDialog * getOutputConfigurationDialog() = 0;
    //! \return Czy �r�d�o mo�e jeszcze generowac kolejne dane
    virtual bool empty() const = 0;
    //! \return Ikona �r�d�a
    virtual QIcon* getIcon() const = 0;
};

typedef shared_ptr<IDataSource> IDataSourcePtr;
typedef shared_ptr<const IDataSource> IDataSourceConstPtr;

}

#endif  //  HEADER_GUARD___IDATASOURCE_H__