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
#include "DicomSource.h"

typedef core::Filesystem fs;

dicom::LayersVectorConstPtr resolveLocalXml( const fs::Path& xmlPath, dicom::LayersVectorConstPtr layersVector = dicom::LayersVectorConstPtr() )
{
    if (fs::pathExists(xmlPath)) {
        dicom::LayersVectorPtr layers = dicom::DicomLoader::loadLayers(xmlPath);
        if (layers->size()) {
            return layers;
        }
    }
    return layersVector;
}

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
        QString desc("");
        utils::ObjectWrapperConstPtr sessionWrp;
        if (s->hasObject(typeid(dicom::DicomInternalStruct),false)) {
            core::ConstObjectsList inter;
            s->getObjects(inter, typeid(dicom::DicomInternalStruct), false);
            sessionWrp = *inter.begin();
            std::string sessionName;
            if (sessionWrp->tryGetMeta("core/source", sessionName)) {
                fs::Path sp(sessionName);
                label = QString(sp.stem().string().c_str());
            }
            dicom::DicomInternalStructConstPtr internalStruct = sessionWrp->get();
            auto session = internalStruct->tryGetSession(label.toStdString());
            if (session) {
                desc = DicomSource::createDesc(*session);
            }
            //DicomInternalStructConstPtr test = (*inter.begin())->get();
        }
        core::IHierarchyItemPtr sessionItem(new core::HierarchyItem(label, desc));
        rootItem->appendChild(sessionItem);
        s->getMotions(motions);

        BOOST_FOREACH(utils::ObjectWrapperConstPtr motionOW, motions) {	

            PluginSubject::MotionConstPtr motion = motionOW->get();
            std::string trialName;
            motionOW->tryGetMeta("core/source", trialName);
            
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
                        (*wrapper)[std::string("TRIAL_NAME")] = trialName;
                        
                        LayeredImageConstPtr cimg = wrapper->get();
                        LayeredImagePtr img = utils::const_pointer_cast<LayeredImage>(cimg);
                        bool localAdded = false;
                        for (auto itXml = layers.begin(); itXml != layers.end(); ++itXml) {
                            LayersVectorConstPtr layersVector = (*itXml)->get();
                            std::string xmlUser = "unknown";
                            if ((*itXml)->tryGetMeta("core/source", xmlUser)) {
                                // TODO zrobic to regexem
                                xmlUser = fs::Path(xmlUser).stem().string();
                                xmlUser = xmlUser.substr(xmlUser.find_last_of(".") + 1);
                            }
                            if (name == xmlUser) {
                                layersVector = resolveLocalXml(p, layersVector);
                                localAdded = true;
                            }
                            for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
                                img->addLayer(*layerIt, xmlUser);
                            }
                        }

                        // nie bylo konfliktu danych lokalnych vs sciagnietych, ale mozliwe, ze sa tylko lokalne
                        if (!localAdded) {
                            auto layersVector = resolveLocalXml(p);
                            if (layersVector) {
                                for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
                                    img->addLayer(*layerIt, name);
                                }
                            }
                        }

                        for (int row = img->getNumTags() - 1; row >= 0; --row) {
                            std::string tag = img->getTag(row);
                            img->setTagVisible(tag, tag == name);
                        }
                        std::string imageFilename = stem.string();
                        if (sessionWrp) {
                            dicom::DicomInternalStructConstPtr internalStruct = sessionWrp->get();
                            auto internalImage = internalStruct->tryGetImage(imageFilename + ".png");
                            if (internalImage) {
                                desc = DicomSource::createDesc(*internalImage);
                            }
                        }
                        
                        QIcon icon;
                        if (img->getNumTags()) {
                            icon = QIcon(":/dicom/file_done.png");
                        } else {
                            icon = QIcon(":/dicom/file.png");
                        }

                        core::IHierarchyItemPtr imgItem(new core::HierarchyDataItem(wrapper, icon, QString::fromStdString(imageFilename), desc));
                        sessionItem->appendChild(imgItem);
                        hasData = true;
                    } else {
                        UTILS_ASSERT(false);
                    }
                } else {
                    // powinien byc zawsze jeden obraz na motion
                    UTILS_ASSERT(false);
                }
            }
        }
    }

    if (hasData) {
        return rootItem;
    }

    return core::IHierarchyItemPtr();
}



