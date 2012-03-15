#include "hmmPCH.h"
#include "TreeBuilder.h"
#include "HmmMainWindow.h"
#include "EMGFilter.h"

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
            motionItem->setText(0, motion->getLocalName().c_str());
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
                if (forces.size() > 0) {
                    kineticItem->addChild(createForcesBranch(motion, QObject::tr("Forces"), getRootForcesIcon(), getForcesIcon()));
                }
                if (moments.size() > 0) {
                    kineticItem->addChild(createMomentsBranch(motion, QObject::tr("Moments"), getRootMomentsIcon(), getMomentsIcon()));
                }
                // do rozwiniecia - potrzeba parsowac pliki vsk i interpretowac strukture kinamatyczna tak jak to robi Vicon
                if (powers.size() > 0) {
                    kineticItem->addChild(createPowersBranch(motion, QObject::tr("Powers"), getRootPowersIcon(), getPowersIcon()));
                }
            }

            bool hasMarkers = motion->hasObjectOfType(typeid(MarkerCollection));
            bool hasJoints = motion->hasObjectOfType(typeid(kinematic::JointAnglesCollection));
            if (hasMarkers || hasJoints) {
                QTreeWidgetItem* kinematicItem;
                if (hasJoints || hasMarkers || hasGrf) {
                    kinematicItem = new Multiserie3D(motion);
                } else {
                    kinematicItem = new QTreeWidgetItem();
                }
                kinematicItem->setText(0, QObject::tr("Kinematic data"));
                motionItem->addChild(kinematicItem);
                if (hasMarkers) {
                    kinematicItem->addChild(createMarkersBranch(motion, QObject::tr("Markers"), getRootMarkersIcon(), getMarkersIcon()));

                }

                if (hasJoints) {
                    kinematicItem->addChild(createJointsBranch(motion, QObject::tr("Joints"), getRootJointsIcon(), getJointsIcon()));

                }

                /*if  {
                    item2Helper[kinematicItem] = TreeItemHelperPtr(new Multiserie3D(motion));
                }*/
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
    motion->getWrappers(emgs, typeid(EMGChannel));
    int count = emgs.size();			
    for (int i = 0; i < count; i++) {	
        EMGChannelPtr c = emgs[i]->get();	
        if (c) {
            EMGFilterHelper* channelItem = new EMGFilterHelper(emgs[i]);
            channelItem->setIcon(0, itemIcon);	
            channelItem->setText(0, c->getName().c_str());			
            channelItem->setMotion(motion);
            emgItem->addChild(channelItem);			
        }
    }

    return emgItem;
}

QTreeWidgetItem*  TreeBuilder::createGRFBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    std::vector<core::ObjectWrapperConstPtr> grfCollections;
    motion->getWrappers(grfCollections, typeid(GRFCollection));
    QTreeWidgetItem* grfItem = new TreeWrappedItemHelper(grfCollections[0]);
    grfItem->setText(0, rootName);
    grfItem->setIcon(0, rootIcon);
    std::vector<core::ObjectWrapperConstPtr> grfs;
    motion->getWrappers(grfs, typeid(GRFChannel));
    int count = grfs.size();			
    for (int i = 0; i < count; i++) {
        GRFChannelConstPtr c = grfs[i]->get();
        if (c) {
            TreeItemHelper* channelItem = new NewVector3ItemHelper(grfs[i]);	
            channelItem->setIcon(0, itemIcon);						
            channelItem->setText(0, c->getName().c_str());		
            channelItem->setMotion(motion);
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
    for (int i = 0; i < count; i++) {							
        QTreeWidgetItem* channelItem = new TreeWrappedItemHelper(videos[i]);	

        channelItem->setIcon(0, itemIcon);						
        channelItem->setText(0, videos[i]->getName().c_str());			
        videoItem->addChild(channelItem);
    }

    return videoItem;
}

QTreeWidgetItem* TreeBuilder::createJointsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    QTreeWidgetItem* skeletonItem = new JointsItemHelper(motion);
    skeletonItem->setIcon(0, rootIcon);
    skeletonItem->setText(0, rootName);

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
    QTreeWidgetItem* markersItem = new TreeWrappedItemHelper(markerCollection);
    markersItem->setIcon(0, rootIcon);
    markersItem->setText(0, rootName);		

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
