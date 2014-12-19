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
#include <QtCore/QTimer>
#include <utils/ObjectWrapper.h>
#include <threadingUtils/StreamData.h>
#include <imucostumelib/ProtocolSendBufferHelper.h>
#include <imucostumelib/ImuCostume.h>
#include <corelib/Variant.h>

class Ui_IMUCostumeListWidget;
class QTreeWidgetItem;

namespace IMU {
	class IMUCostumeDataSource;
}

class IMUCostumeWidget : public QWidget
{

	Q_OBJECT

private:

	struct CostumeStreams 
	{
		utils::shared_ptr<threadingUtils::IStreamT<imuCostume::ProtocolSendBufferHelper::Buffer>> rawStream;
		utils::shared_ptr<threadingUtils::ResetableStreamStatusObserver> rawStreamObserver;
		utils::shared_ptr<threadingUtils::IStreamT<imuCostume::Costume::Data>> costumeStream;
		utils::shared_ptr<threadingUtils::ResetableStreamStatusObserver> costumeStreamObserver;
	};

public:
	IMUCostumeWidget(IMU::IMUCostumeDataSource * ds, QWidget * parent = nullptr, const Qt::WindowFlags f = 0);
	virtual ~IMUCostumeWidget();

private slots:	

	void onCostumesListContextMenu(const QPoint & position);
	void onCostumeChange(QTreeWidgetItem * current, QTreeWidgetItem * previous);

	void onLoad();
	void onUnload();

	void onLoadAll();
	void onUnloadAll();	

    void onLoadDatFile();

	void testCommunication();        

	void refreshData();

private:
	IMU::IMUCostumeDataSource * ds;
	Ui_IMUCostumeListWidget * ui;
	QTimer refreshTimer;
	std::map<std::string, CostumeStreams> costumeStreams;
};

#endif	//	HEADER_GUARD_IMU__IMUCOSTUMELISTWIDGET_H__