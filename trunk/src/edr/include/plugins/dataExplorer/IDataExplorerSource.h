/********************************************************************
    created:  2011/10/24
    created:  24:10:2011   13:18
    filename: IDataExplorerSource.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATAEXPLORERSOURCE_H__
#define HEADER_GUARD___IDATAEXPLORERSOURCE_H__

#include <core/IIdentifiable.h>
#include <core/IDataManager.h>
#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include <QtGui/QAction>


//! Interfejs dla Ÿróde³ danych obs³ugiwany przez DataExplorera
class IDataExplorerSource : public core::IIdentifiable
{
public:
    virtual ~IDataExplorerSource() {}

    //! \return Widget z widokiem Ÿród³a - g³wny widget realizuj¹cy ca³a funkcjonalnoœæ Ÿród³a wraz z filtrami
    virtual QWidget * getOrCreateView() = 0;
    //! \return Widget do konfiguracji Ÿród³a
    virtual QDialog * getOrCreateConfigurationDialog() = 0;
    //! \return Widget do obs³ugi filtrowania danych
    virtual QWidget * getOrCreateFiltersWidget() = 0;
    //! \return Akcja s³u¿¹ca do odœwie¿ania danych
    virtual QAction * getOrCreateRefreshAction() = 0;
    //! \return Nazwa Ÿród³a
    virtual const std::string & getName() const = 0;
    //! \param memoryDataManager Manager danych w pamieci - dostêp do inicjalizacji poszczególnych obiektów domenowych
    //! \param fileDataManager Manager danych plikowych - dostêp do inicjalizacji poszczególnych plików
    virtual void init(core::IMemoryDataManager * memoryDataManager, core::IFileDataManager * fileDataManager) = 0;
};

#endif HEADER_GUARD___IDATAEXPLORERSOURCE_H__