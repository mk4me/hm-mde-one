#include "PCH.h"
#include "IMUPerspective.h"
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
#include <plugins/newCommunication/ICommunicationDataSource.h>
#include <plugins/newCommunication/IDataSourceUser.h>
#include <corelib/ISourceManager.h>
#include <corelib/PluginCommon.h>
#include <corelib/ISourceManager.h>
#include <boost/format.hpp>

typedef core::Filesystem fs;


core::IHierarchyItemPtr IMU::IMUPerspective::getPerspective( PluginSubject::SubjectPtr subject )
{
	auto comm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());

    bool hasData = false;
    auto rootItem = utils::make_shared<core::HierarchyItem>(QString(), QString(), QIcon());
    core::ConstVariantsList sessions;
    subject->getSessions(sessions);
    for (auto it = sessions.crbegin(); it != sessions.crend(); ++it) {
        core::ConstVariantsList motions;
        PluginSubject::SessionConstPtr s = (*it)->get();
        QString label(QString::fromUtf8(s->getLocalName().c_str()));
        QString desc("");
        core::VariantConstPtr sessionWrp;
        
        core::HierarchyItemPtr sessionItem(new core::HierarchyItem(label, desc));
        rootItem->appendChild(sessionItem);
        s->getMotions(motions);

        BOOST_FOREACH(core::VariantConstPtr motionOW, motions) {	

            PluginSubject::MotionConstPtr motion = motionOW->get();
            std::string trialName;
            motionOW->getMetadata("core/source", trialName);
            
            if (motion->hasObject(typeid(VectorChannelCollection), false)) {
                core::ConstVariantsList vectors;
				hasData = true;
				motion->getObjects(vectors, typeid(VectorChannelCollection), false);
				createIMUBranch(vectors, sessionItem);
            }
        }
    }

    if (hasData) {
        return rootItem;
    }

    return core::IHierarchyItemPtr();
}

void IMU::IMUPerspective::createIMUBranch(core::ConstVariantsList &oList, core::HierarchyItemPtr root)
{
	int i = 0;
	for (auto it = oList.begin(); it != oList.end(); ++i, ++it) {
		VectorChannelCollectionConstPtr collection = (*it)->get();
		if (collection) {
			QString name;
			switch (i) {
			case 0: name = "Accelerations"; break;
			case 1: name = "Magnetometers"; break;
			case 2: name = "Gyroscopes"; break;
			default: name = "Item"; break;
			}
			auto collectionItem = utils::make_shared<core::HierarchyItem>(name, QString());
			for (int j = 0; j < collection->getNumChannels(); ++j) {
				core::VariantPtr wrapper = core::Variant::create<VectorChannel>();
				VectorChannelConstPtr c = collection->getChannel(j);
				wrapper->set(utils::const_pointer_cast<VectorChannel>(c));
				static int number = 0;
				wrapper->setMetadata("name", "Serie_" + boost::lexical_cast<std::string>(number++));
				wrapper->setMetadata("source", "TreeBuilder");
				NewVector3ItemHelperPtr channelHelper = utils::make_shared<NewVector3ItemHelper>(wrapper);
				core::IHierarchyItemPtr channelItem = utils::make_shared<core::HierarchyDataItem>(wrapper, QIcon(), QString::fromStdString(c->getName()), QString(), channelHelper);
				collectionItem->appendChild(channelItem);
			}
			root->appendChild(collectionItem);
		}
	}
}

//core::HierarchyDataItemPtr dicom::DicomPerspective::tryGetHierarchyItem( const std::string& filename )
//{
//    auto it = name2hierarchy.find(filename);
//    if (it != name2hierarchy.end()) {
//        return (it->second).lock();
//    }
//
//    return core::HierarchyDataItemPtr();
//}


//void dicom::DicomHelper::createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series )
//{
//    UTILS_ASSERT(wrapper, "Item should be initialized");
//    core::VariantPtr wrp = utils::const_pointer_cast<core::Variant>(wrapper);
//    std::string name = getUserName();
//	fs::Path realXmlFilename(boost::str(boost::format(xmlFilename) % name));
//    wrp->setMetadata("DICOM_XML", realXmlFilename.string());
//	wrp->setMetadata("DICOM_XML_PATTERN", xmlFilename);
//    wrp->setMetadata("TRIAL_NAME", trialName);
//    //core::VariantPtr wrp = wrapper->clone();
//    auto serie = visualizer->createSerie(wrp->data()->getTypeInfo(), wrp);
//    auto layeredSerie = dynamic_cast<LayeredSerie*>(serie->serie());
//    UTILS_ASSERT(layeredSerie);
//    layeredSerie->setName(path.toStdString());
//                        
//    ILayeredImagePtr img = layeredSerie->getImage();
//    //LayeredImagePtr img = utils::const_pointer_cast<LayeredImage>(cimg);
//    bool localAdded = false;
//
//    for (auto itXml = layers.begin(); itXml != layers.end(); ++itXml) {
//        LayersVectorConstPtr layersVector = (*itXml)->get();
//        std::string xmlUser = "unknown";
//        if ((*itXml)->getMetadata("core/source", xmlUser)) {
//            // TODO zrobic to regexem
//            xmlUser = fs::Path(xmlUser).stem().string();
//            xmlUser = xmlUser.substr(xmlUser.find_last_of(".") + 1);
//        }
//        if (name == xmlUser) {
//            layersVector = resolveLocalXml(realXmlFilename, layersVector);
//            localAdded = true;
//        }
//
//		bool bFound = false;
//		bool iFound = false;
//		bool fFound = false;
//		bool jFound = false;
//		bool imgFound = false;
//
//		for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
//
//			switch((*layerIt)->getAdnotationIdx()){
//			case dicom::annotations::bloodLevel:
//				bFound = true;
//				break;
//
//
//			case dicom::annotations::inflammatoryLevel:
//				iFound = true;
//				break;
//
//			case dicom::annotations::jointType:
//				jFound = true;
//				break;
//
//			case dicom::annotations::fingerType:
//				fFound = true;
//				break;
//
//			case dicom::annotations::imageType:
//				imgFound = true;
//				break;
//			}
//		}		
//
//		if(iFound == false){
//			auto l = dicom::ILayerItemPtr(new InflammatoryLevelLayer(dicom::annotations::inflammatoryLevel, dicom::annotations::unknownInflammatoryLevel));
//			l->setName(QObject::tr("Inflammatory level"));			
//			img->addLayer(l, xmlUser);
//		}
//
//		if(bFound == false && img->isPowerDoppler() == true){
//			auto l = dicom::ILayerItemPtr(new BloodLevelLayer(dicom::annotations::bloodLevel, dicom::annotations::unknownBloodLevel));
//			l->setName(QObject::tr("Blood level"));			
//			img->addLayer(l, xmlUser);
//		}
//
//		if(fFound == false){
//			auto l = dicom::ILayerItemPtr(new FingerTypeLayer(dicom::annotations::fingerType, dicom::annotations::unknownFinger));
//			l->setName(QObject::tr("Finger type"));			
//			img->addLayer(l, xmlUser);
//		}
//
//		if(jFound == false){
//			auto l = dicom::ILayerItemPtr(new JointTypeLayer(dicom::annotations::jointType, dicom::annotations::unknownJoint));
//			l->setName(QObject::tr("Joint type"));			
//			img->addLayer(l, xmlUser);
//		}
//
//		if(imgFound == false){
//			auto l = dicom::ILayerItemPtr(new ImageQualityLayer(dicom::annotations::imageType, dicom::annotations::different));
//			l->setName(QObject::tr("Image"));			
//			img->addLayer(l, xmlUser);
//		}
//
//        for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
//            img->addLayer(*layerIt, xmlUser);
//        }
//    }
//
//    // nie bylo konfliktu danych lokalnych vs sciagnietych, ale mozliwe, ze sa tylko lokalne
//    if (!localAdded) {
//
//		bool bFound = false;
//		bool iFound = false;
//		bool fFound = false;
//		bool jFound = false;
//		bool imgFound = false;
//
//        auto layersVector = resolveLocalXml(realXmlFilename);
//        if (layersVector) {
//            for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
//                
//				switch((*layerIt)->getAdnotationIdx()){
//				case dicom::annotations::bloodLevel:
//					bFound = true;
//					break;
//
//
//				case dicom::annotations::inflammatoryLevel:
//					iFound = true;
//					break;
//
//				case dicom::annotations::jointType:
//					jFound = true;
//					break;
//
//				case dicom::annotations::fingerType:
//					fFound = true;
//					break;
//
//				case dicom::annotations::imageType:
//					imgFound = true;
//					break;
//				}
//            }
//        }
//
//		if(iFound == false){
//			auto l = dicom::ILayerItemPtr(new InflammatoryLevelLayer(dicom::annotations::inflammatoryLevel, dicom::annotations::unknownInflammatoryLevel));
//			l->setName(QObject::tr("Inflammatory level"));			
//			img->addLayer(l, name);
//		}
//
//		if(bFound == false && img->isPowerDoppler() == true){
//			auto l = dicom::ILayerItemPtr(new BloodLevelLayer(dicom::annotations::bloodLevel, dicom::annotations::unknownBloodLevel));
//			l->setName(QObject::tr("Blood level"));			
//			img->addLayer(l, name);
//		}
//
//		if(fFound == false){
//			auto l = dicom::ILayerItemPtr(new FingerTypeLayer(dicom::annotations::fingerType, dicom::annotations::unknownFinger));
//			l->setName(QObject::tr("Finger type"));			
//			img->addLayer(l, name);
//		}
//
//		if(jFound == false){
//			auto l = dicom::ILayerItemPtr(new JointTypeLayer(dicom::annotations::jointType, dicom::annotations::unknownJoint));
//			l->setName(QObject::tr("Joint type"));			
//			img->addLayer(l, name);
//		}
//
//		if(imgFound == false){
//			auto l = dicom::ILayerItemPtr(new ImageQualityLayer(dicom::annotations::imageType, dicom::annotations::different));
//			l->setName(QObject::tr("Image"));			
//			img->addLayer(l, name);
//		}
//
//		if (layersVector) {
//			for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
//				img->addLayer(*layerIt, name);				
//			}
//		}
//    }
//
//    for (int row = img->getNumTags() - 1; row >= 0; --row) {
//        std::string tag = img->getTag(row);
//        img->setTagVisible(tag, tag == name);
//    }
//    layeredSerie->refresh();
//    LayeredImageVisualizerView* view = dynamic_cast<LayeredImageVisualizerView*>(visualizer->getOrCreateWidget());
//    UTILS_ASSERT(view);
//    view->refresh();
//    visualizer->setActiveSerie(serie);
//    series.push_back(serie);    
//}

//dicom::DicomHelper::DicomHelper( const core::VariantConstPtr & wrapper, const core::ConstVariantsList& layers,
//    const std::string& xmlFilename, const std::string& trialName) :
//    WrappedItemHelper(wrapper),
//    layers(layers),
//    xmlFilename(xmlFilename),
//    trialName(trialName)
//{
//
//}

void IMU::IMUPerspectiveService::init(core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager, core::IMemoryDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager)
{
	this->sourceManager = sourceManager;
}

void IMU::IMUPerspectiveService::finalize()
{
}

void IMU::IMUPerspectiveService::update(double deltaTime)
{
}

QWidget* IMU::IMUPerspectiveService::getWidget()
{
	return nullptr;
}

QWidgetList IMU::IMUPerspectiveService::getPropertiesWidgets()
{
	return QWidgetList();
}

const bool IMU::IMUPerspectiveService::lateInit()
{
	communication::ICommunicationDataSourcePtr comm = core::querySource<communication::ICommunicationDataSource>(sourceManager);
	if (comm){
		comm->addHierarchyPerspective(utils::make_shared<IMUPerspective>());
		return true;
	}

	return false;
}
