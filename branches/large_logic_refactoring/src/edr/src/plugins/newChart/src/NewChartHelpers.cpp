#include "NewChartPCH.h"
#include <QtGui/QHBoxLayout>
#include "NewChartHelpers.h"

std::pair<QWidget*, QDoubleSpinBox*> LabeledSpinbox::create(const QString& label, double singleStep, double minimum, double maximum)
{
	std::pair<QWidget*, QDoubleSpinBox*> ret;
    ret.first = new QWidget();
    ret.first->setLayout(new QHBoxLayout());
	auto layout = ret.first->layout();
    QLabel* l = new QLabel(label);
    layout->addWidget(l);
    QDoubleSpinBox* spin = new QDoubleSpinBox();

    spin->setMaximum(maximum);
    spin->setMinimum(minimum);
    spin->setSingleStep(singleStep);
    spin->setMaximumWidth(90);
    layout->addWidget(spin);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
	ret.second = spin;
    return ret;
}

