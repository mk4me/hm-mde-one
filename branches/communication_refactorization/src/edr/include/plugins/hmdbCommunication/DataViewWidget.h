/********************************************************************
	created:  2014/07/25	10:20:55
	filename: DataViewWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWWIDGET_H__

#include <plugins/hmdbCommunication/Export.h>
#include <QtWidgets/QWidget>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <coreui/CoreCursorChanger.h>
#include <corelib/IThread.h>
#include <plugins/hmdbCommunication/DataStatus.h>

class QTreeWidgetItem;

namespace Ui
{
	class DataViewWidget;
}

namespace hmdbCommunication
{
	class IHMDBShallowCopyContext;
	class IDataSourcePerspective;	
	class IDataSourceContent;
}

class HMDBCOMMUNICATION_EXPORT DataViewWidget : public QWidget
{
	Q_OBJECT

public:

	DataViewWidget(hmdbCommunication::IHMDBShallowCopyContext * shallowCopyContext,		
		hmdbCommunication::IDataSourcePerspective * perspective = nullptr,
		hmdbCommunication::IDataSourceContent * content = nullptr,		
		QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~DataViewWidget();

	void setPerspective(hmdbCommunication::IDataSourcePerspective * perspective);
	void setContent(hmdbCommunication::IDataSourceContent * content);
	void setShallowCopy(const hmdbCommunication::ShallowCopyConstPtr shallowCopy);	

	const hmdbCommunication::IHMDBRemoteContext::OperationConstPtr operation() const;

public slots:

	void selectItem(char type, int id);

private slots:

	void onLoad();
	void onLoadAll();
	void onUnload();
	void onUnloadAll();
	void onRebuild();
	void onRefreshStatus();
	void onTrySynchronize();
	void onDownload();
	void onDownloadAll();
	void onForceDownload();
	void onForceDownloadAll();
	void onTreeCurrentItemChange(QTreeWidgetItem* previous, QTreeWidgetItem* current);
	void onExtractLocalShallowCopy();
	void onNoShallowCopyInStorage();
	void onNoShallowCopyInStorageAndLocalOnly();
	void onExtractLocalShallowCopyAndTryUpdate();
	void setRemoteOperationsEnabled(const bool enable);
	void setLocalOperationsEnabled(const bool enable);
	void rebuildPerspective();
	void refreshContent();
	void onSynchronizeFinished();

signals:

	void currentItemChanged(char previousType, int previousId, char currentType, int currentId);
	void operationAboutToStart(char type);

private:

	void refreshCurrentContent(bool perspectiveFirst);	

	const hmdbCommunication::DataStatus DataViewWidget::refrshItemContent(QTreeWidgetItem * item,
		bool perspectiveFirst);

	const QIcon statusIcon(const hmdbCommunication::DataStatus dataStatus);

	void onSynchronize();
	void initializeActions();
	void initializeShallowCopy();
	void setDefaultPerspectiveHeaders();

	void synchronize(hmdbCommunication::IHMDBRemoteContext::SynchronizeOperationPtr sOp,
		utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger);

private:
	core::IThreadPtr thread;
	hmdbCommunication::ShallowCopyConstPtr shallowCopy_;
	hmdbCommunication::IDataSourcePerspective * perspective_;
	hmdbCommunication::IDataSourceContent * content_;
	hmdbCommunication::IHMDBShallowCopyContext * shallowCopyContext_;	
	Ui::DataViewWidget * ui;
	hmdbCommunication::IHMDBRemoteContext::OperationConstPtr operation_;
	bool delPerspective;
	bool delContent;
	std::map<hmdbCommunication::DataStatus, QIcon> icons;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWWIDGET_H__