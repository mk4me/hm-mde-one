#include "CorePCH.h"
#include <boost/foreach.hpp>
#include <core/StringTools.h>
#include "VisualizerTitleBarComboPick.h"
#include "DataManager.h"

using namespace core;

Q_DECLARE_METATYPE(ObjectWrapperConstPtr);

VisualizerTitleBarComboPick::VisualizerTitleBarComboPick( QWidget* parent /*= nullptr */ ) :
QWidget(parent), groupSources(new QActionGroup(this)), menuSource(new QMenu(this))
{
    setupUi(this);
    /*setAttribute(Qt::WA_NoSystemBackground, true);*/
    connect(menuSource, SIGNAL(aboutToShow()), this, SLOT(fillSourcesMenu()));
    buttonSource->setMenu(menuSource);
}


void VisualizerTitleBarComboPick::fillSourcesMenu()
{
    fillSourcesMenu(menuSource, groupSources, getCurrentSlot());
}

void VisualizerTitleBarComboPick::fillSourcesMenu( QMenu* menu, QActionGroup* group, int slotNo )
{
    if ( visualizer ) {
        delete groupSources;
        groupSources = new QActionGroup(this);
        menuSource->clear();

        // wspierane obiekty w tym slocie
        std::list<ObjectWrapper::Type> supportedTypes = visualizer->getSupportedTypes(slotNo);
        UTILS_ASSERT(!supportedTypes.empty());

        // pobranie obiektu w tym slocie
        const core::ObjectWrapperConstPtr& object = visualizer->getObject(slotNo);

        // akcja zeruj¹ca obiekt
        QAction* actionNone = menu->addAction("none");
        connect(actionNone, SIGNAL(triggered()), this, SLOT(sourceSelected()) );
        actionNone->setCheckable(true);
        actionNone->setData(qVariantFromValue(ObjectWrapperConstPtr()));
        actionNone->setChecked(!object);
        group->addAction(actionNone);

        bool nested = true;// (++supportedTypes.begin() != supportedTypes.end());

        // dodanie submenu z akcjami wybieraj¹cymi konkretne obiekty
        BOOST_FOREACH(ObjectWrapper::Type type, supportedTypes) {
            // pobranie obiektów z dataManagera
            // TODO: bardziej generyczne pobranie obiektów
            LOG_DEBUG("Type " << type.typeinfo.name() << " at slot " << slotNo);
            std::vector<ObjectWrapperPtr> objects;
            DataManager::getInstance()->getObjects(objects, type);

            std::ostringstream str;
            str << type.typeinfo.name() << " [" << objects.size() << "]";

            if ( objects.empty() ) {
                QAction* action = menu->addAction(toQString(str.str()));
                action->setEnabled(false);
            } else {
                QMenu* nestedMenu = nested ? menu->addMenu(toQString(str.str())) : menu;
                BOOST_FOREACH(ObjectWrapperPtr ptr, objects) {
                    //titleBar->comboData->addItem( toQString(ptr->getName()) );
                    std::ostringstream str;
                    str << ptr->getName() << " (from " << ptr->getSource() << ")";
                    QAction* action = nestedMenu->addAction(toQString(str.str()));
                    connect(action, SIGNAL(triggered()), this, SLOT(sourceSelected()) );
                    //connect(action, SIGNAL(triggered()), mapper, SLOT(map()));
                    action->setCheckable(true);
                    action->setData(qVariantFromValue(ObjectWrapperConstPtr(ptr)));
                    action->setChecked( ptr == object );
                    group->addAction(action);
                }
            }
        }
    }
}


void VisualizerTitleBarComboPick::sourceSelected()
{
    UTILS_ASSERT(visualizer);

    QAction* action = qobject_cast<QAction*>(sender());
    UTILS_ASSERT(action);

    int slotNo = getCurrentSlot();
    UTILS_ASSERT(slotNo >= 0);
    ObjectWrapperConstPtr obj = action->data().value<ObjectWrapperConstPtr>();

    QString objName = getLabel(obj, true);
    QString slotName = toQString(visualizer->getName(slotNo));

    // ustawienie wygl¹du guzika
    buttonSource->setText(objName);
    comboSlot->setItemText(slotNo, QString("%0: %1").arg(slotName).arg(objName));

    // ustawienie wizualizatora
    visualizer->setObject(slotNo, obj);
    visualizer->trySetUp();
}

int VisualizerTitleBarComboPick::getCurrentSlot() const
{
    return comboSlot->currentIndex();
}

void VisualizerTitleBarComboPick::clear()
{
    // usuniêcie informacji o slotach
    comboSlot->clear();
    // poukrywanie elementów UI
    comboSlot->hide();
    buttonSource->hide();
}

void VisualizerTitleBarComboPick::setCurrentSlot( int idx )
{
    if ( sender() != comboSlot ) {
        if ( comboSlot->currentIndex() != idx ) {
            // przekierowanie slotu
            comboSlot->setCurrentIndex(idx);
        }
    } else {
        if ( idx < 0 ) {
            buttonSource->hide();
        } else {
            UTILS_ASSERT(visualizer);
            ObjectWrapperConstPtr ptr = visualizer->getObject(idx);
            buttonSource->setText( getLabel(ptr, true) );
            buttonSource->show();
        }
    }
}

QString VisualizerTitleBarComboPick::getLabel( const core::ObjectWrapperConstPtr& object, bool noSource )
{
    if ( !object ) {
        return QString("none");
    } else {
        if ( noSource ) {
            return toQString(object->getName());
        } else {
            // TODO: uzupe³niæ
            return toQString(object->getName());
        }
    }
}

void VisualizerTitleBarComboPick::setCurrentVisualizer( const VisualizerPtr& visualizer )
{
    this->visualizer = visualizer;
    // odœwie¿enie listy obiektów wejœciowych
    comboSlot->clear();
    for ( int i = 0; i < visualizer->getNumObjects(); ++i ) {
        // nazwy slotów
        comboSlot->addItem( QString("%0: %1")
            .arg(core::toQString(visualizer->getName(i)))
            .arg(getLabel(ObjectWrapperConstPtr(), true)) );
    }
}