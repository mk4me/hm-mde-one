/********************************************************************
    created:  2014/01/02
    created:  2:1:2014   12:27
    filename: IMUCostumeListWidget.h
    author:   Mateusz Janiak
    
    purpose:  Widget zarz¹dzaj¹cy danymi kostiumów
*********************************************************************/
#ifndef HEADER_GUARD_IMU__IMUCOSTUMELISTWIDGET_H__
#define HEADER_GUARD_IMU__IMUCOSTUMELISTWIDGET_H__

#include <QtWidgets/QWidget>

class Ui_IMUCostumeListWidget;

namespace IMU {
	class IMUCostumeDataSource;
}

class IMUCostumeWidget : public QWidget
{

	Q_OBJECT

public:
	IMUCostumeWidget(IMU::IMUCostumeDataSource * ds, QWidget * parent = nullptr, const Qt::WindowFlags f = 0);
	virtual ~IMUCostumeWidget();

private slots:

	void onConnect();

	void onCostumesListContextMenu(const QPoint & position);

	void onLoad();
	void onUnload();

	void onLoadAll();
	void onUnloadAll();

	void onCalibrate();

private:
	IMU::IMUCostumeDataSource * ds;
	Ui_IMUCostumeListWidget * ui;
};

#endif	//	HEADER_GUARD_IMU__IMUCOSTUMELISTWIDGET_H__