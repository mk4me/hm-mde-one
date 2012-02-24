#include "NewChartPCH.h"
#include <QtGui/QHBoxLayout>
#include "NewChartHelpers.h"

boost::tuple<QWidget*, QLabel*, QDoubleSpinBox*> LabeledSpinbox::create(QWidget* parent, const QString& label, double singleStep, double minimum, double maximum)
{
    QWidget* spinWidget = new QWidget(parent);
    spinWidget->setLayout(new QHBoxLayout());
    QLabel* l = new QLabel(label);
    spinWidget->layout()->addWidget(l);
    QDoubleSpinBox* spin = new QDoubleSpinBox();
    spin->setMaximum(maximum);
    spin->setMinimum(minimum);
    spin->setSingleStep(singleStep);
    spin->setMaximumWidth(90);
    spinWidget->layout()->addWidget(spin);
    spinWidget->layout()->setMargin(0);
    spinWidget->layout()->setContentsMargins(0, 0, 0, 0);
    return boost::make_tuple(spinWidget, l, spin);
}

