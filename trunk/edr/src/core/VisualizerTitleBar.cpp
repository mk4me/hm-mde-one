#include "CorePCH.h"
#include <core/ObjectWrapper.h>
#include <core/StringTools.h>
#include <boost/foreach.hpp>
#include "VisualizerTitleBar.h"
#include "DataManager.h"
#include "VisualizerManager.h"
#include "VisualizerTitleBarComboPick.h"

using namespace core;

VisualizerTitleBar::VisualizerTitleBar( QWidget* parent /*= nullptr*/ ) :
QWidget(parent), widgetCustomTitleBar(nullptr), widgetSources(nullptr)
{
    setupUi(this);
    buttonClose->setDefaultAction(actionClose);
    buttonFloat->setDefaultAction(actionFloat);
    buttonSplitH->setDefaultAction(actionSplitHorizontally);
    buttonSplitV->setDefaultAction(actionSplitVertically);
    comboType->clear();
}

VisualizerTitleBar::~VisualizerTitleBar()
{

}

void VisualizerTitleBar::addVisualizer( const QString& label, UniqueID id )
{
    comboType->addItem( label, qVariantFromValue(id) );
}

void VisualizerTitleBar::addVisualizer( const QIcon& icon, const QString& label, UniqueID id )
{
    comboType->addItem( icon, label, qVariantFromValue(id) );
}

void VisualizerTitleBar::clear()
{
    setWidgetSources(nullptr);

    // usuniêcie wizualizatora
    visualizer.reset();

    // czysczenie fragmentu z customowymi akcjami
    QLayoutItem* item;
    while ((item = layoutCustom->takeAt(0)) != nullptr) {
        QWidget* widget = item->widget();
        delete item;
        delete widget;
    }

    //layoutCustom->removeWidget(widgetCustomTitleBar);
    utils::deletePtr(widgetCustomTitleBar);

}

void VisualizerTitleBar::setCurrentVisualizer( UniqueID id )
{
    if ( !visualizer || visualizer->getID() != id ) {
        setCurrentVisualizer( static_cast<VisualizerManager*>(getVisualizerManager())->createVisualizer(id) );
    }
}

void VisualizerTitleBar::setCurrentVisualizer( const VisualizerPtr& visualizer )
{
    if ( this->visualizer != visualizer ) {
        // wyczyszczenie
        clear();

        // pobranie nowego wizualizatora
        this->visualizer = visualizer;
        // dodanie g³ównego widgetu
        if ( visualizer ) {

            // czy indeks siê zgadza?
            // tradycyjne wyszukiwanie nie dzia³a przy copy by value
            int idx = -1;
            UniqueID id = visualizer->getID();
            for ( int i = 0; i < comboType->count() && idx < 0; ++i ) {
                if ( id == comboType->itemData(i).value<UniqueID>() ) {
                    idx = i;
                }
            }
            UTILS_ASSERT(idx >= 0);
            comboType->setCurrentIndex(idx);

            // TODO: dodanie widgetu tytu³owego
            // TODO: kto zarz¹dza customWidgettem?
            // dodanie widgetu z wyborem Ÿród³a
            // TODO: wybór na podstawie wizualizatora
            QWidget* sources = new VisualizerTitleBarComboPick();
            setWidgetSources(sources);

            // dodajemy akcje ogólne widgetu
            BOOST_FOREACH(QObject* obj, visualizer->getOrCreateGenericActions()) {
                QAction* action = qobject_cast<QAction*>(obj);
                if ( action ) {
                    // dodajemy guzik z akcj¹
                    QToolButton* button = new QToolButton(this);
                    button->setDefaultAction(action);
                    layoutCustom->addWidget(button);
                } else {
                    QMenu* menu = qobject_cast<QMenu*>(obj);
                    if ( menu ) {
                        // dodajemy ca³e menu
                        QToolButton* button = new QToolButton(this);
                        button->setText(menu->title());
                        button->setPopupMode(QToolButton::InstantPopup);
                        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
                        button->setMenu(menu);
                        layoutCustom->addWidget(button);
                    } else {
                        UTILS_FAIL("what?");
                    }
                }
            }
        }

        // wysy³amy sygna³
        emit currentVisualizerChanged(visualizer);
    }
}

void VisualizerTitleBar::setCurrentVisualizer( int idx )
{
    if ( sender() != comboType ) {
        if ( comboType->currentIndex() != idx ) {
            // przekierowanie slotu
            comboType->setCurrentIndex(idx);
        }
    } else {
        if ( idx >= 0 ) {
            UniqueID id = comboType->itemData(idx).value<UniqueID>();
            setCurrentVisualizer(id);
        } else {
            setCurrentVisualizer( VisualizerPtr() );
        }
    }
}

void VisualizerTitleBar::setWidgetSources( QWidget* widgetSources )
{
    if ( this->widgetSources ) {
        // widget sam powinien zostaæ usuniêty...
        delete this->widgetSources;
    }
    this->widgetSources = widgetSources;
    if (widgetSources) {
        if ( !connect(this, SIGNAL(currentVisualizerChanged(VisualizerPtr)), widgetSources, SLOT(setCurrentVisualizer(VisualizerPtr))) ) {
            UTILS_FAIL("Nie uda³o siê pod³¹czyæ slotu.");
        }
        widgetSources->setParent(frameSources);
        layoutSources->addWidget(widgetSources);
    }
}

void VisualizerTitleBar::removeAllVisualizers()
{
    while (comboType->count()) {
        comboType->removeItem(0);
    }
}

int VisualizerTitleBar::getNumVisualizers() const
{
    return comboType->count();
}

void VisualizerTitleBar::paintEvent( QPaintEvent *paintEvent )
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this); // PE_Widget
}