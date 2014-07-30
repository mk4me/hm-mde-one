#include "CommunicationPCH.h"
#include "ViewConfigurationWidget.h"
#include "ui_ViewConfigurationWidget.h"

ViewConfigurationWidget::ViewConfigurationWidget(QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::ViewConfigurationWidget)
{
	ui->setupUi(this);
}

ViewConfigurationWidget::~ViewConfigurationWidget()
{

}

const int ViewConfigurationWidget::viewIdx() const
{

}

const int ViewConfigurationWidget::configurationIdx() const
{

}

void ViewConfigurationWidget::setViewIdx(const int viewIdx)
{

}

void ViewConfigurationWidget::setConfigurationIdx(const int cfgIdx)
{

}

void ViewConfigurationWidget::resetViewIdx()
{

}

void ViewConfigurationWidget::resetConfigurationIdx()
{

}