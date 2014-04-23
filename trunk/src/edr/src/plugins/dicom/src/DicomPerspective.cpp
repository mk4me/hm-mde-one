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
#include <boost/format.hpp>
#include "LayeredSerie.h"
#include "LayeredImageVisualizerView.h"

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
						std::string xmlFilenamePattern = (tmpDir / sstem).string() + ".%1%.xml";
                        fs::Path xmlFilename(boost::str(boost::format(xmlFilenamePattern) % name));
                        
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
                        auto hasAnnotation = [&](const std::string& filename, const core::ConstObjectsList& layers) -> bool {
                            for (auto it = layers.begin(); it != layers.end(); ++it) {
                                std::string source;
                                (*it)->getMetadata("core/source", source);
                                if (source.find(filename) != std::string::npos) {
                                    return true;
                                }
                            }
                            return false;
                        };
                        if (fs::pathExists(xmlFilename) || hasAnnotation(name, layers)) {
                            icon = QIcon(":/dicom/file_done.png");
                        } else if (!layers.empty()) {
                            icon = QIcon(":/dicom/file_blue.png");
                        } else {
                            icon = QIcon(":/dicom/file.png");
                        }
                        
                        DicomHelperPtr helper = DicomHelperPtr(new DicomHelper(wrapper, layers, xmlFilenamePattern, trialName));
                        core::HierarchyDataItemPtr imgItem(new core::HierarchyDataItem(icon, QString::fromStdString(imageFilename), desc, helper));
                        std::string mapName = imageFilename + ".png";

                        // TODO : to rozwiazanie moze byc problematyczne w przyszlosci, bo w calosci zalezy od implementacji update'a drzewa danych
                        auto itName = name2hierarchy.find(mapName);
                        if (itName == name2hierarchy.end()) {
                            name2hierarchy[imageFilename + ".png"] = imgItem;
                        }

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

core::HierarchyDataItemPtr dicom::DicomPerspective::tryGetHierarchyItem( const std::string& filename )
{
    auto it = name2hierarchy.find(filename);
    if (it != name2hierarchy.end()) {
        return (it->second).lock();
    }

    return core::HierarchyDataItemPtr();
}


void dicom::DicomHelper::createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series )
{
    UTILS_ASSERT(wrapper, "Item should be initialized");
    core::ObjectWrapperPtr wrp = utils::const_pointer_cast<core::ObjectWrapper>(wrapper);
    std::string name = getUserName();
	fs::Path realXmlFilename(boost::str(boost::format(xmlFilename) % name));
    wrp->setMetadata("DICOM_XML", realXmlFilename.string());
	wrp->setMetadata("DICOM_XML_PATTERN", xmlFilename);
    wrp->setMetadata("TRIAL_NAME", trialName);
    //core::ObjectWrapperPtr wrp = wrapper->clone();
    auto serie = visualizer->createSerie(wrp->getTypeInfo(), wrp);
    auto layeredSerie = dynamic_cast<LayeredSerie*>(serie->serie());
    UTILS_ASSERT(layeredSerie);
    layeredSerie->setName(path.toStdString());
                        
    ILayeredImagePtr img = layeredSerie->getImage();
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
            layersVector = resolveLocalXml(realXmlFilename, layersVector);
            localAdded = true;
        }

		bool bFound = false;
		bool iFound = false;
		bool fFound = false;
		bool jFound = false;
		bool imgFound = false;

		for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {

			switch((*layerIt)->getAdnotationIdx()){
			case dicom::adnotations::bloodLevel:
				bFound = true;
				break;


			case dicom::adnotations::inflammatoryLevel:
				iFound = true;
				break;

			case dicom::adnotations::jointType:
				jFound = true;
				break;

			case dicom::adnotations::fingerType:
				fFound = true;
				break;

			case dicom::adnotations::imageType:
				imgFound = true;
				break;
			}
		}		

		if(iFound == false){
			auto l = dicom::ILayerItemPtr(new InflammatoryLevelLayer(dicom::adnotations::inflammatoryLevel, dicom::adnotations::unknownInflammatoryLevel));
			l->setName(QObject::tr("Inflammatory level"));			
			img->addLayer(l, xmlUser);
		}

		if(bFound == false && img->isPowerDoppler() == true){
			auto l = dicom::ILayerItemPtr(new BloodLevelLayer(dicom::adnotations::bloodLevel, dicom::adnotations::unknownBloodLevel));
			l->setName(QObject::tr("Blood level"));			
			img->addLayer(l, xmlUser);
		}

		if(fFound == false){
			auto l = dicom::ILayerItemPtr(new FingerTypeLayer(dicom::adnotations::fingerType, dicom::adnotations::unknownFinger));
			l->setName(QObject::tr("Finger type"));			
			img->addLayer(l, xmlUser);
		}

		if(jFound == false){
			auto l = dicom::ILayerItemPtr(new JointTypeLayer(dicom::adnotations::jointType, dicom::adnotations::unknownJoint));
			l->setName(QObject::tr("Joint type"));			
			img->addLayer(l, xmlUser);
		}

		if(imgFound == false){
			auto l = dicom::ILayerItemPtr(new ImageQualityLayer(dicom::adnotations::imageType, dicom::adnotations::different));
			l->setName(QObject::tr("Image"));			
			img->addLayer(l, xmlUser);
		}

        for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
            img->addLayer(*layerIt, xmlUser);
        }
    }

    // nie bylo konfliktu danych lokalnych vs sciagnietych, ale mozliwe, ze sa tylko lokalne
    if (!localAdded) {

		bool bFound = false;
		bool iFound = false;
		bool fFound = false;
		bool jFound = false;
		bool imgFound = false;

        auto layersVector = resolveLocalXml(realXmlFilename);
        if (layersVector) {
            for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
                
				switch((*layerIt)->getAdnotationIdx()){
				case dicom::adnotations::bloodLevel:
					bFound = true;
					break;


				case dicom::adnotations::inflammatoryLevel:
					iFound = true;
					break;

				case dicom::adnotations::jointType:
					jFound = true;
					break;

				case dicom::adnotations::fingerType:
					fFound = true;
					break;

				case dicom::adnotations::imageType:
					imgFound = true;
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

		if(fFound == false){
			auto l = dicom::ILayerItemPtr(new FingerTypeLayer(dicom::adnotations::fingerType, dicom::adnotations::unknownFinger));
			l->setName(QObject::tr("Finger type"));			
			img->addLayer(l, name);
		}

		if(jFound == false){
			auto l = dicom::ILayerItemPtr(new JointTypeLayer(dicom::adnotations::jointType, dicom::adnotations::unknownJoint));
			l->setName(QObject::tr("Joint type"));			
			img->addLayer(l, name);
		}

		if(imgFound == false){
			auto l = dicom::ILayerItemPtr(new ImageQualityLayer(dicom::adnotations::imageType, dicom::adnotations::different));
			l->setName(QObject::tr("Image"));			
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
    layeredSerie->refresh();
    LayeredImageVisualizerView* view = dynamic_cast<LayeredImageVisualizerView*>(visualizer->getOrCreateWidget());
    UTILS_ASSERT(view);
    view->refresh();
    visualizer->setActiveSerie(serie);
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
