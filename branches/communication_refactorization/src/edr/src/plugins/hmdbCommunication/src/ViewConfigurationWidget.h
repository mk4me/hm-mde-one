/********************************************************************
	created:  2014/07/23	14:58:32
	filename: ViewConfigurationWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__VIEWCONFIGURATIONWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__VIEWCONFIGURATIONWIDGET_H__

#include <QtGui/QWidget>

namespace Ui
{
	class ViewConfigurationWidget;
}

class ViewConfigurationWidget : public QWidget
{

	Q_OBJECT
	Q_PROPERTY(int viewIdx READ viewIdx WRITE setViewIdx RESET resetViewIdx NOTIFY viewIdxChanged)
	Q_PROPERTY(int configurationIdx READ configurationIdx WRITE setConfigurationIdx RESET resetConfigurationIdx NOTIFY configurationIdxChanged)

public:
	ViewConfigurationWidget(QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~ViewConfigurationWidget();

	const int viewIdx() const;
	const int configurationIdx() const;

public slots:

	void setViewIdx(const int viewIdx);
	void setConfigurationIdx(const int cfgIdx);

	void resetViewIdx();
	void resetConfigurationIdx();

signals:

	void viewIdxChanged(const int viewIdx);
	void configurationIdxChanged(const int cfgIdx);

private:

	Ui::ViewConfigurationWidget * ui;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__VIEWCONFIGURATIONWIDGET_H__