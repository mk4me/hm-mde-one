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
#include "PointsLayer.h"
#include <corelib/ISourceManager.h>

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

std::string getUserName()
{
    communication::ICommunicationDataSourcePtr comm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());
    const communication::IUser* usr = comm->currentUser();
    return usr->name();
}

core::IHierarchyItemPtr dicom::DicomPerspective::getPerspective( PluginSubject::SubjectPtr subject )
{
    std::string name = getUserName();

	auto comm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());
    
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
            if (sessionWrp->getMetadata("core/source", sessionName)) {
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
            motionOW->getMetadata("core/source", trialName);
            
            if (motion->hasObject(typeid(dicom::ILayeredImage), false)) {
                core::ConstObjectsList images;
                core::ConstObjectsList layers;
                motion->getObjects(images, typeid(dicom::ILayeredImage), false);	
                motion->getObjects(layers, typeid(dicom::LayersVector), false);
                
                // w bazie jest po jednym pliku na motion
                if (images.size() == 1) {
                    utils::ObjectWrapperConstPtr wrapper = (*images.begin());
                    std::string sourceFile;
                    if (wrapper->getMetadata("core/source", sourceFile)) {
                        fs::Path stem = fs::Path(sourceFile).stem();
						std::string sstem = stem.string();
                        fs::Path xmlFilename = tmpDir / (sstem + "." + name + ".xml");
                        
                        LayeredImageConstPtr img = wrapper->get();
						LayeredImagePtr ncimg = utils::const_pointer_cast<LayeredImage>(img);
                        
                        std::string imageFilename = stem.string();
                        if (sessionWrp) {
                            dicom::DicomInternalStructConstPtr internalStruct = sessionWrp->get();
                            auto internalImage = internalStruct->tryGetImage(imageFilename + ".png");
                            if (internalImage) {
                                desc = DicomSource::createDesc(*internalImage);
								ncimg->setIsPowerDoppler(internalImage->isPowerDoppler);
                            }
                        }

						ncimg->setTrialID(comm->trialID(stem.string()));
                        
                        QIcon icon;
                        if (!layers.empty() || fs::pathExists(xmlFilename)) {
                            icon = QIcon(":/dicom/file_done.png");
                        } else {
                            icon = QIcon(":/dicom/file.png");
                        }
                        
                        DicomHelperPtr helper = DicomHelperPtr(new DicomHelper(wrapper, layers, xmlFilename.string(), trialName));
                        core::IHierarchyItemPtr imgItem(new core::HierarchyDataItem(icon, QString::fromStdString(imageFilename), desc, helper));
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

void dicom::DicomHelper::createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series )
{
    UTILS_ASSERT(wrapper, "Item should be initialized");
    std::string name = getUserName();
    core::ObjectWrapperPtr wrp = wrapper->clone();
    wrp->setMetadata("DICOM_XML", xmlFilename);
    wrp->setMetadata("TRIAL_NAME", trialName);
                        
    LayeredImagePtr img = wrp->get();
    //LayeredImagePtr img = utils::const_pointer_cast<LayeredImage>(cimg);
    bool localAdded = false;

    for (auto itXml = layers.begin(); itXml != layers.end(); ++itXml) {
        LayersVectorConstPtr layersVector = (*itXml)->get();
        std::string xmlUser = "unknown";
        if ((*itXml)->getMetadata("core/source", xmlUser)) {
            // TODO zrobic to regexem
            xmlUser = fs::Path(xmlUser).stem().string();
            xmlUser = xmlUser.substr(xmlUser.find_last_of(".") + 1);
        }
        if (name == xmlUser) {
            layersVector = resolveLocalXml(xmlFilename, layersVector);
            localAdded = true;
        }
        for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
            img->addLayer(*layerIt, xmlUser);
        }
    }

    // nie bylo konfliktu danych lokalnych vs sciagnietych, ale mozliwe, ze sa tylko lokalne
    if (!localAdded) {

		bool bFound = false;
		bool iFound = false;

        auto layersVector = resolveLocalXml(xmlFilename);
        if (layersVector) {
            for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
                
				switch((*layerIt)->getAdnotationIdx()){
				case dicom::adnotations::bloodLevel:
					bFound = true;
					break;


				case dicom::adnotations::inflammatoryLevel:
					iFound = true;
					break;
				}
            }
        }

		if(iFound == false){
			auto l = dicom::ILayerItemPtr(new InflammatoryLevelLayer(dicom::adnotations::inflammatoryLevel, dicom::adnotations::unknownInflammatoryLevel));
			l->setName(QObject::tr("Inflammatory level"));			
			img->addLayer(l, name);
		}

		if(bFound == false && img->isPowerDoppler() == true){
			auto l = dicom::ILayerItemPtr(new BloodLevelLayer(dicom::adnotations::bloodLevel, dicom::adnotations::unknownBloodLevel));
			l->setName(QObject::tr("Blood level"));			
			img->addLayer(l, name);
		}

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
    auto serie = visualizer->createSerie(wrp->getTypeInfo(), wrp);
    serie->serie()->setName(path.toStdString());
    series.push_back(serie);    
}

dicom::DicomHelper::DicomHelper( const core::ObjectWrapperConstPtr & wrapper, const core::ConstObjectsList& layers,
    const std::string& xmlFilename, const std::string& trialName) :
    WrappedItemHelper(wrapper),
    layers(layers),
    xmlFilename(xmlFilename),
    trialName(trialName)
{

}
