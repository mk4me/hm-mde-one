#include "hmmPCH.h"
#include "TreeBuilder.h"
#include "HmmMainWindow.h"


QTreeWidgetItem* TreeBuilder::createTree(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions)
{
    DataFilterPtr nullFilter;
    return TreeBuilder::createTree(rootItemName, sessions, nullFilter);
}

QTreeWidgetItem* TreeBuilder::createTree(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions, DataFilterPtr dataFilter)
{
    QTreeWidgetItem* rootItem = new QTreeWidgetItem();
    rootItem->setText(0, rootItemName);
    for (int i = sessions.size() - 1; i >= 0; --i) {
        SessionConstPtr session = dataFilter ? dataFilter->filterData(sessions[i]) : sessions[i];

        QIcon icon(core::getResourceString("icons/charts.png"));
        QIcon icon3D(core::getResourceString("icons/3d.png"));
        QIcon iconVideo(core::getResourceString("icons/video.png"));

        BOOST_FOREACH(MotionPtr motion, session->getMotions()) {	
            QTreeWidgetItem* motionItem = new QTreeWidgetItem();  
            motionItem->setText(0, motion->getName().c_str());
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
                std::vector<core::ObjectWrapperPtr> forces = motion->getWrappers(typeid(ForceCollection));
                std::vector<core::ObjectWrapperPtr> moments = motion->getWrappers(typeid(MomentCollection));
                std::vector<core::ObjectWrapperPtr> powers = motion->getWrappers(typeid(PowerCollection));
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
            bool hasJoints = motion->hasObjectOfType(typeid(kinematic::JointAnglesCollection));

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

            //QTreeWidgetItem* testItem = new QTreeWidgetItem();
            //testItem->setText(0, "TEST");
            //motionItem->addChild(testItem);
            //if (motion->hasObjectOfType(typeid(ForceCollection))) {
            //    ObjectWrapperPtr forcesWrapper = motion->getWrapperOfType(typeid(ForceCollection));
            //    ForceCollectionPtr forces = forcesWrapper->get();
            //    std::vector<ObjectWrapperPtr> fxWrappers;
            //    std::vector<ObjectWrapperPtr> fyWrappers;
            //    std::vector<ObjectWrapperPtr> fzWrappers;
            //    for( int i = 0; i < forces->getNumChannels(); i++) {
            //        ForceChannelPtr f = forces->getChannel(i);
            //        ScalarChannelReaderInterfacePtr x(new VectorToScalarAdaptor(f, 0));
            //        ScalarChannelReaderInterfacePtr y(new VectorToScalarAdaptor(f, 1));
            //        ScalarChannelReaderInterfacePtr z(new VectorToScalarAdaptor(f, 2));

            //        core::ObjectWrapperPtr wrapperX = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
            //        core::ObjectWrapperPtr wrapperY = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
            //        core::ObjectWrapperPtr wrapperZ = core::ObjectWrapper::create<ScalarChannelReaderInterface>();

            //        wrapperX->set(x);
            //        wrapperY->set(y);
            //        wrapperZ->set(z);

            //        static int number = 0;
            //        // hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
            //        std::string suffix = boost::lexical_cast<std::string>(number++);
            //        wrapperX->setName("FX_" + suffix);
            //        wrapperX->setSource("FX_" + suffix);
            //        wrapperY->setName("FY_" + suffix);
            //        wrapperY->setSource("FY_" + suffix);
            //        wrapperZ->setName("FZ_" + suffix);
            //        wrapperZ->setSource("FZ_" + suffix);

            //        fxWrappers.push_back(wrapperX);
            //        fyWrappers.push_back(wrapperY);
            //        fzWrappers.push_back(wrapperZ);
            //    }
            //    QTreeWidgetItem* fX = new QTreeWidgetItem();
            //    fX->setText(0, "Motion Forces - X");
            //    //item2Helper[fX] = TreeItemHelperPtr(new MultiserieHelper(fxWrappers));
            //    testItem->addChild(fX);

            //    QTreeWidgetItem* fY = new QTreeWidgetItem();
            //    fY->setText(0, "Motion Forces - Y");
            //    //item2Helper[fY] = TreeItemHelperPtr(new MultiserieHelper(fyWrappers));
            //    testItem->addChild(fY);

            //    QTreeWidgetItem* fZ = new QTreeWidgetItem();
            //    fZ->setText(0, "Motion Forces - Z");
            //    //item2Helper[fZ] = TreeItemHelperPtr(new MultiserieHelper(fzWrappers));
            //    testItem->addChild(fZ);
            //}


        }

    }

    return rootItem;
}

QTreeWidgetItem* TreeBuilder::createEMGBranch( MotionPtr motion, const QString& rootName, const QIcon& itemIcon )
{
    QTreeWidgetItem* emgItem = new QTreeWidgetItem();
    emgItem->setText(0, rootName);
    std::vector<core::ObjectWrapperPtr> emgs = motion->getWrappers(typeid(EMGChannel));
    int count = emgs.size();			
    for (int i = 0; i < count; i++) {	
        EMGChannelPtr c = emgs[i]->get();	
        if (c) {
            QTreeWidgetItem* channelItem = new ChartItemHelper(emgs[i]);
            channelItem->setIcon(0, itemIcon);	
            channelItem->setText(0, c->getName().c_str());			
            emgItem->addChild(channelItem);			
        }
    }

    return emgItem;
}

QTreeWidgetItem*  TreeBuilder::createGRFBranch( MotionPtr motion, const QString& rootName, const QIcon& itemIcon )
{
    std::vector<core::ObjectWrapperPtr> grfCollections = motion->getWrappers(typeid(GRFCollection));
    QTreeWidgetItem* grfItem = new TreeWrappedItemHelper(grfCollections[0]);
    grfItem->setText(0, rootName);
    //grfItem->setIcon(0, icon3D);
    std::vector<core::ObjectWrapperPtr> grfs = motion->getWrappers(typeid(GRFChannel));
    int count = grfs.size();			
    for (int i = 0; i < count; i++) {
        GRFChannelPtr c = grfs[i]->get();
        if (c) {
            QTreeWidgetItem* channelItem = new NewVector3ItemHelper(grfs[i]);	
            channelItem->setIcon(0, itemIcon);						
            channelItem->setText(0, c->getName().c_str());			
            grfItem->addChild(channelItem);			
        }
    }

    return grfItem;
}

QTreeWidgetItem* TreeBuilder::createVideoBranch( MotionPtr motion, const QString& rootName, const QIcon& itemIcon )
{
    QTreeWidgetItem* videoItem = new QTreeWidgetItem();
    videoItem->setText(0, rootName);
    std::vector<core::ObjectWrapperPtr> videos = motion->getWrappers(typeid(VideoChannel));
    int count = videos.size();			
    for (int i = 0; i < count; i++) {							
        QTreeWidgetItem* channelItem = new TreeWrappedItemHelper(videos[i]);	

        channelItem->setIcon(0, itemIcon);						
        channelItem->setText(0, videos[i]->getName().c_str());			
        videoItem->addChild(channelItem);
    }

    return videoItem;
}

QTreeWidgetItem* TreeBuilder::createJointsBranch( MotionPtr motion, const QString& rootName, const QIcon& itemIcon )
{
   // QTreeWidgetItem* skeletonItem = new QTreeWidgetItem();
    QTreeWidgetItem* skeletonItem = new JointsItemHelper(motion);
    skeletonItem->setIcon(0, itemIcon);
    skeletonItem->setText(0, rootName);
    return skeletonItem;
}

QTreeWidgetItem* TreeBuilder::createMarkersBranch( MotionPtr motion, const QString& rootName, const QIcon& itemIcon )
{
    core::ObjectWrapperPtr markerCollection = motion->getWrapperOfType(typeid(MarkerCollection));
    QTreeWidgetItem* markersItem = new TreeWrappedItemHelper(markerCollection);
    markersItem->setIcon(0, itemIcon);
    markersItem->setText(0, rootName);		

    MarkerCollectionPtr m = markerCollection->get(); 
    tryAddVectorToTree<MarkerChannel>(motion, m, "Marker collection", &itemIcon, markersItem, false);
    return markersItem;
}