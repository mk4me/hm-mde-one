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
    //treeWidget->setRootIsDecorated(false);
    treeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    treeHolder->layout()->addWidget(treeWidget);
    connect(hideButton, SIGNAL(clicked(bool)), this, SLOT(setFiltersExpanded(bool)));
    setFiltersExpanded(hideButton->isChecked());
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
        containerFrame->setMaximumWidth(w + 22);
        containerFrame->setMinimumWidth(w + 22);
        scrollArea->setMinimumHeight(3 * margin + filterHeight * 2);
    }

    filter->setParent(filterScroll);
    filter->setGeometry(margin + x * (filterWidth + margin),margin +  y * (margin + filterHeight), filterWidth, filterHeight);
}

bool AnalisisWidget::eventFilter( QObject* object, QEvent* event )
{
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouse = static_cast<QMouseEvent*>(event);
        lastMouseButton = mouse->button();
    }

    return false;
}

void AnalisisWidget::setFiltersExpanded( bool expand )
{
    scrollArea->setHidden(!expand);
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
    //setStyleSheet("border: 0px;");
}

