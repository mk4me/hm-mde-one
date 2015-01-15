#include "FileSource.h"
#include <corelib/Filesystem.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include "FileSourceWidget.h"
#include "../acclaimformatslib/acclaimformatslib/MotionData.h"
#include "../acclaimformatslib/acclaimformatslib/Skeleton.h"
#include "kinematiclib/Skeleton.h"
#include "kinematiclib/SkeletonState.h"
#include <plugins/kinematic/Wrappers.h>


FileSource::FileSource() :
    fileDM(nullptr),
    memoryDM(nullptr)
{

}


void FileSource::init( core::IMemoryDataManager * memoryDM, core::IStreamDataManager * streamDM, core::IFileDataManager * fileDM )
{
    this->fileDM = fileDM;
    this->memoryDM = memoryDM;
    this->streamDM = streamDM;
}

bool FileSource::lateInit()
{
    return true;
}

void FileSource::finalize()
{
    
}

void FileSource::update( double deltaTime )
{
    
}

QWidget* FileSource::getWidget()
{
    return new FileSourceWidget(this);
}

QWidget* FileSource::getControlWidget()
{
    return nullptr;
}

QWidget* FileSource::getSettingsWidget()
{
    return nullptr;
}

void FileSource::getOfferedTypes( utils::TypeInfoList & offeredTypes ) const
{
    
}

void FileSource::addFile( const core::Filesystem::Path& path )
{
    auto transaction = fileDM->transaction();
    transaction->addFile(path);
    core::ConstVariantsList oList;
    transaction->getObjects(path, oList);

    auto hierarchyTransaction = memoryDM->hierarchyTransaction();
    core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>(path.filename().string().c_str(), path.string().c_str(), QIcon());
    for (auto it = oList.begin(); it != oList.end(); ++it) {
        core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(*it, QString());
        root->appendChild(item);
    }

    hierarchyTransaction->addRoot(root);
}

void FileSource::loadAsfAmc()
{
	auto transaction = fileDM->transaction();
	core::Filesystem::Path p1 = "C:/Users/Wojciech/Desktop/test.amc";
	core::Filesystem::Path p2 = "C:/Users/Wojciech/Desktop/test.asf";
	transaction->addFile(p1);
	transaction->addFile(p2);
	core::ConstVariantsList oList;

	transaction->getObjects(p1, oList);
	auto wrp1 = *oList.begin();
	transaction->getObjects(p2, oList);
	auto wrp2 = *(++oList.begin());

	acclaim::MotionDataConstPtr data = wrp1->get() ;
	acclaim::SkeletonConstPtr model = wrp2->get();
	kinematic::SkeletonPtr skeleton = utils::make_shared<kinematic::Skeleton>();
	if (!kinematic::Skeleton::convert(*model, *skeleton)) {
		throw std::runtime_error("Unable to convert skeleton");
	}
	kinematic::SkeletonStatesPtr states = utils::make_shared<kinematic::SkeletonStates>();
	states->frameTime = data->frameTime;
	auto firstFrame = data->frames[0];
	for (auto& bone : firstFrame.bonesData) {
		states->jointNames.push_back(bone.name);
	}
	for (auto& frame : data->frames) {
		states->frames.push_back(kinematic::SkeletonState::convert(*model, frame));
	}

	auto sws = utils::make_shared<kinematic::SkeletonWithStates>();
	sws->skeleton = skeleton;
	sws->states = states;

	auto object = core::Variant::create<kinematic::SkeletonWithStates>();
	object->set(sws);
	
	auto hierarchyTransaction = memoryDM->hierarchyTransaction();
	core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>(p1.filename().string().c_str(), p1.string().c_str(), QIcon());
	core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(object, QString());
	root->appendChild(item);
	hierarchyTransaction->addRoot(root);
}
