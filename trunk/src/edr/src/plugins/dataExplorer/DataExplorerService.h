/********************************************************************
    created:  2011/10/24
    created:  24:10:2011   13:32
    filename: DataExplorerService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATAEXPLORERSERVICE_H__
#define HEADER_GUARD___DATAEXPLORERSERVICE_H__

#include <map>
#include <QtCore/QObject>
#include <QtGui/QTabWidget>
#include <core/IService.h>
#include <core/SmartPtr.h>
#include <plugins/dataExplorer/IDataExplorerSource.h>

class DataExplorerService : public QObject, public core::IService
{
    UNIQUE_ID("{0157346E-D2F3-4A4F-854F-37C87FA3E5F9}", "DataExplorer Service");

    Q_OBJECT;

private:

    typedef core::shared_ptr<IDataExplorerSource> DataExplorerSourcePtr;

    typedef std::map<QAction *, QDialog *> ActionsToConfiguration;

public:
    DataExplorerService();
    ~DataExplorerService();

    //core::Iservice
public:

    virtual void init(core::IManagersAccessor * managersAccessor);
    virtual QWidget* getWidget(std::vector<QObject*>& actions);
    virtual const std::string& getName() const;


    //custom operations
public:
    virtual void registerSource(IDataExplorerSource * source);

private slots:

    void configurationRequest();

private:

    void addSourceToWidget(IDataExplorerSource * source);

private:
    ActionsToConfiguration actionsToConfiguration;
    core::IManagersAccessor * managersAccessor;
    std::map<UniqueID, DataExplorerSourcePtr> sources;
    QTabWidget * widget;
};

#endif HEADER_GUARD___DATAEXPLORERSERVICE_H__