#include "hmmPCH.h"
#include "TreeBuilder.h"
#include "HmmMainWindow.h"
#include <plugins/hmmlib/EMGFilter.h>
#include <corelib/IDataManagerReader.h>
#include "Measurements.h"
#include <corelib/DataAccessors.h>

using namespace PluginSubject;

QTreeWidgetItem* TreeBuilder::createTree(const QString& rootItemName, const core::ConstObjectsList& sessions)
{
    SubjectHierarchyFilterPtr nullFilter;
    return TreeBuilder::createTree(rootItemName, sessions, nullFilter);
}

QTreeWidgetItem* TreeBuilder::createTree(const QString& rootItemName, const core::ConstObjectsList& sessions, const SubjectHierarchyFilterPtr & dataFilter)
{
    QTreeWidgetItem* rootItem = new QTreeWidgetItem();
    rootItem->setText(0, rootItemName);

	//SessionConstPtr session = dataFilter ? dataFilter->filterData(*it) : *it;
	core::ConstObjectsList filteredSessions;
	if(dataFilter != nullptr){
		dataFilter->filterSessions(sessions, filteredSessions);
	}else {
		filteredSessions = sessions;
	}

    for (auto it = filteredSessions.crbegin(); it != filteredSessions.crend(); ++it) {
		std::string emgConfigName;
		(*it)->tryGetMeta("EMGConf", emgConfigName);

        core::ConstObjectsList filteredMotions;

		if(dataFilter != nullptr){
			dataFilter->filterSessionMotions(*it, filteredMotions);
		}else {
			PluginSubject::SessionConstPtr s = (*it)->get();
			s->getMotions(filteredMotions);
		}
        
        BOOST_FOREACH(core::ObjectWrapperConstPtr motionOW, filteredMotions) {	

			PluginSubject::MotionConstPtr motion = motionOW->get();

            QTreeWidgetItem* motionItem = new QTreeWidgetItem();  

			QString label(QString::fromUtf8(motion->getLocalName().c_str()));

			std::string metaLabel;
			if(motionOW->tryGetMeta("label", metaLabel)){
				label = QString::fromStdString(metaLabel);
			}

            motionItem->setText(0, label);
            rootItem->addChild(motionItem);
            bool hasEmg = motion->hasObject(typeid(EMGChannel), false);
            bool hasGrf = motion->hasObject(typeid(GRFCollection), false);
            if (hasEmg || hasGrf) {
                QTreeWidgetItem* analogItem = new QTreeWidgetItem();
                analogItem->setText(0, QObject::tr("Analog data"));
                motionItem->addChild(analogItem);
                if (hasEmg) {	
                    analogItem->addChild(createEMGBranch(motion, QObject::tr("EMG"), getRootEMGIcon(), getEMGIcon(),emgConfigName));
                }	         
                             
                if (hasGrf) {
                    analogItem->addChild(createGRFBranch(motion, QObject::tr("GRF"), getRootGRFIcon(), getGRFIcon(), std::string()));
                }
            }

            if (motion->hasObject(typeid(ForceCollection), false) || motion->hasObject(typeid(MomentCollection), false) || motion->hasObject(typeid(PowerCollection), false)) {
                QTreeWidgetItem* kineticItem = new QTreeWidgetItem();
                kineticItem->setText(0, QObject::tr("Kinetic data"));
                motionItem->addChild(kineticItem);
                core::ConstObjectsList forces;
                motion->getObjects(forces, typeid(ForceCollection), false);
                core::ConstObjectsList moments;
                motion->getObjects(moments, typeid(MomentCollection), false);
                core::ConstObjectsList powers;
                motion->getObjects(powers, typeid(PowerCollection), false);
                if (!forces.empty()) {
                    kineticItem->addChild(createForcesBranch(motion, QObject::tr("Forces"), getRootForcesIcon(), getForcesIcon(), std::string()));
                }
                if (!moments.empty()) {
                    kineticItem->addChild(createMomentsBranch(motion, QObject::tr("Moments"), getRootMomentsIcon(), getMomentsIcon(), std::string()));
                }
                // do rozwiniecia - potrzeba parsować pliki vsk i interpretować strukture kinamatyczna tak jak to robi Vicon
                if (!powers.empty()) {
                    kineticItem->addChild(createPowersBranch(motion, QObject::tr("Powers"), getRootPowersIcon(), getPowersIcon(), std::string()));
                }
            }

            bool hasMarkers = motion->hasObject(typeid(MarkerCollection), false);
            bool hasJoints = motion->hasObject(typeid(kinematic::JointAnglesCollection), false);
            bool hasAngles = motion->hasObject(typeid(AngleCollection), false);
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
                    kinematicItem->addChild(createMarkersBranch(motion, QObject::tr("Markers"), getRootMarkersIcon(), getMarkersIcon(), std::string()));

                }
                if (hasAngles || hasJoints) {
                    kinematicItem->addChild(createJointsBranch(motion, QObject::tr("Joints"), getRootJointsIcon(), getJointsIcon(), std::string()));
                }
            }

            if (motion->hasObject(typeid(VideoChannel), false)) {
                motionItem->addChild(createVideoBranch(motion, QObject::tr("Videos"), QIcon(), getVideoIcon(), std::string()));
            }
        }
    }

    return rootItem;
}

QTreeWidgetItem* TreeBuilder::createEMGBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & conf )
{
    QTreeWidgetItem* emgItem = new QTreeWidgetItem();
    emgItem->setText(0, rootName);
    emgItem->setIcon(0, rootIcon);
    core::ConstObjectsList emgs;
    motion->getObjects(emgs, typeid(EMGCollection), false);
    EMGCollectionConstPtr collection = emgs.front()->get();
    
    auto measurements = Measurements::get();

    MeasurementConfigConstPtr config;
    //próbuje pobrać metadane
	try{
		if(measurements != nullptr && conf.empty() == false) {
			QString confName = QString("EMG_") + QString::fromStdString(conf);
			if (measurements->hasConfig(confName)) {
				config = measurements->getConfig(confName);
			}
		}
    } catch(...) {
        PLUGIN_LOG_WARNING("Problem with setting EMG names");
    }

	core::ConstObjectsList().swap(emgs);

    motion->getObjects(emgs, typeid(EMGChannel), false);		
    for(auto it = emgs.begin(); it != emgs.end(); ++it) {	
        EMGChannelConstPtr c = (*it)->get();	
        if (c) {
			std::string l("UNKNOWN");
			(*it)->tryGetMeta("core/name", l);

            EMGFilterHelperPtr channelHelper(new EMGFilterHelper(*it));
            channelHelper->setMotion(motion);
            HmmTreeItem* channelItem = new HmmTreeItem(channelHelper);
            channelItem->setIcon(0, itemIcon);
            channelItem->setItemAndHelperText(QString::fromStdString(l));
            emgItem->addChild(channelItem);			
        }
    }

    return emgItem;
}

QTreeWidgetItem*  TreeBuilder::createGRFBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    core::ConstObjectsList grfCollections;
    motion->getObjects(grfCollections, typeid(GRFCollection), false);
    TreeWrappedItemHelperPtr grfCollectionHelper(new TreeWrappedItemHelper(grfCollections.front()));
    HmmTreeItem* grfItem = new HmmTreeItem(grfCollectionHelper);
    grfItem->setItemAndHelperText(rootName);
    grfItem->setIcon(0, rootIcon);
    core::ConstObjectsList grfs;
    motion->getObjects(grfs, typeid(GRFChannel), false);
    for(auto it = grfs.begin(); it != grfs.end(); ++it) {
        GRFChannelConstPtr c = (*it)->get();
        if (c) {
			std::string l("UNKNOWN");
			(*it)->tryGetMeta("core/name", l);
            TreeItemHelperPtr channelHelper(new NewVector3ItemHelper(*it));
            channelHelper->setMotion(motion);
            HmmTreeItem* channelItem = new HmmTreeItem(channelHelper);	
            channelItem->setIcon(0, itemIcon);						
            channelItem->setItemAndHelperText(QString::fromStdString(l));	
            grfItem->addChild(channelItem);			
        }
    }
    return grfItem;
}

QTreeWidgetItem* TreeBuilder::createVideoBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    QTreeWidgetItem* videoItem = new QTreeWidgetItem();
    videoItem->setText(0, rootName);
    videoItem->setIcon(0, rootIcon);
    core::ConstObjectsList videos;
    motion->getObjects(videos, typeid(VideoChannel), false);	
    for(auto it = videos.begin(); it != videos.end(); ++it) {			
        TreeWrappedItemHelperPtr channelHelper(new TreeWrappedItemHelper(*it));
        channelHelper->setMotion(motion);
        HmmTreeItem* channelItem = new HmmTreeItem(channelHelper);	
        channelItem->setIcon(0, itemIcon);						
		std::string l("UNKNOWN");
		(*it)->tryGetMeta("core/name", l);		
		channelItem->setItemAndHelperText(QString::fromStdString(l));
        videoItem->addChild(channelItem);
    }

    return videoItem;
}

QTreeWidgetItem* TreeBuilder::createJointsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    bool hasJoints = motion->hasObject(typeid(kinematic::JointAnglesCollection), false);
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
		core::ConstObjectsList aCollections;
		motion->getObjects(aCollections, typeid(AngleCollection), false);
        core::ObjectWrapperConstPtr angleCollection = aCollections.front();
        AngleCollectionConstPtr m = angleCollection->get();
        tryAddVectorToTree<AngleChannel>(motion, m, "Angle collection", itemIcon, skeletonItem, false);
    } catch (...) {

    }
    
    return skeletonItem;
}

QTreeWidgetItem* TreeBuilder::createMarkersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
	core::ConstObjectsList mCollections;
	motion->getObjects(mCollections, typeid(MarkerCollection), false);
    core::ObjectWrapperConstPtr markerCollection = mCollections.front();
    TreeWrappedItemHelperPtr markersHelper(new TreeWrappedItemHelper(markerCollection));

    markersHelper->setMotion(motion);
    HmmTreeItem* markersItem = new HmmTreeItem(markersHelper);
    markersItem->setIcon(0, rootIcon);
    markersItem->setItemAndHelperText(rootName);		

    MarkerCollectionConstPtr m = markerCollection->get(); 
    tryAddVectorToTree<MarkerChannel>(motion, m, "Marker collection", itemIcon, markersItem, false);
    return markersItem;
}

QTreeWidgetItem* TreeBuilder::createForcesBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    return createTBranch<ForceCollection>(motion, rootName, rootIcon, itemIcon);
}

QTreeWidgetItem* TreeBuilder::createMomentsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    return createTBranch<MomentCollection>(motion, rootName, rootIcon, itemIcon);
}

QTreeWidgetItem* TreeBuilder::createPowersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    return createTBranch<PowerCollection>(motion, rootName, rootIcon, itemIcon);
}
