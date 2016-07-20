#include "NewChartPCH.h"
#include <QtWidgets/QHBoxLayout>
#include "NewChartHelpers.h"


template <class Spin, typename T>
std::pair<QWidget*, Spin*> createSB(const QString& label, T singleStep, T minimum, T maximum)
{
	std::pair<QWidget*, Spin*> ret;
	ret.first = new QWidget();
	auto layout = new QHBoxLayout();
	layout->setContentsMargins(3, 0, 3, 0);
	layout->setSpacing(3);
	ret.first->setLayout(layout);
	QLabel* l = new QLabel(label);
	layout->addWidget(l);
	Spin* spin = new Spin();

	spin->setMaximum(maximum);
	spin->setMinimum(minimum);
	spin->setSingleStep(singleStep);
	spin->setMaximumWidth(90);
	layout->addWidget(spin);
	l->setBuddy(spin);
	ret.second = spin;
	return ret;
}

std::pair<QWidget*, QDoubleSpinBox*> LabeledSpinbox::create(const QString& label, double singleStep, double minimum, double maximum)
{
	return createSB<QDoubleSpinBox, double>(label, singleStep, minimum, maximum);
}

std::pair<QWidget*, QSpinBox*> LabeledSpinbox::create(const QString& label, int singleStep, int minimum, int maximum)
{
	return createSB<QSpinBox, int>(label, singleStep, minimum, maximum);
}

