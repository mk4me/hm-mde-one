#include "MdePCH.h"
#include "TreeBuilder.h"
#include <corelib/IDataManagerReader.h>
#include <corelib/DataAccessors.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/c3d/C3DCollections.h>
#include <plugins/subject/IMotion.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include "TreeItemHelper.h"

using namespace PluginSubject;

core::IHierarchyItemPtr TreeBuilder::createTree(const QString& rootItemName, const core::ConstObjectsList& sessions)
{
    core::IHierarchyItemPtr rootItem(new core::HierarchyItem(rootItemName));
    auto filteredSessions = sessions;
    for (auto it = filteredSessions.crbegin(); it != filteredSessions.crend(); ++it) {
        
        std::string emgConfigName;
        (*it)->tryGetMeta("EMGConf", emgConfigName);
        core::ConstObjectsList filteredMotions;
        
        PluginSubject::SessionConstPtr s = (*it)->get();
        s->getMotions(filteredMotions);
        

        BOOST_FOREACH(core::ObjectWrapperConstPtr motionOW, filteredMotions) {	

            PluginSubject::MotionConstPtr motion = motionOW->get();


            QString label(QString::fromUtf8(motion->getLocalName().c_str()));

            std::string metaLabel;
            if(motionOW->tryGetMeta("label", metaLabel)){
                label = QString::fromStdString(metaLabel);
            }

            core::IHierarchyItemPtr motionItem(new core::HierarchyItem(label));
            rootItem->appendChild(motionItem);
            bool hasEmg = motion->hasObject(typeid(EMGChannel), false);
            bool hasGrf = motion->hasObject(typeid(GRFCollection), false);
            if (hasEmg || hasGrf) {
                core::IHierarchyItemPtr analogItem(new core::HierarchyItem(QObject::tr("Analog data")));
                motionItem->appendChild(analogItem);
                if (hasEmg) {	
                    analogItem->appendChild(createEMGBranch(motion, QObject::tr("EMG"), getRootEMGIcon(), getEMGIcon(),emgConfigName));
                }	         
                
                if (hasGrf) {
                    analogItem->appendChild(createGRFBranch(motion, QObject::tr("GRF"), getRootGRFIcon(), getGRFIcon(), std::string()));
                }
            }

            if (motion->hasObject(typeid(ForceCollection), false) || motion->hasObject(typeid(MomentCollection), false) || motion->hasObject(typeid(PowerCollection), false)) {
                core::IHierarchyItemPtr kineticItem(new core::HierarchyItem(QObject::tr("Kinetic data")));
                motionItem->appendChild(kineticItem);
                core::ConstObjectsList forces;
                motion->getObjects(forces, typeid(ForceCollection), false);
                core::ConstObjectsList moments;
                motion->getObjects(moments, typeid(MomentCollection), false);
                core::ConstObjectsList powers;
                motion->getObjects(powers, typeid(PowerCollection), false);
                if (!forces.empty()) {
                    kineticItem->appendChild(createForcesBranch(motion, QObject::tr("Forces"), getRootForcesIcon(), getForcesIcon(), std::string()));
                }
                if (!moments.empty()) {
                    kineticItem->appendChild(createMomentsBranch(motion, QObject::tr("Moments"), getRootMomentsIcon(), getMomentsIcon(), std::string()));
                }
                // do rozwiniecia - potrzeba parsować pliki vsk i interpretować strukture kinamatyczna tak jak to robi Vicon
                if (!powers.empty()) {
                    kineticItem->appendChild(createPowersBranch(motion, QObject::tr("Powers"), getRootPowersIcon(), getPowersIcon(), std::string()));
                }
            }

            bool hasMarkers = motion->hasObject(typeid(MarkerCollection), false);
            bool hasJoints = motion->hasObject(typeid(kinematic::JointAnglesCollection), false);
            bool hasAngles = motion->hasObject(typeid(AngleCollection), false);
            if (hasMarkers || hasJoints || hasAngles) {
                core::IHierarchyItemPtr kinematicItem;
                QString kinematicName = QObject::tr("Kinematic data");
                 
                if (false && hasJoints || hasMarkers || hasGrf) {
                    Multiserie3DPtr multi(new Multiserie3D(motion));
                    kinematicItem = core::IHierarchyItemPtr(new core::HierarchyDataItem(QIcon(), kinematicName, multi));
                } else {
                    kinematicItem = core::IHierarchyItemPtr(new core::HierarchyItem(kinematicName));
                }
                motionItem->appendChild(kinematicItem);
                if (hasMarkers) {
                    kinematicItem->appendChild(createMarkersBranch(motion, QObject::tr("Markers"), getRootMarkersIcon(), getMarkersIcon(), std::string()));

                }
                if (hasAngles || hasJoints) {
                    kinematicItem->appendChild(createJointsBranch(motion, QObject::tr("Joints"), getRootJointsIcon(), getJointsIcon(), std::string()));
                }
            }

            if (motion->hasObject(typeid(VideoChannel), false)) {
                motionItem->appendChild(createVideoBranch(motion, QObject::tr("Videos"), QIcon(), getVideoIcon(), std::string()));
            }
        }
    }

    return rootItem;
}

core::IHierarchyItemPtr TreeBuilder::createEMGBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & conf )
{
    core::IHierarchyItemPtr emgItem(new core::HierarchyItem(rootName, rootIcon));
    core::ConstObjectsList emgs;
    motion->getObjects(emgs, typeid(EMGCollection), false);

    EMGCollectionConstPtr collection = emgs.front()->get();

    //auto measurements = Measurements::get();
    //MeasurementConfigConstPtr config;
    ////próbuje pobrać metadane
    //try{
    //    if(measurements != nullptr && conf.empty() == false) {
    //        QString confName = QString("EMG_") + QString::fromStdString(conf);
    //        if (measurements->hasConfig(confName)) {
    //            config = measurements->getConfig(confName);
    //        }
    //    }
    //} catch(...) {
    //    PLUGIN_LOG_WARNING("Problem with setting EMG names");
    //}

    core::ConstObjectsList().swap(emgs);

    motion->getObjects(emgs, typeid(EMGChannel), false);		
    for(auto it = emgs.begin(); it != emgs.end(); ++it) {	
        EMGChannelConstPtr c = (*it)->get();	
        if (c) {
            std::string l("UNKNOWN");
            (*it)->tryGetMeta("core/name", l);

            core::IHierarchyItemPtr channelItem(new core::HierarchyDataItem(*it, itemIcon, QString::fromStdString(l)));
            emgItem->appendChild(channelItem);			

            //EMGFilterHelperPtr channelHelper(new EMGFilterHelper(*it));
            //channelHelper->setMotion(motion);
            //
            //core::IHierarchyItemPtr channelItem(new HierarchyDataItem(channelHelper));
            //channelItem->setIcon(0, itemIcon);
            //channelItem->setItemAndHelperText(QString::fromStdString(l));
            //emgItem->addChild(channelItem);			
        }
    }

    return emgItem;
}

core::IHierarchyItemPtr TreeBuilder::createGRFBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    core::ConstObjectsList grfCollections;
    motion->getObjects(grfCollections, typeid(GRFCollection), false);
    //TreeWrappedItemHelperPtr grfCollectionHelper(new TreeWrappedItemHelper(grfCollections.front()));
    core::IHierarchyItemPtr grfItem(new core::HierarchyDataItem(grfCollections.front(), rootIcon, rootName));
    
    core::ConstObjectsList grfs;
    motion->getObjects(grfs, typeid(GRFChannel), false);
    for(auto it = grfs.begin(); it != grfs.end(); ++it) {
        GRFChannelConstPtr c = (*it)->get();
        if (c) {
            std::string l("UNKNOWN");
            (*it)->tryGetMeta("core/name", l);
            core::HierarchyHelperPtr channelHelper(new NewVector3ItemHelper(*it));
            // todo w jakim celu ustawiany byl motion
            //channelHelper->setMotion(motion);
            core::IHierarchyItemPtr channelItem( new core::HierarchyDataItem(itemIcon, QString::fromStdString(l), channelHelper));	
            grfItem->appendChild(channelItem);			
        }
    }
    return grfItem;
}


core::IHierarchyItemPtr TreeBuilder::createVideoBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    core::IHierarchyItemPtr videoItem(new core::HierarchyItem(rootName, rootIcon));
    
    core::ConstObjectsList videos;
    motion->getObjects(videos, typeid(VideoChannel), false);	
    for(auto it = videos.begin(); it != videos.end(); ++it) {			
        std::string l("UNKNOWN");
        (*it)->tryGetMeta("core/name", l);		
        core::IHierarchyItemPtr channelItem(new core::HierarchyDataItem(*it, itemIcon, QString::fromStdString(l)));	
        videoItem->appendChild(channelItem);
    }

    return videoItem;
}

core::IHierarchyItemPtr TreeBuilder::createJointsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    bool hasJoints = motion->hasObject(typeid(kinematic::JointAnglesCollection), false);
    core::IHierarchyItemPtr skeletonItem;
    if (hasJoints) {    
        JointsItemHelperPtr skeletonHelper(new JointsItemHelper(motion));
        // todo setmotion
        //skeletonHelper->setMotion(motion);
        skeletonItem = core::IHierarchyItemPtr(new core::HierarchyDataItem(rootIcon, rootName, skeletonHelper));
    } else {
        skeletonItem = core::IHierarchyItemPtr(new core::HierarchyItem(rootName, rootIcon));
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

core::IHierarchyItemPtr TreeBuilder::createMarkersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    core::ConstObjectsList mCollections;
    motion->getObjects(mCollections, typeid(MarkerCollection), false);
    core::ObjectWrapperConstPtr markerCollection = mCollections.front();
    core::WrappedItemHelperPtr markersHelper(new core::WrappedItemHelper(markerCollection));
    // todo setmotion
    //markersHelper->setMotion(motion);
    core::IHierarchyItemPtr markersItem(new core::HierarchyDataItem(rootIcon, rootName, markersHelper));

    MarkerCollectionConstPtr m = markerCollection->get(); 
    tryAddVectorToTree<MarkerChannel>(motion, m, "Marker collection", itemIcon, markersItem, false);
    return markersItem;
}

core::IHierarchyItemPtr TreeBuilder::createForcesBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    return createTBranch<ForceCollection>(motion, rootName, rootIcon, itemIcon);
}

core::IHierarchyItemPtr TreeBuilder::createMomentsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    return createTBranch<MomentCollection>(motion, rootName, rootIcon, itemIcon);
}

core::IHierarchyItemPtr TreeBuilder::createPowersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    return createTBranch<PowerCollection>(motion, rootName, rootIcon, itemIcon);
}

template <class Collection>
core::IHierarchyItemPtr TreeBuilder::createTBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    typedef typename core::ObjectWrapperT<Collection>::Ptr CollectionPtr;
    typedef typename core::ObjectWrapperT<Collection>::ConstPtr CollectionConstPtr;
    typedef typename Collection::ChannelType Channel;

    core::ConstObjectsList collection;
    motion->getObjects(collection, typeid(Collection), false);
    core::IHierarchyItemPtr rootItem (new core::HierarchyItem(rootName, rootIcon));
    CollectionConstPtr m = collection.front()->get();
    tryAddVectorToTree<Channel>(motion, m, "Collection", itemIcon, rootItem, false);
    return rootItem;
}

template <class Channel, class CollectionPtr>
void TreeBuilder::tryAddVectorToTree( const PluginSubject::MotionConstPtr & motion, const CollectionPtr & collection, const std::string& name, const QIcon& childIcon, core::IHierarchyItemPtr parentItem, bool createContainerItem /*= true */ )
{
    if (collection) {
        std::vector<core::ObjectWrapperConstPtr> wrappers;
        for (int i = 0; i < collection->getNumChannels(); ++i) {
            core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<VectorChannel>();
            wrapper->set(core::const_pointer_cast<VectorChannel>(boost::dynamic_pointer_cast<const VectorChannel>(collection->getChannel(i))));

            static int number = 0;
            std::string();
            (*wrapper)["name"] = "Serie_" + boost::lexical_cast<std::string>(number);
            (*wrapper)["source"] = "TreeBuilder";
            wrappers.push_back(wrapper);
        }
        core::IHierarchyItemPtr collectionItem;
        if (createContainerItem) {
            collectionItem = core::IHierarchyItemPtr(new core::HierarchyItem(QString::fromStdString(name)));
            parentItem->appendChild(collectionItem);
        } else {
            collectionItem = parentItem;
        }
        int count = wrappers.size();
        for (int i = 0; i < count; ++i) {
            VectorChannelConstPtr c = wrappers[i]->get();
            core::HierarchyHelperPtr channelHelper(new NewVector3ItemHelper(wrappers[i]));
            // todo setmotion
            //channelHelper->setMotion(motion);

            core::IHierarchyItemPtr channelItem (new core::HierarchyDataItem(childIcon, QString::fromStdString(c->getName()), channelHelper));
            collectionItem->appendChild(channelItem);
        }
    }
}
