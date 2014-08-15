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
#include <QtCore/QObject>
#include "IMUCFGParser.h"
#include "IMUDatParser.h"

typedef core::Filesystem fs;

VectorChannelPtr createChannel(int hz, const IMU::IMUConfig& config, int i, const std::string& unit = std::string())
{
    auto name = [&](const IMU::IMUConfig& conf, int idx) -> std::string {
		auto size = conf.joints.size();
        for (int i = 0; i < size; ++i) {
            if (conf.joints[i].idx == idx) {
                return conf.joints[i].name;
            }
        }
		QString unknown = QObject::tr("Sensor %1").arg(idx);
        return unknown.toStdString();
    };
	
    auto c = utils::make_shared<VectorChannel>(hz);
    // + 1 , bo numeracja w plikach *.cfg zaczyna si� od 1
    c->setName(name(config, i + 1));
	c->setValueBaseUnit(unit);

    return c;
}



core::IHierarchyItemPtr IMU::IMUPerspective::getPerspective( PluginSubject::SubjectPtr subject )
{
	auto comm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());

    bool hasData = false;
    auto rootItem = utils::make_shared<core::HierarchyItem>(QString(), QString(), QIcon());
    core::ConstVariantsList sessions;
    subject->getSessions(sessions);
	
    for (auto& sessionWrp :sessions) {
		
        core::ConstVariantsList motions;
		PluginSubject::SessionConstPtr s = sessionWrp->get();
		core::ConstVariantsList configs;
		IMU::IMUConfigConstPtr config = utils::make_shared<IMUConfig>();
		if (s->hasObject(typeid(IMU::IMUConfig), false)) {
			s->getObjects(configs, typeid(IMU::IMUConfig), false);
			UTILS_ASSERT(configs.size() == 1);
			(*configs.begin())->tryGet(config);
		}


		
        QString label(QString::fromUtf8(s->getLocalName().c_str()));
        QString desc("");
        
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

			if (motion->hasObject(typeid(IMU::Frames), false)) {
				core::ConstVariantsList framesV;
				hasData = true;
				motion->getObjects(framesV, typeid(IMU::Frames), false);

				auto accWrapper = utils::ObjectWrapper::create<VectorChannelCollection>();
				auto magWrapper = utils::ObjectWrapper::create<VectorChannelCollection>();
				auto gyrWrapper = utils::ObjectWrapper::create<VectorChannelCollection>();
				
				VectorChannelCollectionPtr accelerations = utils::make_shared<VectorChannelCollection>();
				VectorChannelCollectionPtr magnetometers = utils::make_shared<VectorChannelCollection>();
				VectorChannelCollectionPtr gyroscopes = utils::make_shared<VectorChannelCollection>();
				
				// todo przeniesc te hz do configa?
				int hz = 50;
				/*IMU::Frames frames;
				if (config->imusCount != -1) {
					frames = IMU::IMUDatParser::parse(source, config->imusCount);
				}
				else {
					auto res = IMU::IMUDatParser::parse(source);
					frames = res.first;
					config->imusCount = res.second;
				}*/
				
				IMU::FramesConstPtr frms = (*framesV.begin())->get();
				if (config->imusCount == -1 && !frms->empty()) {
					auto c = utils::const_pointer_cast<IMU::IMUConfig>(config);
					c->imusCount = frms->at(0).size();
				}
				for (int i = 0; i < config->imusCount; ++i) {
				    accelerations->addChannel(createChannel(hz, *config, i, "m/s^2"));
				    magnetometers->addChannel(createChannel(hz, *config, i));
				    gyroscopes->addChannel(createChannel(hz, *config, i, "rad/s"));
				}
				    
				for (auto itFrame = frms->begin(); itFrame != frms->end(); ++itFrame) {
				    const IMU::Frame& f = *itFrame;
				    for (int i = 0; i < config->imusCount; ++i) {
						// TODO te parametry (1024, 2048) trzeba gdzies przeniesc (do ramki?)
				        accelerations->getChannel(i)->addPoint(osg::Vec3(f[i].raw.acc_x, f[i].raw.acc_y, f[i].raw.acc_z) * (1 / 1024.0f));
						magnetometers->getChannel(i)->addPoint(osg::Vec3(f[i].raw.mag_x, f[i].raw.mag_y, f[i].raw.mag_z) * (1 / 2048.0f));
						gyroscopes->getChannel(i)->addPoint(osg::Vec3(f[i].raw.rate_x, f[i].raw.rate_y, f[i].raw.rate_z) * (1 / 2048.0f));
				    }
				}
				accWrapper->set(accelerations);
				magWrapper->set(magnetometers);
				gyrWrapper->set(gyroscopes);

				{
					core::VariantsList objects;

					auto _lambda = [&](VectorChannelCollectionPtr collection, const std::string& groupName)
					{
						int count = collection->getNumChannels();
						for (int i = 0; i < count; ++i) {
							VectorChannelPtr channel = collection->getChannel(i);
							
							core::VariantPtr wrapper = core::Variant::create<VectorChannel>();
							wrapper->setMetadata("core/name", channel->getName());
							wrapper->setMetadata("core/source", s->getName() + std::string("/") + groupName);
							wrapper->set(channel);
							objects.push_back(wrapper);
						}
					};

					_lambda(accelerations, "accelerations");
					_lambda(magnetometers, "magnetometers");
					_lambda(gyroscopes, "gyroscopes");

					auto memTransaction = memoryDataManager->transaction();
					for (auto it = objects.begin(); it != objects.end(); ++it){
						memTransaction->addData(*it);
					}
				}
				    
				

				auto accItem = createImuCollectionItem(0, accelerations);
				sessionItem->appendChild(accItem);

				auto magItem = createImuCollectionItem(1, magnetometers);
				sessionItem->appendChild(magItem);

				auto gyrItem = createImuCollectionItem(2, gyroscopes);
				sessionItem->appendChild(gyrItem);
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
			auto collectionItem = createImuCollectionItem(i, collection);
			root->appendChild(collectionItem);
		}
	}
}

core::IHierarchyItemPtr IMU::IMUPerspective::createImuCollectionItem(int i, VectorChannelCollectionConstPtr collection)
{
	QString name;
	switch (i) {
		case 0: name = QObject::tr("Accelerations"); break;
		case 1: name = QObject::tr("Magnetometers"); break;
		case 2: name = QObject::tr("Gyroscopes"); break;
		default: name = QObject::tr("Item"); break;
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

	return collectionItem;
}

IMU::IMUPerspective::IMUPerspective(core::IMemoryDataManager * memoryDataManager) : 
	memoryDataManager(memoryDataManager)
{

}

void IMU::IMUPerspectiveService::init(core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager, core::IMemoryDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager)
{
	this->sourceManager = sourceManager;
	this->memoryDataManager = memoryDataManager;
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
		comm->addHierarchyPerspective(utils::make_shared<IMUPerspective>(memoryDataManager));
		return true;
	}

	return false;
}