#include "DicomPCH.h"
#include "DicomPerspective.h"
#include <corelib/HierarchyItem.h>

#include <corelib/IDataManagerReader.h>
#include <corelib/DataAccessors.h>
#include <plugins/subject/IMotion.h>
#include <corelib/HierarchyDataItem.h>
#include <plugins/newCommunication/TreeItemHelper.h>
#include <boost/foreach.hpp>
#include <plugins/subject/ISubject.h>
#include <plugins/newCommunication/IPatient.h>
#include <corelib/PluginCommon.h>
#include <utils/ObjectWrapper.h>
#include "plugins/dicom/ILayeredImage.h"
#include "DicomInternalStruct.h"
#include "LayeredImage.h"
#include <plugins/newCommunication/ICommunicationDataSource.h>
#include <plugins/newCommunication/IDataSourceUser.h>
#include <corelib/ISourceManager.h>
#include <corelib/PluginCommon.h>

typedef core::Filesystem fs;


core::IHierarchyItemPtr dicom::DicomPerspective::getPerspective( PluginSubject::SubjectPtr subject )
{
    communication::ICommunicationDataSourcePtr comm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());
    const communication::IUser* usr = comm->currentUser();
    std::string name = usr->name();
    
    fs::Path tmpDir = plugin::getUserDataPath() / std::string( "MEDUSA/tmp");
    if (!fs::pathExists(tmpDir)) {
        fs::createDirectory(tmpDir);
    }


    bool hasData = false;
    auto rootItem = utils::make_shared<core::HierarchyItem>(QString(), QString(), QIcon());
    core::ConstObjectsList sessions;
    subject->getSessions(sessions);
    for (auto it = sessions.crbegin(); it != sessions.crend(); ++it) {
        core::ConstObjectsList motions;
        PluginSubject::SessionConstPtr s = (*it)->get();
        QString label(QString::fromUtf8(s->getLocalName().c_str()));
        QString desc("desc");
        if (s->hasObject(typeid(dicom::DicomInternalStruct),false)) {
            core::ConstObjectsList inter;
            s->getObjects(inter, typeid(dicom::DicomInternalStruct), false);
            utils::ObjectWrapperConstPtr sessionWrp = *inter.begin();
            std::string sessionName;
            if (sessionWrp->tryGetMeta("core/source", sessionName)) {
                label = QString(sessionName.c_str());
            }
            
            //DicomInternalStructConstPtr test = (*inter.begin())->get();
        }
        core::IHierarchyItemPtr sessionItem(new core::HierarchyItem(label, desc));
        rootItem->appendChild(sessionItem);
        s->getMotions(motions);

        BOOST_FOREACH(utils::ObjectWrapperConstPtr motionOW, motions) {	

            PluginSubject::MotionConstPtr motion = motionOW->get();
            if (motion->hasObject(typeid(dicom::ILayeredImage), false)) {
                core::ConstObjectsList images;
                core::ConstObjectsList layers;
                motion->getObjects(images, typeid(dicom::ILayeredImage), false);	
                motion->getObjects(layers, typeid(dicom::LayersVector), false);
                
                // w bazie jest po jednym pliku na motion
                if (images.size() == 1) {
                    utils::ObjectWrapperPtr wrapper = utils::const_pointer_cast<utils::ObjectWrapper>(*images.begin());
                    std::string sourceFile;
                    if (wrapper->tryGetMeta("core/source", sourceFile)) {
                        fs::Path stem = fs::Path(sourceFile).stem();
                        fs::Path p = tmpDir / (stem.string() + "." + name + ".xml");
                        (*wrapper)[std::string("DICOM_XML")] = std::string(p.string());

                        // TODO : wsparcie dla wielu plikow xml + tymczasowego save'a
                        if (layers.size() == 1) {
                            LayeredImageConstPtr cimg = wrapper->get();
                            LayeredImagePtr img = utils::const_pointer_cast<LayeredImage>(cimg);
                            LayersVectorConstPtr layersVector = (*layers.begin())->get();
                            for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
                                img->addLayer(*layerIt);
                            }
                        }

                        core::IHierarchyItemPtr imgItem(new core::HierarchyDataItem(wrapper, QIcon(), QString::fromStdString(stem.string()), desc));
                        sessionItem->appendChild(imgItem);
                        hasData = true;
                    } else {
                        UTILS_ASSERT(false);
                    }
                } else {
                    // powinien byc zawsze jeden obraz na motion
                    UTILS_ASSERT(false);
                    /*for(auto it = images.begin(); it != images.end(); ++it) {			
                        utils::ObjectWrapperPtr wrapper = utils::const_pointer_cast<utils::ObjectWrapper>(*it);
                        std::string sourceFile;
                        if (wrapper->tryGetMeta("core/source", sourceFile)) {
                            fs::Path stem = fs::Path(sourceFile).stem();
                            fs::Path p = tmpDir / (stem.string() + "." + name + ".xml");
                            (*wrapper)[std::string("DICOM_XML")] = std::string(p.string());
                            core::IHierarchyItemPtr imgItem(new core::HierarchyDataItem(*it, QIcon(), QString::fromStdString(stem.string()), desc));
                            sessionItem->appendChild(imgItem);
                            hasData = true;
                        } else {
                            UTILS_ASSERT(false);
                        }

                    }*/
                }
            }
        }
    }

    if (hasData) {
        return rootItem;
    }

    return core::IHierarchyItemPtr();
}

