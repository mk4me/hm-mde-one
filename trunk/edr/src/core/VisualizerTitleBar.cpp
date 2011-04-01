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

    // usuni�cie wizualizatora
    visualizer.reset();

    //layoutCustom->removeWidget(widgetCustomTitleBar);
    utils::deletePtr(widgetCustomTitleBar);
}

void VisualizerTitleBar::setCurrentVisualizer( UniqueID id )
{
    if ( !visualizer || visualizer->getID() != id ) {
        setCurrentVisualizer( VisualizerManager::getInstance()->createVisualizer(id) );
    }
}

void VisualizerTitleBar::setCurrentVisualizer( const VisualizerPtr& visualizer )
{
    if ( this->visualizer != visualizer ) {
        // wyczyszczenie
        clear();

        // pobranie nowego wizualizatora
        this->visualizer = visualizer;
        // dodanie g��wnego widgetu
        if ( visualizer ) {

            // czy indeks si� zgadza?
            // tradycyjne wyszukiwanie nie dzia�a przy copy by value
            int idx = -1;
            UniqueID id = visualizer->getID();
            for ( int i = 0; i < comboType->count() && idx < 0; ++i ) {
                if ( id == comboType->itemData(i).value<UniqueID>() ) {
                    idx = i;
                }
            }
            UTILS_ASSERT(idx >= 0);
            comboType->setCurrentIndex(idx);

            // TODO: dodanie widgetu tytu�owego
            // TODO: kto zarz�dza customWidgettem?
            // dodanie widgetu z wyborem �r�d�a
            // TODO: wyb�r na podstawie wizualizatora
            QWidget* sources = new VisualizerTitleBarComboPick();
            setWidgetSources(sources);
        }

        // wysy�amy sygna�
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
        // widget sam powinien zosta� usuni�ty...
        delete this->widgetSources;
    }
    this->widgetSources = widgetSources;
    if (widgetSources) {
        if ( !connect(this, SIGNAL(currentVisualizerChanged(VisualizerPtr)), widgetSources, SLOT(setCurrentVisualizer(VisualizerPtr))) ) {
            UTILS_FAIL("Nie uda�o si� pod��czy� slotu.");
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