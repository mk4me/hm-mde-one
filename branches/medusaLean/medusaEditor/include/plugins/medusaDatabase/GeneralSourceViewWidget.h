/********************************************************************
	created:  2014/08/20	17:16:33
	filename: GeneralSourceViewWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD__medusaDatabase_GENERALSOURCEVIEWWIDGET_H__
#define __HEADER_GUARD__medusaDatabase_GENERALSOURCEVIEWWIDGET_H__

#include <plugins/medusaDatabase/Export.h>
#include <QtWidgets/QWidget>
#include <plugins/medusaDatabase/IHMDBShallowCopyContext.h>

class DataViewConfigurationWidget;

namespace Ui
{
	class GeneralSourceViewWidget;
}

namespace medusaDatabase
{
	class IDataSourcePerspective;
	class IDataSourceContent;
}

class DataViewWidget;
class OperationProgressWidget;

class medusaDatabase_EXPORT GeneralSourceViewWidget : public QWidget
{
	Q_OBJECT;

public:
	GeneralSourceViewWidget(medusaDatabase::IHMDBShallowCopyContextPtr shallowCopyContext,
		QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~GeneralSourceViewWidget();

	DataViewConfigurationWidget * dataViewConfiguration();
	DataViewWidget * dataView();

private Q_SLOTS:

	void onOperationStarts();
	void onOperationEnds();

	void onPerspectiveChanged();
	void onContentChanged();
	void onFilterChanged();

private:
	Ui::GeneralSourceViewWidget * ui;
	DataViewWidget * dataView_;
	DataViewConfigurationWidget * dataViewConfiguration_;
	OperationProgressWidget * operationProgress;

};

#endif	// __HEADER_GUARD___IMUSOURCEVIEWWIDGET_H__