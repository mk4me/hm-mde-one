#include "hmmPCH.h"
#include "TreeRefresher.h"
#include <QtGui/QMessageBox>
#include <corelib/PluginCommon.h>
#include <corelib/DataAccessors.h>
#include <plugins/subject/ISession.h>
#include "TreeBuilder.h"

TreeRefresher::TreeRefresher() : 
preventRefresh(false), 
    needRefresh(false),
    tree(nullptr)
{}

void TreeRefresher::actualRefresh(QTreeWidget* tree, const core::ObjectWrapperCollection& sessions) {

    QMessageBox message;
    message.setWindowTitle(QObject::tr("Refreshing analysis data"));
    message.setText(message.windowTitle());
    message.setWindowFlags(Qt::CustomizeWindowHint);
    message.setStandardButtons(0);

    message.show();
    QApplication::processEvents();

    try{

        QTreeWidgetItem* item = TreeBuilder::createTree(QObject::tr("Active Data"), sessions);

        tree->clear();
        tree->addTopLevelItem(item);
        item->setExpanded(true);

        QFont font = item->font(0);
        font.setPointSize(12);
        item->setFont(0, font);

        for (int i = 0; i < item->childCount(); ++i) {
            QTreeWidgetItem* child = item->child(i);
            child->setExpanded(true);

            QFont font = item->font(0);
            font.setPointSize(14);
            item->setFont(0, font);
        }

    }catch(...){

    }

    message.setCursor(Qt::ArrowCursor);
}

void TreeRefresher::refresh( QTreeWidget* tree )
{
    if (preventRefresh) {
        this->tree = tree;
        needRefresh = true;
    } else {
        core::ObjectWrapperCollection sessions(typeid(PluginSubject::ISession), false);
		plugin::getDataManagerReader()->getObjects(sessions);
        actualRefresh(tree, sessions);
    }
}

void TreeRefresher::setPreventRefresh( bool val )
{ 
    preventRefresh = val; 
    if (!val && needRefresh) {
        UTILS_ASSERT(tree);
        needRefresh = false;
		core::ObjectWrapperCollection sessions(typeid(PluginSubject::ISession), false);
		plugin::getDataManagerReader()->getObjects(sessions);
        actualRefresh(tree, sessions);
        tree = nullptr;
    }
}


