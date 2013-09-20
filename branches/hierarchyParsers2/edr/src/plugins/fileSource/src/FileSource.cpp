#include "FileSource.h"
#include <corelib/Filesystem.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include <plugins/c3d/C3DCollections.h>
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
    files.insert(path);
    auto transaction = fileDM->transaction();
    transaction->addFile(path);
    core::ConstObjectsList oList;
    core::IHierarchyItemConstPtr root;// = utils::make_shared<core::HierarchyItem>(path.filename().string().c_str(), path.string().c_str(), QIcon());
    auto objs = transaction->getObjects(path);
    
    auto hierarchyTransaction = memoryDM->hierarchyTransaction();
    for (auto it = objs.begin(); it != objs.end(); ++it) {
        auto addition = (*it)->additionalObjects;
        for (auto ad = addition.begin(); ad != addition.end(); ++ad) {
            (*ad)->initialize();

        }
        if ((*it)->hierarchy) {
            hierarchyTransaction->addRoot((*it)->hierarchy);
            roots.insert((*it)->hierarchy);
        }
    }

    
}

void FileSource::removeFiles()
{
    for (auto it = files.begin(); it != files.end(); ++it) {
        auto transaction = fileDM->transaction();
        transaction->removeFile(*it);
    }
    files.clear();

    for (auto it = roots.begin(); it != roots.end(); ++it) {
        auto transaction = memoryDM->hierarchyTransaction();
        transaction->removeRoot(*it);
    }
    roots.clear();
}
