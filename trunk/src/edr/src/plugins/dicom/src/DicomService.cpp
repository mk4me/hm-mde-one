#include "DicomPCH.h"
#include "DicomService.h"
#include "corelib/IHierarchyItem.h"
#include "plugins/hmdbCommunication/IHMDBSource.h"
#include "plugins/hmdbCommunication/IHMDBSourceViewManager.h"
#include "hmdbserviceslib/IAuthorizationWS.h"
#include "plugins/dicom/AnnotationStatusManager.h"


dicom::DicomService::DicomService() : 
	sourceManager(nullptr), hierarchyManager(nullptr), perspective(new DicomPerspective)
{

}


dicom::DicomService::~DicomService()
{

}



void dicom::DicomService::updateItemIcon(const std::string& filename, const QIcon& icon)
{
	auto item = perspective->tryGetHierarchyItem(filename);
	if (item) {
		item->setIcon(icon);
		auto hierarchyTransaction = hierarchyManager->transaction();
		core::IHierarchyItemConstPtr root = item;
		while (root->getParent() && root->getParent()->getParent()) {
			root = root->getParent();
		}
		if (root) {
			hierarchyTransaction->updateRoot(root);
		}
	}
}

QWidgetList dicom::DicomService::getPropertiesWidgets()
{
	return QWidgetList();
}

QWidget* dicom::DicomService::getWidget()
{
	return nullptr;
}

void dicom::DicomService::update(double deltaTime)
{

}

void dicom::DicomService::finalize()
{
	statusManager.reset(nullptr);
}

const bool dicom::DicomService::userIsReviewer(hmdbCommunication::IHMDBSession * session)
{
	static std::map<hmdbCommunication::IHMDBSession *, bool> memoMap;

	auto it = memoMap.find(session);
	if (it != memoMap.end()) {
		return it->second;
	}


	bool ret = false;

	if (session != nullptr) {
		try {
			auto lst = session->authorization()->listMyUserGroupsAssigned();
			auto userGroups = hmdbServices::xmlWsdl::parseUserGroups(lst);
			auto it = std::find_if(userGroups.begin(), userGroups.end(), [](const hmdbServices::xmlWsdl::UserGroup & ug)
			{
				if (ug.id == 11 || ug.id == 12) {
					return true;
				}
				return false;
			});

			if (it != userGroups.end()) {
				ret = true;
			}


			memoMap[session] = ret;

		}
		catch (...) {

		}
	}

	return ret;
}


const bool dicom::DicomService::lateInit()
{
	auto comm = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
	if (comm != nullptr) {
		comm->viewManager()->addHierarchyPerspective(perspective);
		return true;
	}

	return false;
}

void dicom::DicomService::init(core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager, core::IDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager, core::IDataHierarchyManager * hierarchyManager)
{
	this->sourceManager = sourceManager;
	this->hierarchyManager = hierarchyManager;
}

dicom::AnnotationStatusManager* dicom::DicomService::getAnnotationStatusManager()
{
	std::unique_lock<std::mutex> ul(statusMutex);
	if (!statusManager) {
		statusManager.reset(new AnnotationStatusManager);
	}
	return statusManager.get();
}


