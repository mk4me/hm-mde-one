#include "MdePCH.h"
#include "SimpleTab.h"
#include "ContextAutoPlacer.h"
#include "IAppUsageContextManager.h"

SimpleTab::SimpleTab( QWidget* mainWidget, const QIcon& icon, const QString& label ) :
    AbstractTab(icon, label),
    widget(mainWidget)
{
}

SimpleTab::SimpleTab( const QIcon& icon, const QString& label ) :
    AbstractTab(icon, label),
    widget(new QWidget()),
    rootContext(new MdeEmptyContext())
{
}


QWidget* SimpleTab::getMainWidget()
{
    return widget;
}

void SimpleTab::registerConxtext( IAppUsageContextManager* manager, IAppUsageContextPtr parent, QTabWidget* flexiWidget )
{
    QList<QAction*> actions = widget->actions();
    if (!actions.empty()) {
        rootContext = SimpleContextPtr(new SimpleContext(flexiWidget, getLabel()));
        manager->addContext(rootContext, parent);
        manager->addWidgetToContext(rootContext, widget);
    } else {
        rootContext = MdeEmptyContextPtr(new MdeEmptyContext());
    }
}



