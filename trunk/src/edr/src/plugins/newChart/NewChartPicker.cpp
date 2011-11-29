#include "NewChartPCH.h"
#include <QtGui/QApplication>
#include <QtCore/QEvent>
#include <QtGui/QPainter>
#include <qwt/qwt_plot.h>
#include <QtGui/QMouseEvent>
#include <qwt/qwt_scale_map.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_directpainter.h>
#include "NewChartPicker.h"
#include "NewChartVisualizer.h"

NewChartPicker::NewChartPicker( NewChartVisualizer* visualizer):
    NewChartState( visualizer ),
    pixelTolerance(20)
{
    canvas = visualizer->getPlot()->canvas();
    canvas->installEventFilter( this );
    //canvas->setFocusPolicy( Qt::StrongFocus );
    canvas->setFocusIndicator( QwtPlotCanvas::ItemFocusIndicator );
    canvas->setFocus();
}

bool NewChartPicker::stateEventFilter( QObject *object, QEvent *event )
{
    if (object != canvas ) {
        return false;
    }

    switch( event->type() ) {
        case QEvent::Paint: {
            QApplication::postEvent( this, new QEvent( QEvent::User ) );
            break;
        }
        case QEvent::MouseButtonPress: {
            select( ( ( QMouseEvent * )event )->pos() );
        }
        default:
            break;
    }

    return QObject::eventFilter( object, event );
}

void NewChartPicker::select( const QPoint &pos )
{
    QwtPlotCurve *curve = nullptr;
    double dist = 10e10;
    const QwtPlotItemList& itmList = visualizer->getPlot()->itemList();
    for ( QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it ) {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *c = ( QwtPlotCurve* )( *it );

            double d;
            c->closestPoint( pos, &d );
            if ( d < dist )
            {
                curve = c;
                dist = d;
            }
        }
    }

    if ( curve && dist < pixelTolerance ) {
        emit serieSelected(static_cast<QwtPlotItem*>(curve));
    }
}

void NewChartPicker::stateBegin()
{
    visualizer->setManipulation(true);
    canvas->setCursor(Qt::DragLinkCursor);
}
