#include "MdePCH.h"
#include "IMUSourceViewWidget.h"
#include "ui_IMUSourceViewWidget.h"
#include <plugins/hmdbCommunication/DataViewWidget.h>
#include <plugins/hmdbCommunication/OperationProgressWidget.h>
#include <plugins/hmdbCommunication/IDataSourcePerspective.h>
#include <plugins/hmdbCommunication/IDataSourceContent.h>

IMUSourceViewWidget::IMUSourceViewWidget(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext,
	hmdbCommunication::IDataSourcePerspective * perspective,
	hmdbCommunication::IDataSourceContent * content, QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::IMUSourceViewWidget), dataView(nullptr),
	operationProgress(nullptr)
{
	ui->setupUi(this);

	dataView = new DataViewWidget(shallowCopyContext, perspective, content);
	operationProgress = new OperationProgressWidget;
	operationProgress->setVisible(false);
	QVBoxLayout * l = new QVBoxLayout;
	l->addWidget(dataView);
	l->addWidget(operationProgress);
	setLayout(l);

	auto res = connect(dataView, SIGNAL(operationAboutToStart()), this, SLOT(onOperationStarts()));
	res = connect(dataView, SIGNAL(operationFinished()), this, SLOT(onOperationEnds()));
}

IMUSourceViewWidget::~IMUSourceViewWidget()
{

}

void IMUSourceViewWidget::onOperationStarts()
{	
	operationProgress->setOperation(dataView->operation());
	operationProgress->setVisible(true);
}

void IMUSourceViewWidget::onOperationEnds()
{
	operationProgress->setVisible(false);
	operationProgress->setOperation(nullptr);
}