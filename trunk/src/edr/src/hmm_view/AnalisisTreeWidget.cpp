#include "hmmPCH.h"
#include "AnalisisTreeWidget.h"
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>

void AnalisisTreeWidget::contextMenuEvent( QContextMenuEvent * event )
{
    QTreeWidget::contextMenuEvent(event);
}

void AnalisisTreeWidget::mousePressEvent( QMouseEvent *event )
{
    if (event->button() == Qt::RightButton) {
        return;
    }

    QTreeWidget::mousePressEvent(event);
}

AnalisisTreeWidget::AnalisisTreeWidget( QWidget *parent /*= 0*/ ) :
QTreeWidget(parent)
{
    setHeaderHidden(true);
    setFrameShape(QFrame::NoFrame);
}

