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
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <coreui/CoreCursorChanger.h>
#include <corelib/IThread.h>
#include <plugins/hmdbCommunication/DataStatus.h>
#include <plugins/hmdbCommunication/Types.h>

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
	class ShallowCopyFilter;
}

Q_DECLARE_OPAQUE_POINTER(hmdbCommunication::IDataSourcePerspective*);
Q_DECLARE_OPAQUE_POINTER(hmdbCommunication::IDataSourceContent*);
Q_DECLARE_OPAQUE_POINTER(hmdbCommunication::ShallowCopyFilter*);
Q_DECLARE_METATYPE(hmdbServices::ID);
Q_DECLARE_METATYPE(hmdbCommunication::DataType);

class HMDBCOMMUNICATION_EXPORT DataViewWidget : public QWidget
{
	Q_OBJECT

public:

	class IOperation : public threadingUtils::IProgress, public threadingUtils::IAbortable
	{
	public:
		virtual ~IOperation() {}
		virtual const QString name() const = 0;
	};

	DEFINE_SMART_POINTERS(IOperation);

public:

	DataViewWidget(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext,		
		hmdbCommunication::IDataSourcePerspective * perspective = nullptr,
		hmdbCommunication::IDataSourceContent * content = nullptr,
		hmdbCommunication::ShallowCopyFilter * filter = nullptr,
		QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~DataViewWidget();

	IOperation * operation();

public slots:

	void selectItem(char type, int id);

	void setPerspective(hmdbCommunication::IDataSourcePerspective * perspective);
	void setContent(hmdbCommunication::IDataSourceContent * content);
	void setFilter(hmdbCommunication::ShallowCopyFilter * filter);

	void rebuildPerspective();
	void refreshContent();	
	void onRefreshStatus();
	void onTrySynchronize();
	void onForceSynchronize();

private slots:

	void onLoad();
	void onLoadAll();
	void onUnload();
	void onUnloadAll();
	void onRebuild();
	void onDownload();
	void onDownloadAll();
	void onForceDownload();
	void onForceDownloadAll();
	void onTreeCurrentItemChange(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void setRemoteOperationsEnabled(const bool enable);
	void setLocalOperationsEnabled(const bool enable);
	void onSynchronizeFinished();
	void onDownloadFinished();
	void initializeShallowCopy();
	void onContextMenu(QPoint position);

signals:

	void currentItemChanged(char currentType, int currentId, char previousType, int previousId);
	void operationAboutToStart();
	void operationFinished();

private:
	static void extractItem(const QTreeWidgetItem * item, char & id, char & type);

	void doShallowCopyFiltering();
	void rawFilterShallowCopy();
	void filterShallowCopy();
	void setShallowCopy(const hmdbCommunication::ShallowCopyConstPtr shallowCopy);
	void extractLocalShallowCopy();
	void noShallowCopyInStorage();
	void noShallowCopyInStorageAndLocalOnly();
	void extractLocalShallowCopyAndTryUpdate();

	void refreshCurrentContent(bool perspectiveFirst);	

	const hmdbCommunication::DataStatus refrshItemContent(QTreeWidgetItem * item,
		const bool perspectiveFirst);

	const hmdbCommunication::DataStatus refreshDataStatus(QTreeWidgetItem * item,
		const bool perspectiveFirst);
	void refreshDataStatus(const bool perspectiveFirst);	
	const bool tryRebuildDataStatus();

	const QIcon statusIcon(const hmdbCommunication::DataStatus dataStatus);

	void onSynchronize();
	void initializeActions();
	void setDefaultPerspectiveHeaders();

	void synchronize(hmdbCommunication::IHMDBShallowCopyRemoteContext::SynchronizeOperationPtr sOp,
		utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger);
	void download(hmdbCommunication::IHMDBRemoteContext::OperationPtr dOp,
		utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger);

	void setupDownload(const hmdbCommunication::StorageFileNames & files);

private:
	core::IThreadPtr remoteOperationThread;
	core::IThreadPtr filteringThread;
	hmdbCommunication::ShallowCopyConstPtr shallowCopy_;
	hmdbCommunication::IDataSourcePerspective * perspective_;
	hmdbCommunication::IDataSourceContent * content_;
	hmdbCommunication::ShallowCopyFilter * filter_;
	hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext_;	
	Ui::DataViewWidget * ui;
	IOperationPtr operation_;
	bool delPerspective;
	bool delContent;
	std::map<hmdbCommunication::DataStatus, QIcon> icons;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWWIDGET_H__