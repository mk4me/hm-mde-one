#include "FileSource.h"
#include <corelib/Filesystem.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include "FileSourceWidget.h"


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

void FileSource::getOfferedTypes( core::TypeInfoList & offeredTypes ) const
{
    
}

void FileSource::addFile( const core::Filesystem::Path& path )
{
    auto transaction = fileDM->transaction();
    transaction->addFile(path);
    core::ConstObjectsList oList;
    core::IHierarchyItemConstPtr root;// = utils::make_shared<core::HierarchyItem>(path.filename().string().c_str(), path.string().c_str(), QIcon());
    transaction->getObjects(path, root, oList);

    auto hierarchyTransaction = memoryDM->hierarchyTransaction();
    /*for (auto it = oList.begin(); it != oList.end(); ++it) {
        core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(*it, QString());
        root->appendChild(item);
    }*/

    hierarchyTransaction->addRoot(root);
}
