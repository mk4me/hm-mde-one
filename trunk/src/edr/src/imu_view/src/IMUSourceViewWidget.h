/********************************************************************
	created:  2014/08/20	17:16:33
	filename: IMUSourceViewWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___IMUSOURCEVIEWWIDGET_H__
#define __HEADER_GUARD___IMUSOURCEVIEWWIDGET_H__

#include <QWidget>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>

namespace Ui
{
	class IMUSourceViewWidget;
}

namespace hmdbCommunication
{
	class IDataSourcePerspective;
	class IDataSourceContent;
}

class DataViewWidget;
class OperationProgressWidget;

class IMUSourceViewWidget : public QWidget
{
	Q_OBJECT;

public:
	IMUSourceViewWidget(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext,
		hmdbCommunication::IDataSourcePerspective * perspective = nullptr,
		hmdbCommunication::IDataSourceContent * content = nullptr,
		QWidget * parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~IMUSourceViewWidget();

private Q_SLOTS:

	void onOperationStarts();
	void onOperationEnds();

private:
	Ui::IMUSourceViewWidget * ui;
	DataViewWidget * dataView;
	OperationProgressWidget * operationProgress;

};

#endif	// __HEADER_GUARD___IMUSOURCEVIEWWIDGET_H__