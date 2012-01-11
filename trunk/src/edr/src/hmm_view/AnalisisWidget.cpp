#include "hmmPCH.h"
#include "AnalisisWidget.h"

AnalisisWidget::AnalisisWidget( QWidget* parent, HmmMainWindow* hmm, int margin /*= 2*/, Qt::WindowFlags flags /*= 0*/ ) : 
QWidget(parent, flags),
    margin(margin),
    lastMouseButton(Qt::NoButton),
    hmm(hmm),
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
    hideButton->setChecked(!hideButton->isChecked());
    setFiltersExpanded(hideButton->isChecked());
    connect(applyButton, SIGNAL(clicked()), this, SLOT(applyClicked()));
    applyButton->hide();
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
        //containerFrame->setFixedSize(w + 22, h + 22);
        scrollArea->setMinimumHeight(3 * margin + filterHeight * 2);
    }

    filter->setParent(filterScroll);
    filter->setGeometry(margin + x * (filterWidth + margin),margin +  y * (margin + filterHeight), filterWidth, filterHeight);

    connect(filter, SIGNAL( clicked()), this, SLOT(switchToFirstTab()));
    for (int i = 0; i < filter->getNumEntries(); i++) {
        connect(filter->getEntry(i), SIGNAL( onFilterClicked(FilterEntryWidget*)), this, SLOT(filterClicked(FilterEntryWidget*)));
    }
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

void AnalisisWidget::filterClicked( FilterEntryWidget* filter )
{
    currentFilter = filter;
    recreateTree(filter);
    if (configurationWidget->layout()) {
        delete configurationWidget->layout();
    }
    // TODO zrobic to w lepszy sposob...
    const QObjectList& childList = configurationWidget->children();
    for (int i = childList.size() - 1; i >= 0; --i) {
        QWidget* w = qobject_cast<QWidget*>(childList.at(i));
        w->hide();
    }
    configurationWidget->setLayout(new QVBoxLayout());

    QWidget* configurator = filter->getConfigurator();
    if (configurator) {
        
        //QDialog* dialog = new QDialog(this);
        //QHBoxLayout* layout = new QHBoxLayout(dialog);
        //layout->addWidget(configurator);
        //dialog->setLayout(layout);
        //dialog->exec();

        //TODO usuwanie smieci, lub button jako pole klasy
        QLayout* layout = configurationWidget->layout();
        configurator->setVisible(true);
        layout->addWidget(configurator);
        //QWidget* spacer = new QWidget();
        //spacer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        applyButton->setVisible(true);
        applyButton->setText(QString("%1 - Apply").arg(filter->getName()));
        
    } else {
        switchToFirstTab();
        applyButton->hide();
    }
    
}

void AnalisisWidget::applyClicked()
{
    UTILS_ASSERT(currentFilter);
    recreateTree(currentFilter);

    switchToFirstTab();

}

void AnalisisWidget::recreateTree(FilterEntryWidget* filter)
{
    QTreeWidget* tree = getTreeWidget();
    tree->clear();
    QTreeWidgetItem* item = filter->createTreeEntry(hmm->getCurrentSessions());
    tree->addTopLevelItem(item);
    item->setExpanded(true);
}

void AnalisisWidget::switchToFirstTab()
{
    // Hack - to rozwiazanie nie jest odporne na zmiane kolejnosci tabow
    tabWidget->setCurrentIndex(0);
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

