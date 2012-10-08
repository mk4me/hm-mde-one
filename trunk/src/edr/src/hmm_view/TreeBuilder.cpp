#include "hmmPCH.h"
#include "TreeBuilder.h"
#include "HmmMainWindow.h"
#include "EMGFilter.h"
#include <core/IDataManager.h>
#include "Measurements.h"

using namespace PluginSubject;

QTreeWidgetItem* TreeBuilder::createTree(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions)
{
    DataFilterPtr nullFilter;
    return TreeBuilder::createTree(rootItemName, sessions, nullFilter);
}

QTreeWidgetItem* TreeBuilder::createTree(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions, const DataFilterPtr & dataFilter)
{
    QTreeWidgetItem* rootItem = new QTreeWidgetItem();
    rootItem->setText(0, rootItemName);
    for (int i = sessions.size() - 1; i >= 0; --i) {
        SessionConstPtr session = dataFilter ? dataFilter->filterData(sessions[i]) : sessions[i];

        std::vector<MotionConstPtr> motions;
        session->getMotions(motions);
        BOOST_FOREACH(MotionConstPtr motion, motions) {	
            QTreeWidgetItem* motionItem = new QTreeWidgetItem();  

			QString label(QString::fromUtf8(motion->getLocalName().c_str()));

			//próbuje pobrać metadane
			try{
				std::vector<core::ObjectWrapperConstPtr> metadata;

				//najpierw pobieram wszystkie motiony z DM, potem znajduję ten którego id równa się mojemu motionowi i dla niego pobieram metadane
				std::vector<PluginSubject::MotionConstPtr> dmMotions;

				core::queryDataPtr(DataManager::getInstance(), dmMotions, true);

				for(auto it = dmMotions.begin(); it != dmMotions.end(); ++it){
					if((*it).get() != motion.get() && (*it)->getID() == motion->getID()){
						core::IDataManagerReader::getMetadataForObject(DataManager::getInstance(), *it, metadata);
					}
				}

				core::IDataManagerReader::getMetadataForObject(DataManager::getInstance(), motion, metadata);
				auto metaITEnd = metadata.end();
				for(auto metaIT = metadata.begin(); metaIT != metaITEnd; ++metaIT){
					core::MetadataConstPtr meta = (*metaIT)->get(false);
					std::string l;

					if(meta != nullptr && meta->value("label", l) == true){
						label = QString::fromUtf8(l.c_str());
					}
				}
			}catch(...){

			}

            motionItem->setText(0, label);
            rootItem->addChild(motionItem);
            bool hasEmg = motion->hasObjectOfType(typeid(EMGChannel));
            bool hasGrf = motion->hasObjectOfType(typeid(GRFCollection));
            if (hasEmg || hasGrf) {
                QTreeWidgetItem* analogItem = new QTreeWidgetItem();
                analogItem->setText(0, QObject::tr("Analog data"));
                motionItem->addChild(analogItem);
                if (hasEmg) {	
                    analogItem->addChild(createEMGBranch(motion, QObject::tr("EMG"), getRootEMGIcon(), getEMGIcon()));
                }	         
                             
                if (hasGrf) {
                    analogItem->addChild(createGRFBranch(motion, QObject::tr("GRF"), getRootGRFIcon(), getGRFIcon()));
                }
            }

            if (motion->hasObjectOfType(typeid(ForceCollection)) || motion->hasObjectOfType(typeid(MomentCollection)) || motion->hasObjectOfType(typeid(PowerCollection))) {
                QTreeWidgetItem* kineticItem = new QTreeWidgetItem();
                kineticItem->setText(0, QObject::tr("Kinetic data"));
                motionItem->addChild(kineticItem);
                std::vector<core::ObjectWrapperConstPtr> forces;
                motion->getWrappers(forces, typeid(ForceCollection));
                std::vector<core::ObjectWrapperConstPtr> moments;
                motion->getWrappers(moments, typeid(MomentCollection));
                std::vector<core::ObjectWrapperConstPtr> powers;
                motion->getWrappers(powers, typeid(PowerCollection));
                if (!forces.empty()) {
                    kineticItem->addChild(createForcesBranch(motion, QObject::tr("Forces"), getRootForcesIcon(), getForcesIcon()));
                }
                if (!moments.empty()) {
                    kineticItem->addChild(createMomentsBranch(motion, QObject::tr("Moments"), getRootMomentsIcon(), getMomentsIcon()));
                }
                // do rozwiniecia - potrzeba parsować pliki vsk i interpretować strukture kinamatyczna tak jak to robi Vicon
                if (!powers.empty()) {
                    kineticItem->addChild(createPowersBranch(motion, QObject::tr("Powers"), getRootPowersIcon(), getPowersIcon()));
                }
            }

            bool hasMarkers = motion->hasObjectOfType(typeid(MarkerCollection));
            bool hasJoints = motion->hasObjectOfType(typeid(kinematic::JointAnglesCollection));
            bool hasAngles = motion->hasObjectOfType(typeid(AngleCollection));
            if (hasMarkers || hasJoints || hasAngles) {
                QTreeWidgetItem* kinematicItem;
                if (hasJoints || hasMarkers || hasGrf) {
                    Multiserie3DPtr multi(new Multiserie3D(motion));
                    kinematicItem = new HmmTreeItem(multi);
                } else {
                    kinematicItem = new QTreeWidgetItem();
                }
                kinematicItem->setText(0, QObject::tr("Kinematic data"));
                motionItem->addChild(kinematicItem);
                if (hasMarkers) {
                    kinematicItem->addChild(createMarkersBranch(motion, QObject::tr("Markers"), getRootMarkersIcon(), getMarkersIcon()));

                }
                if (hasAngles || hasJoints) {
                    kinematicItem->addChild(createJointsBranch(motion, QObject::tr("Joints"), getRootJointsIcon(), getJointsIcon()));
                }
            }

            if (motion->hasObjectOfType(typeid(VideoChannel))) {
                motionItem->addChild(createVideoBranch(motion, QObject::tr("Videos"), QIcon(), getVideoIcon()));
            }

        }

    }

    return rootItem;
}

QTreeWidgetItem* TreeBuilder::createEMGBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    QTreeWidgetItem* emgItem = new QTreeWidgetItem();
    emgItem->setText(0, rootName);
    emgItem->setIcon(0, rootIcon);
    std::vector<core::ObjectWrapperConstPtr> emgs;
    core::ObjectWrapperConstPtr collectionWrp = motion->getWrapperOfType(typeid(EMGCollection));
    EMGCollectionConstPtr collection = collectionWrp->get();
    
    auto measurements = Measurements::get();

    MeasurementConfigConstPtr config;
    //próbuje pobrać metadane
    try{
        std::vector<core::ObjectWrapperConstPtr> metadata;        
        core::IDataManagerReader::getMetadataForObject(DataManager::getInstance(), motion->getSession(), metadata);
        auto metaITEnd = metadata.end();
        for(auto metaIT = metadata.begin(); metaIT != metaITEnd; ++metaIT){
            core::MetadataConstPtr meta = (*metaIT)->get(false);
            std::string l;

            if(measurements != nullptr && meta != nullptr && meta->value("EMGConf", l) == true) {
                QString confName = QString("EMG_") + QString::fromStdString(l);
                if (measurements->hasConfig(confName)) {
                    config = measurements->getConfig(confName);
                }
            }
        }
    } catch(...) {
        LOG_WARNING("Problem with setting EMG names");
    }

    motion->getWrappers(emgs, typeid(EMGChannel));
    int count = emgs.size();			
    for (int i = 0; i < count; ++i) {	
        EMGChannelPtr c = emgs[i]->get();	
        if (c) {
			//TODO tłumaczenia
			//tak się tego nie powinno wołać - musi być w tr const wartość niezmienna w czasie działania aplikacji -> switch?
            //QString n = QString::fromStdString(emgs[i]->getName());
            //n = config ? config->tr(n) : n;
            QString n = QObject::tr(emgs[i]->getName().c_str());
            EMGFilterHelperPtr channelHelper(new EMGFilterHelper(emgs[i]));
            channelHelper->setMotion(motion);
            HmmTreeItem* channelItem = new HmmTreeItem(channelHelper);
            channelItem->setIcon(0, itemIcon);
            channelItem->setItemAndHelperText(n);
            emgItem->addChild(channelItem);			
        }
    }

    return emgItem;
}

QTreeWidgetItem*  TreeBuilder::createGRFBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    std::vector<core::ObjectWrapperConstPtr> grfCollections;
    motion->getWrappers(grfCollections, typeid(GRFCollection));
    TreeWrappedItemHelperPtr grfCollectionHelper(new TreeWrappedItemHelper(grfCollections[0]));
    HmmTreeItem* grfItem = new HmmTreeItem(grfCollectionHelper);
    grfItem->setItemAndHelperText(rootName);
    grfItem->setIcon(0, rootIcon);
    std::vector<core::ObjectWrapperConstPtr> grfs;
    motion->getWrappers(grfs, typeid(GRFChannel));
    int count = grfs.size();			
    for (int i = 0; i < count; ++i) {
        GRFChannelConstPtr c = grfs[i]->get();
        if (c) {
            TreeItemHelperPtr channelHelper(new NewVector3ItemHelper(grfs[i]));
            channelHelper->setMotion(motion);
            HmmTreeItem* channelItem = new HmmTreeItem(channelHelper);	
            channelItem->setIcon(0, itemIcon);						
            channelItem->setItemAndHelperText(c->getName().c_str());	
            grfItem->addChild(channelItem);			
        }
    }
    return grfItem;
}

QTreeWidgetItem* TreeBuilder::createVideoBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    QTreeWidgetItem* videoItem = new QTreeWidgetItem();
    videoItem->setText(0, rootName);
    videoItem->setIcon(0, rootIcon);
    std::vector<core::ObjectWrapperConstPtr> videos;
    motion->getWrappers(videos, typeid(VideoChannel));
    int count = videos.size();			
    for (int i = 0; i < count; ++i) {			
        TreeWrappedItemHelperPtr channelHelper(new TreeWrappedItemHelper(videos[i]));
        channelHelper->setMotion(motion);
        HmmTreeItem* channelItem = new HmmTreeItem(channelHelper);	
        channelItem->setIcon(0, itemIcon);						
        channelItem->setItemAndHelperText(videos[i]->getName().c_str());			
        videoItem->addChild(channelItem);
    }

    return videoItem;
}

QTreeWidgetItem* TreeBuilder::createJointsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    bool hasJoints = motion->hasObjectOfType(typeid(kinematic::JointAnglesCollection));;
    QTreeWidgetItem* skeletonItem = nullptr;
    if (hasJoints) {    
        JointsItemHelperPtr skeletonHelper(new JointsItemHelper(motion));
        skeletonHelper->setMotion(motion);
        HmmTreeItem* hmmSkeletonItem = new HmmTreeItem(skeletonHelper);
        hmmSkeletonItem->setIcon(0, rootIcon);
        hmmSkeletonItem->setItemAndHelperText(rootName);
        skeletonItem = hmmSkeletonItem;
    } else {
        skeletonItem = new QTreeWidgetItem();
        skeletonItem->setText(0, rootName);
    }
        
    try {
        core::ObjectWrapperConstPtr angleCollection = motion->getWrapperOfType(typeid(AngleCollection));
        AngleCollectionConstPtr m = angleCollection->get();
        tryAddVectorToTree<AngleChannel>(motion, m, "Angle collection", itemIcon, skeletonItem, false);
    } catch (...) {

    }
    
    return skeletonItem;
}

QTreeWidgetItem* TreeBuilder::createMarkersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    core::ObjectWrapperConstPtr markerCollection = motion->getWrapperOfType(typeid(MarkerCollection));
    TreeWrappedItemHelperPtr markersHelper(new TreeWrappedItemHelper(markerCollection));

    markersHelper->setMotion(motion);
    HmmTreeItem* markersItem = new HmmTreeItem(markersHelper);
    markersItem->setIcon(0, rootIcon);
    markersItem->setItemAndHelperText(rootName);		

    MarkerCollectionConstPtr m = markerCollection->get(); 
    tryAddVectorToTree<MarkerChannel>(motion, m, "Marker collection", itemIcon, markersItem, false);
    return markersItem;
}

QTreeWidgetItem* TreeBuilder::createForcesBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    return createTBranch<ForceCollection>(motion, rootName, rootIcon, itemIcon);
}

QTreeWidgetItem* TreeBuilder::createMomentsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    return createTBranch<MomentCollection>(motion, rootName, rootIcon, itemIcon);
}

QTreeWidgetItem* TreeBuilder::createPowersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    return createTBranch<PowerCollection>(motion, rootName, rootIcon, itemIcon);
}
