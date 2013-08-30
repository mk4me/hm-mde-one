#include "MdePCH.h"
#include "MdeServiceWindow.h"
#include <QtCore/QSettings>

MdeServiceWindow::MdeServiceWindow( const QString& idName ) :
    id(idName)
{
    loadLayout();
}

MdeServiceWindow::~MdeServiceWindow()
{
    saveLayout();
}

void MdeServiceWindow::saveLayout()
{
    QSettings settings("PJWSTK", "MDE");

    settings.beginGroup(id);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.endGroup();
}

void MdeServiceWindow::loadLayout()
{
    QSettings settings("PJWSTK", "MDE");

    settings.beginGroup(id);
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

void MdeServiceWindow::createMdeDock( QString serviceName, QWidget* widget )
{
    auto it = serviceCount.find(serviceName);
    if (it == serviceCount.end()) {
        serviceCount[serviceName] = 0;
    }
    coreUI::CoreDockWidget* dw = dockWrap(widget, widget->objectName(), serviceName, serviceCount[serviceName]++);
    if (!restoreDockWidget(dw)) {
        addDockWidget(Qt::RightDockWidgetArea, dw);
    }
}

coreUI::CoreDockWidget* MdeServiceWindow::dockWrap(QWidget* widget, const QString& dockTitle, const QString& serviceName, int propNo )
{
    QString dockName = QString("%1_%2_%3").arg(serviceName).arg(dockTitle).arg(propNo);
    coreUI::CoreDockWidget* dock = new coreUI::CoreDockWidget(dockTitle);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setWidget(widget);	
    dock->setObjectName(dockName);
    //dock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return dock;
}