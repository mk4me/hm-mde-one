#include "NewVdfPCH.h"
#include "MergedWidget.h"
#include <QtGui/QListWidget>
#include <QtGui/QLayout>

vdf::MergedWidget::MergedWidget()
{
    setLayout(new QVBoxLayout());
    QWidget* down = new QWidget();
    down->setLayout(new QHBoxLayout());
    down->layout()->addWidget(new QLabel(tr("Merged items")));
    listWidget = new QListWidget();
    layout()->addWidget(listWidget);
    layout()->addWidget(down);
}
