/********************************************************************
	created:  2014/07/25	10:20:55
	filename: DataViewWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWWIDGET_H__

#include <QtGui/QWidget>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <coreui/CoreCursorChanger.h>
#include <corelib/IThread.h>

class QTreeWidgetItem;

namespace Ui
{
	class DataViewWidget;
}

namespace hmdbCommunication
{
	class IHMDBShallowCopyLocalContext;
	class IHMDBShallowCopyRemoteContext;
	class IDataSourcePerspective;	
}

class DataViewWidget : public QWidget
{
	Q_OBJECT

public:

	DataViewWidget(hmdbCommunication::IDataSourcePerspective * perspective,
		hmdbCommunication::IHMDBShallowCopyLocalContext * shallowLocalContext,
		hmdbCommunication::IHMDBShallowCopyRemoteContext * shallowRemoteContext = nullptr,
		QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~DataViewWidget();

	void setPerspective(hmdbCommunication::IDataSourcePerspective * perspective);
	void setShallowCopy(const hmdbCommunication::ShallowCopyConstPtr shallowCopy);
	void setShallowRemoteContext(hmdbCommunication::IHMDBShallowCopyRemoteContext * shallowRemoteContext);

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
	void onSynchronize();
	void onDownload();
	void onDownloadAll();
	void onForceDownload();
	void onForceDownloadAll();
	void onTreeCurrentItemChange(QTreeWidgetItem* previous, QTreeWidgetItem* current);

signals:

	void currentItemChanged(char previousType, int previousId, char currentType, int currentId);
	void operationAboutToStart(char type);

private:

	void setRemoteOperationsEnabled(const bool enable);
	void setLocalOperationsEnabled(const bool enable);
	void rebuildPerspective();

	void synchronize(hmdbCommunication::IHMDBRemoteContext::SynchronizeOperationPtr sOp,
		utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger);

private:
	core::IThreadPtr thread;
	hmdbCommunication::ShallowCopyConstPtr shallowCopy_;
	hmdbCommunication::IDataSourcePerspective * perspective_;
	hmdbCommunication::IHMDBShallowCopyLocalContext * shallowLocalContext_;
	hmdbCommunication::IHMDBShallowCopyRemoteContext * shallowRemoteContext_;
	Ui::DataViewWidget * ui;
	hmdbCommunication::IHMDBRemoteContext::OperationConstPtr operation_;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWWIDGET_H__