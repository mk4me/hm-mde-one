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


//! Interfejs dla �r�de� danych obs�ugiwany przez DataExplorera
class IDataExplorerSource : public core::IIdentifiable
{
public:
    virtual ~IDataExplorerSource() {}

    //! \return Widget z widokiem �r�d�a - g�wny widget realizuj�cy ca�a funkcjonalno�� �r�d�a wraz z filtrami
    virtual QWidget * getOrCreateView() = 0;
    //! \return Widget do konfiguracji �r�d�a
    virtual QDialog * getOrCreateConfigurationDialog() = 0;
    //! \return Widget do obs�ugi filtrowania danych
    virtual QWidget * getOrCreateFiltersWidget() = 0;
    //! \return Akcja s�u��ca do od�wie�ania danych
    virtual QAction * getOrCreateRefreshAction() = 0;
    //! \return Nazwa �r�d�a
    virtual const std::string & getName() const = 0;
    //! \param memoryDataManager Manager danych w pamieci - dost�p do inicjalizacji poszczeg�lnych obiekt�w domenowych
    //! \param fileDataManager Manager danych plikowych - dost�p do inicjalizacji poszczeg�lnych plik�w
    virtual void init(core::IMemoryDataManager * memoryDataManager, core::IFileDataManager * fileDataManager) = 0;
};

#endif HEADER_GUARD___IDATAEXPLORERSOURCE_H__