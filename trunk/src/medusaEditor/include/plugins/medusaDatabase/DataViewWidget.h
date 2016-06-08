/********************************************************************
	created:  2014/07/25	10:20:55
	filename: DataViewWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_medusaDatabase__DATAVIEWWIDGET_H__
#define __HEADER_GUARD_medusaDatabase__DATAVIEWWIDGET_H__

#include <plugins/medusaDatabase/Export.h>
#include <QtWidgets/QWidget>
#include <plugins/medusaDatabase/ShallowCopy.h>
#include <plugins/medusaDatabase/IHMDBRemoteContext.h>
#include <plugins/medusaDatabase/IHMDBShallowCopyContext.h>
#include <plugins/medusaDatabase/IHMDBStatusManager.h>
#include <coreui/CoreCursorChanger.h>
#include <plugins/medusaDatabase/DataStatus.h>
#include <plugins/medusaDatabase/Types.h>
#include <corelib/IThreadPool.h>

class QTreeWidgetItem;

namespace Ui
{
	class DataViewWidget;
}

namespace medusaDatabase
{
	class IHMDBShallowCopyContext;
	class IDataSourcePerspective;	
	class IDataSourceContent;
	class ShallowCopyFilter;
}

Q_DECLARE_OPAQUE_POINTER(medusaDatabase::IDataSourcePerspective*);
Q_DECLARE_OPAQUE_POINTER(medusaDatabase::IDataSourceContent*);
Q_DECLARE_OPAQUE_POINTER(medusaDatabase::ShallowCopyFilter*);
Q_DECLARE_METATYPE(hmdbServices::ID);
Q_DECLARE_METATYPE(medusaDatabase::DataType);

class medusaDatabase_EXPORT DataViewWidget : public QWidget
{
	Q_OBJECT

public:

	class IOperation : public threadingUtils::INormalizedProgress, public threadingUtils::IAbortable
	{
	public:
		virtual ~IOperation() {}
		virtual QString name() const = 0;
	};

	DEFINE_SMART_POINTERS(IOperation);

public:

	DataViewWidget(medusaDatabase::IHMDBShallowCopyContextPtr shallowCopyContext,
		QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~DataViewWidget();

	IOperation * operation();

	medusaDatabase::ShallowCopyConstPtr currentShallowCopy() const;
	medusaDatabase::ShallowCopyConstPtr completeShallowCopy() const;

public slots:

	void selectItem(char type, int id);

	void setPerspective(medusaDatabase::IDataSourcePerspective * perspective);
	void setContent(medusaDatabase::IDataSourceContent * content);
	void setFilter(medusaDatabase::ShallowCopyFilter * filter);

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
	void onUpdateModifiedTrials();

signals:

	void currentItemChanged(char currentType, int currentId, char previousType, int previousId);
	void operationAboutToStart();
	void operationFinished();
	void shallowCopyChanged();
	void modifiedTrialsPresent();

private:
	static void extractItem(const QTreeWidgetItem * item, int & id, char & type);

	void doShallowCopyFiltering();
	void rawFilterShallowCopy();
	void filterShallowCopy();
	void setShallowCopy(const medusaDatabase::ShallowCopyConstPtr shallowCopy);
	void extractLocalShallowCopy();
	void noShallowCopyInStorage();
	void noShallowCopyInStorageAndLocalOnly();
	void extractLocalShallowCopyAndTryUpdate();

	void refreshCurrentContent(bool perspectiveFirst);	

	medusaDatabase::DataStatus refrshItemContent(QTreeWidgetItem * item,
		const bool perspectiveFirst);

	medusaDatabase::DataStatus refreshDataStatus(QTreeWidgetItem * item,
		const bool perspectiveFirst);
	void refreshDataStatus(const bool perspectiveFirst);	
	bool tryRebuildDataStatus();

	QIcon statusIcon(const medusaDatabase::DataStatus dataStatus);

	void onSynchronize();
	

	void initializeActions();
	void setDefaultPerspectiveHeaders();

	void synchronize(medusaDatabase::IHMDBShallowCopyRemoteContext::SynchronizeOperationPtr sOp,
		utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger);
	void download(medusaDatabase::IHMDBRemoteContext::OperationPtr dOp,
		utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger);

	void setupDownload(const medusaDatabase::StorageFileNames & files);

	
	medusaDatabase::StorageFileNames filterModifiedTrialFiles(medusaDatabase::IncrementalBranchShallowCopyConstPtr incShallowCopy);
	bool isTrialLocal(const hmdbServices::IncrementalBranchShallowCopy::Trial& trial,
		medusaDatabase::IHMDBStatusManager::TransactionConstPtr transaction);

private:
	core::Thread remoteOperationThread;
	core::Thread filteringThread;
	medusaDatabase::ShallowCopyConstPtr currentShallowCopy_;
	medusaDatabase::ShallowCopyConstPtr completeShallowCopy_;
	medusaDatabase::IDataSourcePerspective * perspective_;
	medusaDatabase::IDataSourceContent * content_;
	medusaDatabase::ShallowCopyFilter * filter_;
	medusaDatabase::IHMDBShallowCopyContextPtr shallowCopyContext_;	
	Ui::DataViewWidget * ui;
	IOperationPtr operation_;
	std::map<medusaDatabase::DataStatus, QIcon> icons;

	medusaDatabase::StorageFileNames modifiedTrialsFiles;
};


#endif	// __HEADER_GUARD_medusaDatabase__DATAVIEWWIDGET_H__
