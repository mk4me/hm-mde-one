#include "DataExplorerPCH.h"
#include "DataExplorerService.h"
#include <boost/algorithm/string.hpp>
#include <QtGui/QToolButton>
#include <QtGui/QLayout>

DataExplorerService::DataExplorerService() : widget(new TabWidget())
{
    widget->setObjectName(QString::fromUtf8("sourcesTabs"));
    widget->hideTabs();
}

DataExplorerService::~DataExplorerService()
{

}

void DataExplorerService::init(core::IManagersAccessor * managersAccessor)
{
    this->managersAccessor = managersAccessor;
}

QWidget* DataExplorerService::getWidget(core::IActionsGroupManager * actionsManager)
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
    tabWidget->setObjectName(QString::fromUtf8("sourceTab"));
    //tabWidget->setContentsMargins(4,4,4,4);

    //dzielimy widok na pó³ w poziomie
    QVBoxLayout * vLayout = new QVBoxLayout();
    tabWidget->setLayout(vLayout);


    QWidget * centerWidget = new QWidget();
    centerWidget->setObjectName(QString::fromUtf8("sourceCenter"));
    centerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    centerWidget->setLayout(new QHBoxLayout());

    QWidget * currentWidget = new QWidget();
    currentWidget->setObjectName(QString::fromUtf8("source"));
    currentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    currentWidget->setLayout(new QVBoxLayout());
    currentWidget->setContentsMargins(0,0,0,0);

    currentWidget->layout()->addWidget(viewWidget);
    //currentWidget->layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));

    QWidget * actionsWidget = new QWidget();
    actionsWidget->setObjectName(QString::fromUtf8("sourceActions"));
    actionsWidget->setContentsMargins(0,0,0,0);
    QVBoxLayout * vLayoutActions = new QVBoxLayout();
    actionsWidget->setLayout(vLayoutActions);
    actionsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    centerWidget->layout()->addWidget(currentWidget);
    centerWidget->layout()->addWidget(actionsWidget);
    centerWidget->layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));

    if(refreshAction != nullptr || configDialog != nullptr){

        //czêœæ na operacje
        if(refreshAction != nullptr){
            QToolButton * button = new QToolButton();
            button->setDefaultAction(refreshAction);
            vLayout->addWidget(button);
        }

        if(configDialog != nullptr){
            QToolButton * button = new QToolButton();
            QAction * configAction = new QAction(tr("Configure"), nullptr);
            actionsToConfiguration[configAction] = configDialog;
            button->setDefaultAction(configAction);

            connect(configAction, SIGNAL("triggered()"), this, SLOT("configurationRequest()"));

            vLayout->addWidget(button);
        }
    }

    if(filterWidget != nullptr){
        vLayout->addWidget(filterWidget);
    }

    vLayout->addWidget(centerWidget);
    vLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Preferred));

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