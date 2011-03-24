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
    comboType->clear();
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
    setCurrentVisualizer( VisualizerManager::getInstance()->createVisualizer(id) );
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
        layoutSources->removeWidget(this->widgetSources);
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