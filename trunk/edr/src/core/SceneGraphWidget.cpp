#include "CorePCH.h"
#include "SceneGraphWidget.h"
#include <core/OsgSceneDump.h>

////////////////////////////////////////////////////////////////////////////////

Q_DECLARE_METATYPE(IServiceWeakPtr);

////////////////////////////////////////////////////////////////////////////////

SceneGraphWidget::SceneGraphWidget(void):
Ui::SceneGraphWidget(), 
QWidget()
{
    setupUi(this); 
}

void SceneGraphWidget::serviceSelectionChanged(int index)
{
    // lokalna zmienna bo IntelliSense siê gubi...
    setCurrentService(index);
}

void SceneGraphWidget::refreshButtonClicked()
{
    QComboBox* currentServiceCombo = this->currentServiceCombo;
    if ( currentServiceCombo->count() ) {
        setCurrentService(currentServiceCombo->currentIndex());
    }
}

void SceneGraphWidget::setSceneGraph(osg::Node* root)
{
    clearTreeWidget();
    if ( root ) {
        sceneGraphTree->setEnabled(true);
        core::OsgSceneDumpQtTree populator(sceneGraphTree);
        populator.setTraversalMode( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN );
        root->accept(populator);
    } else {
        sceneGraphTree->setEnabled(false);
        new QTreeWidgetItem(sceneGraphTree, QStringList()<<""<<"Scene is empty"<<"");
    }
}

void SceneGraphWidget::clearTreeWidget()
{
    sceneGraphTree->clear();
}


void SceneGraphWidget::addServices( IServiceManager* manager )
{
    for ( int i = 0; i < manager->getNumServices(); ++i ) {
        addService( manager->getService(i) );
    }
}

void SceneGraphWidget::addService( IServicePtr service )
{
    // lokalna zmienna bo IntelliSense siê gubi...
    QComboBox* currentServiceCombo = this->currentServiceCombo;

    // ustalenie parametrów...
    QString name = QString::fromStdString(service->getName());
    QVariant userData = qVariantFromValue(IServiceWeakPtr(service));

    // dodanie do komba
    currentServiceCombo->addItem(name, userData);
}

void SceneGraphWidget::setCurrentService( int index )
{
    QComboBox* currentServiceCombo = this->currentServiceCombo;
    IServiceWeakPtr serviceWeak = currentServiceCombo->itemData(index).value<IServiceWeakPtr>();
    if (IServicePtr service = serviceWeak.lock()) {
        setSceneGraph( service->debugGetLocalSceneRoot() );
    } else {
        clearTreeWidget();
        new QTreeWidgetItem(sceneGraphTree, QStringList()<<""<<"Service pointer is outdated"<<"");
    }
}
