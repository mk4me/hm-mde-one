#include "hmmPCH.h"
#include "AnalisisWidget.h"

AnalisisWidget::AnalisisWidget( QWidget* parent, int margin /*= 2*/, Qt::WindowFlags flags /*= 0*/ ) : 
QWidget(parent, flags),
    margin(margin),
    lastMouseButton(Qt::NoButton),
    filterWidth(-1), filterHeight(-1)
    
{
    setupUi(this);
    if (!treeHolder->layout()) {
        QLayout* layout = new QVBoxLayout();
        layout->setMargin(0);
        layout->setContentsMargins(QMargins(0, 0, 0, 0));
        treeHolder->setLayout(layout);
    }
    treeWidget = new AnalisisTreeWidget(treeHolder);
    treeHolder->layout()->addWidget(treeWidget);
}

void AnalisisWidget::addDataFilterWidget( DataFilterWidget* filter )
{ 
    if (filterWidth < 0 && filterHeight < 0) {
        filterWidth = filter->width();
        filterHeight = filter->height();
    }

    UTILS_ASSERT(filterHeight == filter->height() && filterWidth == filter->width());


    int count = filterScroll->children().size();
    int x = count % 2;
    int y = count / 2;

    if (x == 0) {
        int w = 3 * margin + filterWidth * 2;
        int h = 2 * margin + (filterHeight + margin) * (y + 1);
        filterScroll->setMinimumSize(w, h);
        scrollArea->setMinimumWidth(w + 16);
        scrollArea->setMaximumWidth(w + 16);
        frame->setMaximumWidth     (w + 16);
        scrollArea->setMinimumHeight(3 * margin + filterHeight * 2 + 8);
    }

    filter->setParent(filterScroll);
    filter->setGeometry(margin + x * (filterWidth + margin),margin +  y * (margin + filterHeight), filterWidth, filterHeight);
}


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
