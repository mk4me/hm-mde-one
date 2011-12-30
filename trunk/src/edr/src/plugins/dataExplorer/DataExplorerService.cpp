#include "DataExplorerPCH.h"
#include "DataExplorerService.h"
#include <boost/algorithm/string.hpp>
#include <QtGui/QToolButton>
#include <QtGui/QLayout>

DataExplorerService::DataExplorerService() : widget(new TabWidget())
{
    widget->hideTabs();
    widget->setContentsMargins(0,0,0,0);
}

DataExplorerService::~DataExplorerService()
{

}

void DataExplorerService::init(core::IManagersAccessor * managersAccessor)
{
    this->managersAccessor = managersAccessor;
}

QWidget* DataExplorerService::getWidget(std::vector<QObject*>& actions)
{
    return widget;
}

const std::string& DataExplorerService::getName() const
{
    static const std::string name("DataExplorer");
    return name;
}

void DataExplorerService::registerSource(IDataExplorerSource * source)
{
    UTILS_ASSERT((source != nullptr), "B³êdne Ÿród³o danych dla DataExplorer");
    DataExplorerSourcePtr sourcePtr(source);

    if(source == nullptr){
        throw std::runtime_error("Trying to register uninitialized source");
    }

    if(sources.find(source->getID()) != sources.end()){
        throw std::runtime_error("Source with given ID already registered in DataExplorer");
    }

    //przygotowanie Ÿród³a

    source->init(managersAccessor->getMemoryDataManager(), managersAccessor->getFileDataManager());

    //dodanie Ÿród³a do widgeta

    addSourceToWidget(source);

    //ostateczna rejestracja Ÿród³a

    sources[source->getID()] = sourcePtr;

    if(sources.size() > 1){
        widget->showTabs();
    }
}

void DataExplorerService::addSourceToWidget(IDataExplorerSource * source)
{
    std::string name(source->getName());

    boost::trim(name);

    if(name.empty() == true){
        throw std::runtime_error("Could not service source with empty name or name with white spaces only");
    }

    QWidget * viewWidget = source->getOrCreateView();

    if(viewWidget == nullptr){
        throw std::runtime_error("Initializing source view was unsuccessful");
    }

    QDialog * configDialog = source->getOrCreateConfigurationDialog();
    QWidget * filterWidget = source->getOrCreateFiltersWidget();
    QAction * refreshAction = source->getOrCreateRefreshAction();

    QWidget * tabWidget = new QWidget();
    QWidget * currentWidget = tabWidget;

    //dzielimy widok na pó³ w pionie
    QHBoxLayout * hLayout = new QHBoxLayout();
    tabWidget->setLayout(hLayout);

    if(refreshAction != nullptr || configDialog != nullptr){

        //nowa czêœæ g³ówna
        currentWidget = new QWidget();
        hLayout->addWidget(currentWidget);

        //czêœæ na operacje
        QWidget * actionsWidget = new QWidget();
        QVBoxLayout * vLayout = new QVBoxLayout();
        if(refreshAction != nullptr){
            QToolButton * button = new QToolButton();
            button->setDefaultAction(refreshAction);
            vLayout->addWidget(button);
        }

        if(configDialog != nullptr){
            QToolButton * button = new QToolButton();
            QAction * configAction = new QAction(QString::fromUtf8("Konfiguruj"), nullptr);
            actionsToConfiguration[configAction] = configDialog;
            button->setDefaultAction(configAction);

            connect(configAction, SIGNAL("triggered()"), this, SLOT("configurationRequest()"));

            vLayout->addWidget(button);
        }
        
        actionsWidget->setLayout(vLayout);

        hLayout->addWidget(actionsWidget);

        actionsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    }

    if(filterWidget != nullptr){
        //dzielimy widok na pó³ w pionie
        QVBoxLayout * hLayout = new QVBoxLayout();
        
        hLayout->addWidget(filterWidget);
        
        currentWidget->setLayout(hLayout);
    }

    currentWidget->layout()->addWidget(viewWidget);

    widget->addTab(tabWidget, QString(name.c_str()));
}

void DataExplorerService::configurationRequest()
{
    QAction * action = qobject_cast<QAction *>(sender());

    if(action == nullptr){
        return;
    }

    auto it = actionsToConfiguration.find(action);
    if(it != actionsToConfiguration.end()){
        it->second->exec();
    }
}