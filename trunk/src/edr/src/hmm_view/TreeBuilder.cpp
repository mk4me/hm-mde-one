#include "hmmPCH.h"
#include "TreeBuilder.h"
#include "HmmMainWindow.h"


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

        QIcon icon(core::getResourceString("icons/charts.png"));
        QIcon icon3D(core::getResourceString("icons/3d.png"));
        QIcon iconVideo(core::getResourceString("icons/video.png"));
        std::vector<MotionConstPtr> motions;
        session->getMotions(motions);
        BOOST_FOREACH(MotionConstPtr motion, motions) {	
            QTreeWidgetItem* motionItem = new QTreeWidgetItem();  
            motionItem->setText(0, motion->getLocalName().c_str());
            rootItem->addChild(motionItem);
            bool hasEmg = motion->hasObjectOfType(typeid(EMGChannel));
            bool hasGrf = motion->hasObjectOfType(typeid(GRFCollection));
            if (hasEmg || hasGrf) {
                QTreeWidgetItem* analogItem = new QTreeWidgetItem();
                analogItem->setText(0, QObject::tr("Analog data"));
                motionItem->addChild(analogItem);
                if (hasEmg) {	
                    analogItem->addChild(createEMGBranch(motion, "EMG", icon));
                }	

                if (hasGrf) {
                    analogItem->addChild(createGRFBranch(motion, "GRF", icon));
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
                if (forces.size() > 0) {
                    ForceCollectionPtr f = forces[0]->get();
                    tryAddVectorToTree<ForceChannel>(motion, f, "Forces", &icon3D, kineticItem);
                }
                if (moments.size() > 0) {
                    MomentCollectionPtr m = moments[0]->get();
                    tryAddVectorToTree<MomentChannel>(motion, m, "Moments", &icon3D, kineticItem);
                }
                // do rozwiniecia - potrzeba parsowac pliki vsk i interpretowac strukture kinamatyczna tak jak to robi Vicon
                //tryAddVectorToTree(motion->getAngles(), "Angles", &icon3D, kineticItem);
                if (powers.size() > 0) {
                    PowerCollectionPtr p = powers[0]->get();
                    tryAddVectorToTree<PowerChannel>(motion, p, "Powers", &icon3D, kineticItem);
                }
            }

            bool hasMarkers = motion->hasObjectOfType(typeid(MarkerCollection));
            LOG_DEBUG( "checkin joints" );
            bool hasJoints = motion->hasObjectOfType(typeid(kinematic::JointAnglesCollection));
            LOG_DEBUG( "Motion " << motion->getName() << " " << motion->getLocalName() << " joints status: " << hasJoints ? "true" : "false" );
            if (hasMarkers || hasJoints) {
                QTreeWidgetItem* kinematicItem = new QTreeWidgetItem();
                kinematicItem->setText(0, QObject::tr("Kinematic data"));
                motionItem->addChild(kinematicItem);
                if (hasMarkers) {
                    kinematicItem->addChild(createMarkersBranch(motion, "Markers", icon3D));

                }

                if (hasJoints) {
                    kinematicItem->addChild(createJointsBranch(motion, "Joints", icon3D));

                }

                if (hasJoints || hasMarkers || hasGrf) {
                    //item2Helper[kinematicItem] = TreeItemHelperPtr(new Multiserie3D(motion));
                }
            }

            if (motion->hasObjectOfType(typeid(VideoChannel))) {
                motionItem->addChild(createVideoBranch(motion, "Videos", icon3D));
            }

        }

    }

    return rootItem;
}

QTreeWidgetItem* TreeBuilder::createEMGBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& itemIcon )
{
    QTreeWidgetItem* emgItem = new QTreeWidgetItem();
    emgItem->setText(0, rootName);
    std::vector<core::ObjectWrapperConstPtr> emgs;
    motion->getWrappers(emgs, typeid(EMGChannel));
    int count = emgs.size();			
    for (int i = 0; i < count; i++) {	
        EMGChannelPtr c = emgs[i]->get();	
        if (c) {
            QTreeWidgetItem* channelItem = new NewChartItemHelper(emgs[i]);
            channelItem->setIcon(0, itemIcon);	
            channelItem->setText(0, c->getName().c_str());			
            emgItem->addChild(channelItem);			
        }
    }

    return emgItem;
}

QTreeWidgetItem*  TreeBuilder::createGRFBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& itemIcon )
{
    std::vector<core::ObjectWrapperConstPtr> grfCollections;
    motion->getWrappers(grfCollections, typeid(GRFCollection));
    QTreeWidgetItem* grfItem = new TreeWrappedItemHelper(grfCollections[0]);
    grfItem->setText(0, rootName);
    //grfItem->setIcon(0, icon3D);
    std::vector<core::ObjectWrapperConstPtr> grfs;
    motion->getWrappers(grfs, typeid(GRFChannel));
    int count = grfs.size();			
    for (int i = 0; i < count; i++) {
        GRFChannelConstPtr c = grfs[i]->get();
        if (c) {
            QTreeWidgetItem* channelItem = new NewVector3ItemHelper(grfs[i]);	
            channelItem->setIcon(0, itemIcon);						
            channelItem->setText(0, c->getName().c_str());			
            grfItem->addChild(channelItem);			
        }
    }

    return grfItem;
}

QTreeWidgetItem* TreeBuilder::createVideoBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& itemIcon )
{
    QTreeWidgetItem* videoItem = new QTreeWidgetItem();
    videoItem->setText(0, rootName);
    std::vector<core::ObjectWrapperConstPtr> videos;
    motion->getWrappers(videos, typeid(VideoChannel));
    int count = videos.size();			
    for (int i = 0; i < count; i++) {							
        QTreeWidgetItem* channelItem = new TreeWrappedItemHelper(videos[i]);	

        channelItem->setIcon(0, itemIcon);						
        channelItem->setText(0, videos[i]->getName().c_str());			
        videoItem->addChild(channelItem);
    }

    return videoItem;
}

QTreeWidgetItem* TreeBuilder::createJointsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& itemIcon )
{
   // QTreeWidgetItem* skeletonItem = new QTreeWidgetItem();
    LOG_DEBUG( "Creating joints branch" );
    QTreeWidgetItem* skeletonItem = new JointsItemHelper(motion);
    skeletonItem->setIcon(0, itemIcon);
    skeletonItem->setText(0, rootName);
    return skeletonItem;
}

QTreeWidgetItem* TreeBuilder::createMarkersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& itemIcon )
{
    core::ObjectWrapperConstPtr markerCollection = motion->getWrapperOfType(typeid(MarkerCollection));
    QTreeWidgetItem* markersItem = new TreeWrappedItemHelper(markerCollection);
    markersItem->setIcon(0, itemIcon);
    markersItem->setText(0, rootName);		

    MarkerCollectionConstPtr m = markerCollection->get(); 
    tryAddVectorToTree<MarkerChannel>(motion, m, "Marker collection", &itemIcon, markersItem, false);
    return markersItem;
}