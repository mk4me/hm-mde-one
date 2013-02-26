#include "NewChartPCH.h"
#include <QtGui/QHBoxLayout>
#include "NewChartHelpers.h"

std::pair<QWidget*, QDoubleSpinBox*> LabeledSpinbox::create(const QString& label, double singleStep, double minimum, double maximum)
{
	std::pair<QWidget*, QDoubleSpinBox*> ret;
    ret.first = new QWidget();
	auto layout = new QHBoxLayout();
	layout->setContentsMargins(3,0,3,0);
	layout->setSpacing(3);
    ret.first->setLayout(layout);
    QLabel* l = new QLabel(label);
    layout->addWidget(l);
    QDoubleSpinBox* spin = new QDoubleSpinBox();

    spin->setMaximum(maximum);
    spin->setMinimum(minimum);
    spin->setSingleStep(singleStep);
    spin->setMaximumWidth(90);
    layout->addWidget(spin);
	l->setBuddy(spin);
	ret.second = spin;
    return ret;
}

