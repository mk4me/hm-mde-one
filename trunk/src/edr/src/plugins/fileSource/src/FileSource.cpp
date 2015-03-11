#include "FileSource.h"
#include <corelib/Filesystem.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include <corelib/PluginCommon.h>
#include "FileSourceWidget.h"
#include "../acclaimformatslib/acclaimformatslib/MotionData.h"
#include "../acclaimformatslib/acclaimformatslib/Skeleton.h"
#include "kinematiclib/Skeleton.h"
#include "kinematiclib/SkeletonState.h"
#include <plugins/kinematic/Wrappers.h>
#include <plugins/newChart/Wrappers.h>


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

void FileSource::fillStream() {
	static float t = 0.0f;
	for (auto& stream : streams) {
		auto p = std::make_pair(t,t);
		//stream->pushData(p);
	}

	t += 1.0f;
    
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
	//core::Filesystem::Path p1 = "C:/Users/Mateusz/Desktop/test.amc";
	//core::Filesystem::Path p2 = "C:/Users/Mateusz/Desktop/test.asf";

	core::Filesystem::Path p1 = "/home/wojtek/programming/WORK/MDE/branches/kinematicRefactorForIMU/src/edrutils/tests/kinematiclib/testFiles/test.amc";
	core::Filesystem::Path p2 = "/home/wojtek/programming/WORK/MDE/branches/kinematicRefactorForIMU/src/edrutils/tests/kinematiclib/testFiles/test.asf";
	//core::Filesystem::Path p1 = "C:/Users/Wojciech/Desktop/test.amc";
	//core::Filesystem::Path p2 = "C:/Users/Wojciech/Desktop/test.asf";
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
	SkeletonStatesPtr states = utils::make_shared<SkeletonStates>();
	states->frameTime = data->frameTime;

	const auto mapping = kinematic::SkeletonState::createMapping(*skeleton);
	
	for (auto& frame : data->frames) {
		auto sChange = kinematic::SkeletonState::convert(*model, frame, mapping);
		states->frames.push_back(sChange);
	}

	auto sws = utils::make_shared<SkeletonWithStates>();
	sws->skeleton = skeleton;
	sws->states = states;
	sws->nodesMapping = mapping;
	auto object = core::Variant::create<SkeletonWithStates>();
	object->set(sws);
	
	auto hierarchyTransaction = memoryDM->hierarchyTransaction();
	core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>(p1.filename().string().c_str(), p1.string().c_str(), QIcon());
	core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(object, QString());
	root->appendChild(item);
	hierarchyTransaction->addRoot(root);
}

void FileSource::addChartStream()
{
	typedef threadingUtils::StreamT<std::pair<float, float>> ScalarStreamImp;
	auto stream = utils::dynamic_pointer_cast<ScalarStream>(utils::make_shared<ScalarStreamImp>());
	streams.push_back(stream);
	auto object = core::Variant::create<ScalarStream>();
	object->set(stream);

	auto hierarchyTransaction = memoryDM->hierarchyTransaction();
	core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>("Chart Stream", "Chart Stream" , QIcon());
	core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(object, QString());
	root->appendChild(item);
	hierarchyTransaction->addRoot(root);
}

