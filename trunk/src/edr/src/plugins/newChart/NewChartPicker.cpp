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

NewChartPicker::NewChartPicker( QwtPlot *plot ):
    QObject( plot ),
    plot(plot)
{
    QwtPlotCanvas *canvas = plot->canvas();
    canvas->installEventFilter( this );
    canvas->setFocusPolicy( Qt::StrongFocus );
    canvas->setFocusIndicator( QwtPlotCanvas::ItemFocusIndicator );
    canvas->setFocus();
}

bool NewChartPicker::eventFilter( QObject *object, QEvent *event )
{
    if ( plot == NULL || object != plot->canvas() ) {
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
    QwtPlotCurve *curve = NULL;
    double dist = 10e10;
    int index = -1;

    const QwtPlotItemList& itmList = plot->itemList();
    for ( QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it ) {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *c = ( QwtPlotCurve* )( *it );

            double d;
            int idx = c->closestPoint( pos, &d );
            if ( d < dist )
            {
                curve = c;
                index = idx;
                dist = d;
            }
        }
    }

    if ( curve && dist < 10 ) {
        emit serieSelected(static_cast<QwtPlotItem*>(curve));
    }
}