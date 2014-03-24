#include <corelib/IDataManagerReader.h>
#include <corelib/DataAccessors.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/c3d/C3DCollections.h>
#include <plugins/subject/IMotion.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include <utils/ObjectWrapper.h>
#include "TreeBuilder.h"
#include <plugins/newCommunication/TreeItemHelper.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/video/Wrappers.h>
#include <boost/foreach.hpp>
#include <plugins/subject/ISubject.h>
#include <plugins/newCommunication/IPatient.h>
#include <corelib/PluginCommon.h>
#include <utils/ObjectWrapper.h>

using namespace PluginSubject;
core::IHierarchyItemPtr TreeBuilder::createTree(const QString& rootItemName, const core::ConstObjectsList& sessions)
{
    SubjectHierarchyFilterPtr nullFilter;
    return TreeBuilder::createTree(rootItemName, sessions, nullFilter);
}


core::IHierarchyItemPtr TreeBuilder::createTree( const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject, const SubjectHierarchyFilterPtr & dataFilter )
{
    core::ConstObjectsList sessions;
    subject->getSessions(sessions);
    return createTree(rootItemName, sessions, dataFilter);
}


core::IHierarchyItemPtr TreeBuilder::createTree( const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject )
{
    return TreeBuilder::createTree(rootItemName, subject, SubjectHierarchyFilterPtr());
}



core::IHierarchyItemPtr  TreeBuilder::createTree(const QString& rootItemName, const core::ConstObjectsList& sessions, const SubjectHierarchyFilterPtr & dataFilter)
{
    core::IHierarchyItemPtr rootItem(new core::HierarchyItem(rootItemName, QString()));
    core::ConstObjectsList filteredSessions;
    if(dataFilter != nullptr){
        dataFilter->filterSessions(sessions, filteredSessions);
    }else {
        filteredSessions = sessions;
    }

    for (auto it = filteredSessions.crbegin(); it != filteredSessions.crend(); ++it) {
        
        std::string emgConfigName;
        (*it)->getMetadata("EMGConf", emgConfigName);
        
        core::ConstObjectsList filteredMotions;

        if(dataFilter != nullptr){
            dataFilter->filterSessionMotions(*it, filteredMotions);
        }else {
            PluginSubject::SessionConstPtr s = (*it)->get();
            s->getMotions(filteredMotions);
        }

        BOOST_FOREACH(utils::ObjectWrapperConstPtr motionOW, filteredMotions) {	

            PluginSubject::MotionConstPtr motion = motionOW->get();
           
            QString label(QString::fromUtf8(motion->getLocalName().c_str()));

            std::string metaLabel;
            if(motionOW->getMetadata("label", metaLabel)){
                label = QString::fromStdString(metaLabel);
            }

            QString desc = createDescription(motion);

            core::IHierarchyItemPtr motionItem(new core::HierarchyItem(label, desc));
            rootItem->appendChild(motionItem);
            bool hasEmg = motion->hasObject(typeid(EMGChannel), false);
            bool hasGrf = motion->hasObject(typeid(GRFCollection), false);
            if (hasEmg || hasGrf) {
                core::IHierarchyItemPtr analogItem(new core::HierarchyItem(QObject::tr("Analog data"), desc));
                motionItem->appendChild(analogItem);
                if (hasEmg) {	
                    analogItem->appendChild(createEMGBranch(motion, QObject::tr("EMG"), getRootEMGIcon(), getEMGIcon(),emgConfigName));
                }	         
                
                if (hasGrf) {
                    analogItem->appendChild(createGRFBranch(motion, QObject::tr("GRF"), getRootGRFIcon(), getGRFIcon(), std::string()));
                }
            }

            if (motion->hasObject(typeid(ForceCollection), false) || motion->hasObject(typeid(MomentCollection), false) || motion->hasObject(typeid(PowerCollection), false)) {
                core::IHierarchyItemPtr kineticItem(new core::HierarchyItem(QObject::tr("Kinetic data"), desc));
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
                    kinematicItem = core::IHierarchyItemPtr(new core::HierarchyDataItem(QIcon(), kinematicName, desc, multi));
                } else {
                    kinematicItem = core::IHierarchyItemPtr(new core::HierarchyItem(kinematicName, desc));
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
    QString desc = createDescription(motion);
    core::IHierarchyItemPtr emgItem = utils::make_shared<core::HierarchyItem>(rootName, desc, rootIcon);
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
            (*it)->getMetadata("core/name", l);

            /*core::IHierarchyItemPtr channelItem = utils::make_shared<core::HierarchyDataItem>(*it, itemIcon, QString::fromStdString(l), desc);
            emgItem->appendChild(channelItem);	*/		

            EMGFilterHelperPtr channelHelper(new EMGFilterHelper(*it, getEvents(motion)));
            
            core::IHierarchyItemPtr channelItem(new core::HierarchyDataItem(*it, itemIcon, QString::fromStdString(l), desc, channelHelper));
            emgItem->appendChild(channelItem);			
        }
    }

    return emgItem;
}

core::IHierarchyItemPtr TreeBuilder::createGRFBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    QString desc = createDescription(motion);
    core::ConstObjectsList grfCollections;
    motion->getObjects(grfCollections, typeid(GRFCollection), false);
    //TreeWrappedItemHelperPtr grfCollectionHelper(new TreeWrappedItemHelper(grfCollections.front()));
    core::IHierarchyItemPtr grfItem(new core::HierarchyDataItem(grfCollections.front(), rootIcon, rootName, desc));
    
    core::ConstObjectsList grfs;
    motion->getObjects(grfs, typeid(GRFChannel), false);
    for(auto it = grfs.begin(); it != grfs.end(); ++it) {
        GRFChannelConstPtr c = (*it)->get();
        if (c) {
            std::string l("UNKNOWN");
            (*it)->getMetadata("core/name", l);
            core::HierarchyHelperPtr channelHelper(new NewVector3ItemHelper(*it));
            // todo w jakim celu ustawiany byl motion
            //channelHelper->setMotion(motion);
            core::IHierarchyItemPtr channelItem( new core::HierarchyDataItem(*it, itemIcon,  QString::fromStdString(l), desc, channelHelper));	
            grfItem->appendChild(channelItem);			
        }
    }
    return grfItem;
}


core::IHierarchyItemPtr TreeBuilder::createVideoBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    QString desc = createDescription(motion);
    core::IHierarchyItemPtr videoItem(new core::HierarchyItem(rootName, desc, rootIcon));
   
    core::ConstObjectsList videos;
    motion->getObjects(videos, typeid(VideoChannel), false);	
    for(auto it = videos.begin(); it != videos.end(); ++it) {			
        std::string l("UNKNOWN");
        (*it)->getMetadata("core/name", l);		
        core::IHierarchyItemPtr channelItem(new core::HierarchyDataItem(*it, itemIcon, QString::fromStdString(l), desc));	
        videoItem->appendChild(channelItem);
    }

    return videoItem;
}

core::IHierarchyItemPtr TreeBuilder::createJointsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{

    QString desc = createDescription(motion);
    bool hasJoints = motion->hasObject(typeid(kinematic::JointAnglesCollection), false);
    core::IHierarchyItemPtr skeletonItem;
    if (hasJoints) {    
        JointsItemHelperPtr skeletonHelper(new JointsItemHelper(motion));
        // todo setmotion
        //skeletonHelper->setMotion(motion);
        
        core::ConstObjectsList jCollections;
        motion->getObjects(jCollections, typeid(kinematic::JointAnglesCollection), false);
        if (jCollections.size() != 1) {
            // error
        }

        core::ObjectWrapperConstPtr joints = jCollections.front();



        skeletonItem = core::IHierarchyItemPtr(new core::HierarchyDataItem(joints, rootIcon, rootName, desc, skeletonHelper));
    } else {
        skeletonItem = core::IHierarchyItemPtr(new core::HierarchyItem(rootName, desc, rootIcon));
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
    QString desc = createDescription(motion);
    core::ConstObjectsList mCollections;
    motion->getObjects(mCollections, typeid(MarkerCollection), false);
    core::ObjectWrapperConstPtr markerCollection = mCollections.front();
    core::WrappedItemHelperPtr markersHelper(new core::WrappedItemHelper(markerCollection));
    // todo setmotion
    //markersHelper->setMotion(motion);
    core::IHierarchyItemPtr markersItem(new core::HierarchyDataItem(rootIcon, rootName, desc, markersHelper));

    MarkerCollectionConstPtr m = markerCollection->get(); 
    tryAddVectorToTree<MarkerChannel>(motion, m, "Marker collection", itemIcon, markersItem, false);
    /*int count = markersItem->getNumChildren();
    for (int i = 0; i < count; ++i) {
        core::HierarchyDataItemConstPtr di = utils::dynamic_pointer_cast<const core::HierarchyDataItem>(markersItem->getChild(i));
        if (di) {
            core::HierarchyHelperPtr helperX = utils::make_shared<NewVector3ItemHelper>(di->getData());
            helperX->setText(QObject::tr("All x from session"));
            utils::const_pointer_cast<core::HierarchyDataItem>(di)->addHelper(helperX);

            core::HierarchyHelperPtr helperY = utils::make_shared<NewVector3ItemHelper>(di->getData());
            helperY->setText(QObject::tr("All y from session"));
            utils::const_pointer_cast<core::HierarchyDataItem>(di)->addHelper(helperY);

            core::HierarchyHelperPtr helperZ = utils::make_shared<NewVector3ItemHelper>(di->getData());
            helperZ->setText(QObject::tr("All z from session"));
            utils::const_pointer_cast<core::HierarchyDataItem>(di)->addHelper(helperZ);
        }
    }*/
    return markersItem;
}

core::IHierarchyItemPtr TreeBuilder::createForcesBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    return createTBranch<ForceChannel, ForceCollection>(motion, rootName, rootIcon, itemIcon);
}

core::IHierarchyItemPtr TreeBuilder::createMomentsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    return createTBranch<MomentChannel, MomentCollection>(motion, rootName, rootIcon, itemIcon);
}

core::IHierarchyItemPtr TreeBuilder::createPowersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    return createTBranch<PowerChannel, PowerCollection>(motion, rootName, rootIcon, itemIcon);
}

template <class Channel, class Collection>
core::IHierarchyItemPtr TreeBuilder::createTBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    typedef typename core::ObjectWrapperT<Collection>::Ptr CollectionPtr;
    typedef typename core::ObjectWrapperT<Collection>::ConstPtr CollectionConstPtr;
    //typedef typename Collection::ChannelType Channel;
    QString desc = createDescription(motion);

    core::ConstObjectsList collection;
    motion->getObjects(collection, typeid(Collection), false);
    core::IHierarchyItemPtr rootItem (new core::HierarchyItem(rootName, desc, rootIcon));
    CollectionConstPtr m = collection.front()->get();
    tryAddVectorToTree<Channel>(motion, m, "Collection", itemIcon, rootItem, false);
    return rootItem;
}

template <class Channel, class CollectionPtr>
void TreeBuilder::tryAddVectorToTree( const PluginSubject::MotionConstPtr & motion, const CollectionPtr & collection, const std::string& name, const QIcon& childIcon, core::IHierarchyItemPtr parentItem, bool createContainerItem /*= true */ )
{
    QString desc = createDescription(motion);
    if (collection) {
        std::vector<core::ObjectWrapperConstPtr> wrappers;
        for (int i = 0; i < collection->getNumChannels(); ++i) {
            core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<Channel>();
            wrapper->set(core::const_pointer_cast<Channel>(boost::dynamic_pointer_cast<const Channel>(collection->getChannel(i))));

            static int number = 0;
            std::string();
            wrapper->setMetadata("name", "Serie_" + boost::lexical_cast<std::string>(number));
            wrapper->setMetadata("source", "TreeBuilder");
            wrappers.push_back(wrapper);
        }
        core::IHierarchyItemPtr collectionItem;
        if (createContainerItem) {
            collectionItem = core::IHierarchyItemPtr(new core::HierarchyItem(QString::fromStdString(name), desc));
            parentItem->appendChild(collectionItem);
        } else {
            collectionItem = parentItem;
        }
        int count = wrappers.size();

        EventsCollectionConstPtr events = getEvents(motion);
        
        for (int i = 0; i < count; ++i) {
            VectorChannelConstPtr c = wrappers[i]->get();
            std::string channelName = c->getName();
            std::list<core::HierarchyHelperPtr> helpers;
            NewVector3ItemHelperPtr channelHelper(new NewVector3ItemHelper(wrappers[i], events));
            helpers.push_back(channelHelper);
            helpers.push_back(allTFromSession(channelName, motion->getUnpackedSession(), 0));
            helpers.push_back(allTFromSession(channelName, motion->getUnpackedSession(), 1));
            helpers.push_back(allTFromSession(channelName, motion->getUnpackedSession(), 2));
            helpers.push_back(createNormalized(wrappers[i], motion, c3dlib::C3DParser::IEvent::Left));
            helpers.push_back(createNormalized(wrappers[i], motion, c3dlib::C3DParser::IEvent::Right));
            helpers.push_back(createNormalizedFromAll(channelName, motion->getUnpackedSession(), c3dlib::C3DParser::IEvent::Left));
            helpers.push_back(createNormalizedFromAll(channelName, motion->getUnpackedSession(), c3dlib::C3DParser::IEvent::Right));
            core::IHierarchyItemPtr channelItem (new core::HierarchyDataItem(wrappers[i], childIcon, QString::fromStdString(c->getName()), desc, helpers));
            collectionItem->appendChild(channelItem);
        }
    }
}



core::HierarchyHelperPtr TreeBuilder::allTFromSession( const std::string& channelName, PluginSubject::SessionConstPtr s, int channelNo )
{
    NewMultiserieHelper::ChartWithDescriptionCollection toVisualize;
    core::ConstObjectsList motions;
    s->getMotions(motions);

    for (auto itMotion = motions.begin(); itMotion != motions.end(); ++itMotion) {
        PluginSubject::MotionConstPtr m = (*itMotion)->get();
        core::ConstObjectsList wrappers;
        m->getObjects(wrappers, typeid(utils::DataChannelCollection<VectorChannel>), false);

        EventsCollectionConstPtr events;
        if (m->hasObject(typeid(C3DEventsCollection), false)) {
            core::ConstObjectsList e;
            m->getObjects(e, typeid(C3DEventsCollection), false);
            events = e.front()->get();
        }

        for (auto it = wrappers.begin(); it != wrappers.end(); ++it) {
            VectorChannelCollectionConstPtr collection = (*it)->get();
            int count = collection->getNumChannels();
            for (int i = 0; i < count; ++i) {
                VectorChannelConstPtr channel = collection->getChannel(i);
                if (channel->getName() == channelName) {
                    ScalarChannelReaderInterfacePtr reader(new VectorToScalarAdaptor(channel, channelNo));
                    core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
                    wrapper->set(reader);
                    int no = toVisualize.size();
                    std::string prefix = channelNo == 0 ? "X_" : (channelNo == 1 ? "Y_" : "Z_");
                    wrapper->setMetadata("core/name", prefix + boost::lexical_cast<std::string>(no));
                    std::string src;
                    (*it)->getMetadata("core/source", src);
                    wrapper->setMetadata("core/source", src + boost::lexical_cast<std::string>(no));
                    toVisualize.push_back(NewMultiserieHelper::ChartWithDescription(wrapper, events, m));
                }

            }

        }
    }
    NewMultiserieHelperPtr multi(new NewMultiserieHelper(toVisualize));
    QString text = QString("Multichart/All %1 from session").arg(channelNo == 0 ? "X" : (channelNo == 1 ? "Y" : "Z"));
    multi->setText(text);
    return multi;
}

core::HierarchyHelperPtr TreeBuilder::createNormalized( utils::ObjectWrapperConstPtr wrapper, PluginSubject::MotionConstPtr motion, c3dlib::C3DParser::IEvent::Context context )
{
    NewMultiserieHelper::ChartWithDescriptionCollection toVisualize;
    //MotionConstPtr motion = helper->getMotion();
    EventsCollectionConstPtr events;
    std::vector<FloatPairPtr> segments;
    if (motion->hasObject(typeid(C3DEventsCollection), false)) {
        core::ConstObjectsList wrappers;
        motion->getObjects(wrappers, typeid(C3DEventsCollection), false);
        events = wrappers.front()->get();
        segments = getTimeSegments(events, context);
    }
    std::map<utils::ObjectWrapperConstPtr, QColor> colorMap;
    VectorChannelConstPtr channel = wrapper->get();
    for (int j = 0; j != segments.size(); ++j) {
        FloatPairPtr segment = segments[j];
        for (int channelNo = 0; channelNo <= 2; ++channelNo) {
            ScalarChannelReaderInterfacePtr reader(new VectorToScalarAdaptor(channel, channelNo));
            ScalarChannelReaderInterfacePtr normalized(new ScalarWithTimeSegment(reader, segment->first, segment->second));
            core::ObjectWrapperPtr newWrapper = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
            newWrapper->set(normalized);
            int no = toVisualize.size();
            std::string prefix = channelNo == 0 ? "X_" : (channelNo == 1 ? "Y_" : "Z_");
            colorMap[newWrapper] = channelNo == 0 ? QColor(255, 0, 0) : (channelNo == 1 ? QColor(0, 255, 0) : QColor(0, 0, 255));
            newWrapper->setMetadata("core/name", prefix + ":" + boost::lexical_cast<std::string>(j));
            std::string src;
            wrapper->getMetadata("core/sources", src);
            newWrapper->setMetadata("core/sources", src + boost::lexical_cast<std::string>(no));
            toVisualize.push_back(NewMultiserieHelper::ChartWithDescription(newWrapper, events, motion));
        }
    }
    NewMultiserieHelperPtr multi(new NewMultiserieHelper(toVisualize));
    multi->setColorStrategy(utils::make_shared<ColorMapMultiserieStrategy>(colorMap));

    QString text;
    if (context == c3dlib::C3DParser::IEvent::Left) {
        text = QObject::tr("Multichart/Normalized left from motion");
    } else if (context == c3dlib::C3DParser::IEvent::Right) {
        text = QObject::tr("Multichart/Normalized right from motion");
    } else {
        UTILS_ASSERT(false);
    }
    multi->setText(text);
    return multi;
}

core::HierarchyHelperPtr  TreeBuilder::createNormalizedFromAll( const std::string& channelName, SessionConstPtr s, c3dlib::C3DParser::IEvent::Context context )
{
    NewMultiserieHelper::ChartWithDescriptionCollection toVisualize;
    //SessionConstPtr s = helper->getMotion()->getUnpackedSession();
    core::ConstObjectsList motions;
    s->getMotions(motions);

    std::map<utils::ObjectWrapperConstPtr, QColor> colorMap;
    for (auto itMotion = motions.begin(); itMotion != motions.end(); ++itMotion) {
        PluginSubject::MotionConstPtr m = (*itMotion)->get();
        core::ConstObjectsList wrappers;
        m->getObjects(wrappers, typeid(utils::DataChannelCollection<VectorChannel>), false);

        EventsCollectionConstPtr events;
        std::vector<FloatPairPtr> segments;
        if (m->hasObject(typeid(C3DEventsCollection), false)) {
            core::ConstObjectsList e;
            m->getObjects(e, typeid(C3DEventsCollection), false);
            events = e.front()->get();
            segments = getTimeSegments(events, context);
        }

        for (auto it = wrappers.begin(); it != wrappers.end(); ++it) {
            VectorChannelCollectionConstPtr collection = (*it)->get();
            int count = collection->getNumChannels();
            for (int i = 0; i < count; ++i) {
                VectorChannelConstPtr channel = collection->getChannel(i);
                if (channel->getName() == channelName) {

                    int r = rand() % 200;
                    int g = rand() % 200;
                    int b = rand() % 200;
                    QColor colorX(r + 55,g , b);
                    QColor colorY(r, g + 55, b);
                    QColor colorZ(r, g, b + 55);
                    for (int j = 0; j != segments.size(); ++j) {
                        FloatPairPtr segment = segments[j];

                        for (int channelNo = 0; channelNo <= 2; ++channelNo) {
                            ScalarChannelReaderInterfacePtr reader(new VectorToScalarAdaptor(channel, channelNo));
                            ScalarChannelReaderInterfacePtr normalized(new ScalarWithTimeSegment(reader, segment->first, segment->second));
                            core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
                            wrapper->set(normalized);
                            colorMap[wrapper] = channelNo == 0 ? colorX : (channelNo == 1 ? colorY : colorZ);
                            int no = toVisualize.size();
                            std::string prefix = channelNo == 0 ? "X_" : (channelNo == 1 ? "Y_" : "Z_");
                            wrapper->setMetadata("core/name", prefix + boost::lexical_cast<std::string>(i) + ":" + boost::lexical_cast<std::string>(j));
                            std::string src;
                            (*it)->getMetadata("core/source", src);
                            wrapper->setMetadata("core/source", src + boost::lexical_cast<std::string>(no));
                            toVisualize.push_back(NewMultiserieHelper::ChartWithDescription(wrapper, events, m));
                        }
                    }
                }

            }

        }
    }
    if (toVisualize.empty() == false) {
        NewMultiserieHelperPtr multi(new NewMultiserieHelper(toVisualize));
        multi->setColorStrategy(IMultiserieColorStrategyPtr(new ColorMapMultiserieStrategy(colorMap)));

        QString text;
        if (context == c3dlib::C3DParser::IEvent::Left) {
            text = QObject::tr("Multichart/Normalized left from session");
        } else if (context == c3dlib::C3DParser::IEvent::Right) {
            text = QObject::tr("Multichart/Normalized right from session");
        } else {
            UTILS_ASSERT(false);
        }
        multi->setText(text);
        return multi;
    } 

    //UTILS_ASSERT(false);
    return NewMultiserieHelperPtr();
}

QString TreeBuilder::createDescription( PluginSubject::MotionConstPtr motion )
{
    QString text;
    PluginSubject::SessionConstPtr session = motion->getUnpackedSession();
    PluginSubject::SubjectConstPtr subject = session->getUnpackedSubject();
    
    text += QObject::tr("Motion: ") + QString::fromStdString(motion->getLocalName()) + "\n";
    text += QObject::tr("Session: ") + QString::fromStdString(session->getLocalName()) + "\n";
    text += QObject::tr("Subject: ") + QString::fromStdString(subject->getName()) + "\n";
   
    std::vector<core::ObjectWrapperConstPtr> metadata;      
    try {

		std::string groupName, groupID;

		if(motion->getSession()->getMetadata("groupName", groupName) == true) {
			text += QObject::tr("Owner: %1").arg(groupName.c_str()); 
			if (motion->getSession()->getMetadata("groupID", groupID) == true) {
				text += QString("(%1)").arg(groupID.c_str());
			}
			text += "\n";
		}

		std::string date;
		if(motion->getSession()->getMetadata("data", date) == true) {
			text += QObject::tr("Date: %1\n").arg(date.c_str());
		}
    } catch (...) {
        PLUGIN_LOG_WARNING("Problem with summary window metadata");
    }
    
    if (session->hasObject(typeid(communication::IPatient), false)) {
		core::ConstObjectsList patients;
		session->getObjects(patients, typeid(communication::IPatient), false);
        communication::PatientConstPtr patient = patients.front()->get();
        text += QObject::tr("Patient: ") + QString::fromStdString(patient->getName()) + " " + QString::fromStdString(patient->getSurname()) + "\n";
    }

    if (session->hasObject(typeid(communication::AntropometricData), false)) {
		core::ConstObjectsList antropo;
		session->getObjects(antropo, typeid(communication::AntropometricData), false);
        utils::shared_ptr<const communication::AntropometricData> antro = antropo.front()->get();
        text += " ";
        text += QObject::tr("Weight: ") + QString("%1 ").arg(antro->bodyMass.first) + QString::fromStdString(antro->bodyMass.second) + "\n";
    }
    return text;
}

EventsCollectionConstPtr TreeBuilder::getEvents( const PluginSubject::MotionConstPtr & motion )
{
    EventsCollectionConstPtr events;
    if (motion->hasObject(typeid(C3DEventsCollection), false)) {
        utils::ConstObjectsList m;
        motion->getObjects(m, typeid(C3DEventsCollection), false);
        events = m.front()->get();
    }
    return events;
}

core::IHierarchyItemPtr MotionPerspective::getPerspective( PluginSubject::SubjectPtr subject )
{
    if (hasValidData(subject)) {
        return TreeBuilder::createTree("SUB", subject);
    }

    return core::IHierarchyItemPtr();
}

bool MotionPerspective::hasValidData(PluginSubject::SubjectPtr subject)
{
    std::set<core::TypeInfo> types;
    types.insert(typeid(C3DEventsCollection));
    types.insert(typeid(EMGChannel));
    types.insert(typeid(GRFCollection));
    types.insert(typeid(ForceCollection));
    types.insert(typeid(MomentCollection));
    types.insert(typeid(PowerCollection));
    types.insert(typeid(MarkerCollection));
    types.insert(typeid(kinematic::JointAnglesCollection));
    types.insert(typeid(AngleCollection));
    types.insert(typeid(VideoChannel));
    types.insert(typeid(EMGCollection));
    types.insert(typeid(GRFChannel));

    core::ConstObjectsList sessions;
    subject->getSessions(sessions);
    for (auto it = sessions.crbegin(); it != sessions.crend(); ++it) {
        core::ConstObjectsList motions;
        PluginSubject::SessionConstPtr s = (*it)->get();
        s->getMotions(motions);

        BOOST_FOREACH(utils::ObjectWrapperConstPtr motionOW, motions) {	

            PluginSubject::MotionConstPtr motion = motionOW->get();
            for (auto it = types.begin(); it != types.end(); ++it) {
                if (motion->hasObject(*it, false)) {
                    return true;
                }
            }
        }
    }

    return false;
}
