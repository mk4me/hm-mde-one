#include "hmmPCH.h"
#include "IllnessUnit.h"

template<class TreeItem>
TreeItem* createItem(const QString& name, QTreeWidgetItem* parent = nullptr) 
{
    TreeItem* item = new TreeItem();
    item->setText(0, name);
    if (parent) {
        parent->addChild(item);
    }
    return dynamic_cast<TreeItem*>(item);
}

template<class TreeItem, class Args>
TreeItem* createArgsItem(const QString& name, const Args& args, QTreeWidgetItem* parent = nullptr) 
{
    TreeItem* item = new TreeItem(args);
    item->setText(0, name);
    if (parent) {
        parent->addChild(item);
    }
    return dynamic_cast<TreeItem*>(item);
}

std::vector<core::ObjectWrapperPtr> Endo::extractWrappersFromVector( const std::vector<SessionPtr>& sessions, int scalarIndex = 0 )
{
    std::vector<core::ObjectWrapperPtr> xWrappers;
    BOOST_FOREACH(SessionPtr session, sessions)
    {   
        BOOST_FOREACH(MotionPtr motion, session->getMotions()) {
            if (motion->hasObjectOfType(typeid(VectorChannel))) {
                core::ObjectWrapperPtr wrapper = motion->getWrapperOfType(typeid(VectorChannel));
                VectorChannelPtr moment = wrapper->get();
                ScalarChannelReaderInterfacePtr x(new VectorToScalarAdaptor(moment, scalarIndex));

                core::ObjectWrapperPtr wrapperX = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
                wrapperX->set(x);
                static int number = 0;
                std::string suffix = boost::lexical_cast<std::string>(number++);
                wrapperX->setName("Wrapper_" + suffix);
                wrapperX->setSource("Wrapper_" + suffix);
                xWrappers.push_back(wrapperX);
            }
        }
    }

    return xWrappers;
}

void Endo::createKineticBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions )
{
    core::IMemoryDataManager* managerNULL = nullptr;
    
    DataFilterPtr leftPrev =  createCustomV3Filter<MomentCollectionPtr>(managerNULL, false, "LHipMoment");
    DataFilterPtr rightPrev = createCustomV3Filter<MomentCollectionPtr>(managerNULL, false, "RHipMoment");
    DataFilterPtr leftPost =  createCustomV3Filter<MomentCollectionPtr>(managerNULL, true, "LHipMoment");
    DataFilterPtr rightPost = createCustomV3Filter<MomentCollectionPtr>(managerNULL, true, "RHipMoment");
    createVectorEntry(root, sessions, "Moment w stawie biodrowym", leftPrev, rightPrev, leftPost, rightPost, 0);

    
    DataFilterPtr leftPrevF =  createCustomV3Filter<ForceCollectionPtr>(managerNULL, false, "LHipForce");
    DataFilterPtr rightPrevF = createCustomV3Filter<ForceCollectionPtr>(managerNULL, false, "RHipForce");
    DataFilterPtr leftPostF =  createCustomV3Filter<ForceCollectionPtr>(managerNULL, true, "LHipForce");
    DataFilterPtr rightPostF = createCustomV3Filter<ForceCollectionPtr>(managerNULL, true, "RHipForce");
    createVectorEntry(root,sessions, "Sila w stawie biodrowym", leftPrevF, rightPrevF, leftPostF, rightPostF, 0);

    DataFilterPtr leftPrevP =  createCustomV3Filter<PowerCollectionPtr>(managerNULL, false, "LHipPower");
    DataFilterPtr rightPrevP = createCustomV3Filter<PowerCollectionPtr>(managerNULL, false, "RHipPower");
    DataFilterPtr leftPostP =  createCustomV3Filter<PowerCollectionPtr>(managerNULL, true, "LHipPower");
    DataFilterPtr rightPostP = createCustomV3Filter<PowerCollectionPtr>(managerNULL, true, "RHipPower");
    createVectorEntry(root, sessions, "Moc w stawie biodrowym", leftPrevP, rightPrevP, leftPostP, rightPostP, 0);
}

template<class CollectionPtr>
DataFilterPtr Endo::createCustomV3Filter( core::IMemoryDataManager* manager, bool post, const std::string& name )
{
    std::vector<DataFilterPtr> filters;
    filters.push_back(DataFilterPtr(new SessionNumberFilter(manager, post ? 3 : 1, post ? 4 : 2)));
    filters.push_back(DataFilterPtr(new TypeFilter(manager, typeid(CollectionPtr::element_type))));
    filters.push_back(DataFilterPtr(new ChannelExtractorFilter<CollectionPtr>(manager)));
    filters.push_back(DataFilterPtr(new NameFilter(manager, name)));
    return DataFilterPtr(new CustomFilter(manager, filters));
}

void Endo::createEMGBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions )
{
    QTreeWidgetItem* emgItem = root;
    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L1");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R9");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L1");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R9");
        createEMGEntry(root, sessions, "miesien prosty uda", lprev, rprev, lpost, rpost);
    }

    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L5");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R13");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L5");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R13");
        createEMGEntry(root, sessions, "miesien posladkowy sredni", lprev, rprev, lpost, rpost);
    }

    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L7");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R15");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L7");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R15");
        createEMGEntry(root, sessions, "Gluteus Maximus", lprev, rprev, lpost, rpost);
    }
}

DataFilterPtr Endo::createCustomEMGFilter( core::IMemoryDataManager* manager, bool post, const std::string& name )
{
    std::vector<DataFilterPtr> filters;
    filters.push_back(DataFilterPtr(new SessionNumberFilter(manager, post ? 3 : 1, post ? 4 : 2)));
    filters.push_back(DataFilterPtr(new TypeFilter(manager, typeid(EMGChannel))));
    filters.push_back(DataFilterPtr(new NameFilter(manager, name)));
    return DataFilterPtr(new CustomFilter(manager, filters));
}

std::vector<core::ObjectWrapperPtr> Endo::extractWrappersFromEMG(const std::vector<SessionPtr>& sessions)
{
    std::vector<core::ObjectWrapperPtr> ret;
    BOOST_FOREACH(SessionPtr session, sessions) {
        BOOST_FOREACH(MotionPtr motion, session->getMotions()) {
            std::vector<core::ObjectWrapperPtr> emgs = motion->getWrappers(typeid(EMGChannel));
            ret.insert(ret.end(), emgs.begin(), emgs.end());
        }
    }
    return ret;
}


void Endo::createAngleBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions )
{
    core::IMemoryDataManager* managerNULL = nullptr;

    DataFilterPtr leftPrevA =  createCustomV3Filter<AngleCollectionPtr>(managerNULL, false, "LHipAngles");
    DataFilterPtr rightPrevA = createCustomV3Filter<AngleCollectionPtr>(managerNULL, false, "RHipAngles");
    DataFilterPtr leftPostA =  createCustomV3Filter<AngleCollectionPtr>(managerNULL, true, "LHipAngles");
    DataFilterPtr rightPostA = createCustomV3Filter<AngleCollectionPtr>(managerNULL, true, "RHipAngles");
    createVectorEntry(root, sessions, "kat zgiecia stawu biodrowego w plaszczyznie strzalkowej", leftPrevA, rightPrevA, leftPostA, rightPostA, 0);
    createVectorEntry(root, sessions, "kat odwodzenia stawu biodrowego w plaszczyznie czolowej", leftPrevA, rightPrevA, leftPostA, rightPostA, 1);
    createVectorEntry(root, sessions, "kat rotacji biodra", leftPrevA, rightPrevA, leftPostA, rightPostA, 2);
}

QTreeWidgetItem* Endo::createTreeBranch( const QString& rootItemName, const std::vector<SessionConstPtr>& sessions )
{
    
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
    this->createEMGBranch(root, sessions);
    this->createKineticBranch(root, sessions);
    this->createAngleBranch(root, sessions);
    return root;
}

void Endo::createEMGEntry(QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions, const QString& name,
    DataFilterPtr leftPrev, DataFilterPtr rightPrev, DataFilterPtr leftPost, DataFilterPtr rightPost)
{
    std::vector<core::ObjectWrapperPtr> wrpX1 = extractWrappersFromEMG(leftPrev->filterSessions (sessions)); 
    std::vector<core::ObjectWrapperPtr> wrpX2 = extractWrappersFromEMG(rightPrev->filterSessions(sessions));
    std::vector<core::ObjectWrapperPtr> wrpX3 = extractWrappersFromEMG(leftPost->filterSessions (sessions)); 
    std::vector<core::ObjectWrapperPtr> wrpX4 = extractWrappersFromEMG(rightPost->filterSessions(sessions));

    std::vector<core::ObjectWrapperPtr> wrpX12 = wrpX1; wrpX12.insert(wrpX12.end(), wrpX2.begin(), wrpX2.end());
    std::vector<core::ObjectWrapperPtr> wrpX34 = wrpX3; wrpX34.insert(wrpX34.end(), wrpX4.begin(), wrpX4.end());

    ChildrenVisualizers* hipMoment = createItem<ChildrenVisualizers> (name, root);

    MultiserieHelper* hipMomentPrev = createArgsItem< MultiserieHelper >("Pre",  wrpX12,hipMoment);
    MultiserieHelper* hipMomentPost = createArgsItem< MultiserieHelper >("Post", wrpX34,hipMoment);

    hipMomentPrev->setColorPolicy(MultiserieHelper::HalfRedHalfGreen);
    hipMomentPost->setColorPolicy(MultiserieHelper::HalfRedHalfGreen);

    createArgsItem<MultiserieHelper>("Left",  wrpX1, hipMomentPrev)->setColorPolicy(MultiserieHelper::Red);
    createArgsItem<MultiserieHelper>("Right", wrpX2, hipMomentPrev)->setColorPolicy(MultiserieHelper::Green);
    createArgsItem<MultiserieHelper>("Left",  wrpX3, hipMomentPost)->setColorPolicy(MultiserieHelper::Red);
    createArgsItem<MultiserieHelper>("Right", wrpX4, hipMomentPost)->setColorPolicy(MultiserieHelper::Green);
}

void Endo::createVectorEntry(QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions, const QString& name,
    DataFilterPtr leftPrev, DataFilterPtr rightPrev, DataFilterPtr leftPost, DataFilterPtr rightPost, int index)
{
    std::vector<core::ObjectWrapperPtr> wrpX1 = extractWrappersFromVector(leftPrev->filterSessions (sessions), index); 
    std::vector<core::ObjectWrapperPtr> wrpX2 = extractWrappersFromVector(rightPrev->filterSessions(sessions), index);
    std::vector<core::ObjectWrapperPtr> wrpX3 = extractWrappersFromVector(leftPost->filterSessions (sessions), index); 
    std::vector<core::ObjectWrapperPtr> wrpX4 = extractWrappersFromVector(rightPost->filterSessions(sessions), index);

    std::vector<core::ObjectWrapperPtr> wrpX12 = wrpX1; wrpX12.insert(wrpX12.end(), wrpX2.begin(), wrpX2.end());
    std::vector<core::ObjectWrapperPtr> wrpX34 = wrpX3; wrpX34.insert(wrpX34.end(), wrpX4.begin(), wrpX4.end());

    ChildrenVisualizers* hipMoment = createItem<ChildrenVisualizers> (name, root);
    hipMoment->setPolicy(ChildrenVisualizers::Vertical);

    NewMultiserieHelper* hipMomentPrev = createArgsItem< NewMultiserieHelper >(QObject::tr("Pre"),  wrpX12, hipMoment);
    
    //NewMultiserieHelper* hipMomentPost = createArgsItem< NewMultiserieHelper >("Post", wrpX34,hipMoment);
    MultiserieHelper* hipMomentPost = createArgsItem< MultiserieHelper >(QObject::tr("Post"), wrpX12, hipMoment);

    hipMomentPrev->setColorPolicy(NewMultiserieHelper::HalfRedHalfGreen);
    hipMomentPrev->setTitle(QString("Multichart - %1").arg(name));

    hipMomentPost->setColorPolicy(MultiserieHelper::HalfRedHalfGreen);

    NewMultiserieHelper* lHelper = createArgsItem<NewMultiserieHelper >(QObject::tr("Left"),  wrpX1, hipMomentPrev);
    NewMultiserieHelper* rHelper = createArgsItem<NewMultiserieHelper >(QObject::tr("Right"), wrpX2, hipMomentPrev);
    lHelper->setColorPolicy(NewMultiserieHelper::Red);
    rHelper->setColorPolicy(NewMultiserieHelper::Green);
    lHelper->setTitle(QString("Multichart - %1 (%2)").arg(name).arg("Left"));
    rHelper->setTitle(QString("Multichart - %1 (%2)").arg(name).arg("Right"));

    //createArgsItem<NewMultiserieHelper >("Left",  wrpX3, hipMomentPost)->setColorPolicy(NewMultiserieHelper::Red);
    //createArgsItem<NewMultiserieHelper >("Right", wrpX4, hipMomentPost)->setColorPolicy(NewMultiserieHelper::Green);
    createArgsItem<MultiserieHelper >("Left",  wrpX1, hipMomentPost)->setColorPolicy(MultiserieHelper::Red);
    createArgsItem<MultiserieHelper >("Right", wrpX2, hipMomentPost)->setColorPolicy(MultiserieHelper::Green);
}

QTreeWidgetItem* Stroke::createTreeBranch( const QString& rootItemName, const std::vector<SessionConstPtr>& sessions )
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L1");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R9");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L1");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R9");
        createEMGEntry(root, sessions, "miesien prosty uda", lprev, rprev, lpost, rpost);
    }
    
    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L2");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R10");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L2");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R10");
        createEMGEntry(root, sessions, "Byc moze miesien piszczelowy sredni", lprev, rprev, lpost, rpost);

    }

    return root;
}

void Spine::createEMGBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions )
{
    //1. miÍsieÒ prostownik grzbietu (najd≥uøszy) (na wys. T12)  (UB > L4 i R12)
    //2. miÍsieÒ kapturowy (powyøej C3) (UB > L1 i R9)
    //3. miÍsieÒ poúladkowy wielki (LB > L7 i R15)

    QTreeWidgetItem* emgItem = root;
    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L1");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R9");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L1");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R9");
        createEMGEntry(root, sessions, "miesien prosty uda", lprev, rprev, lpost, rpost);
    }

    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L5");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R13");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L5");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R13");
        createEMGEntry(root, sessions, "miesien posladkowy sredni", lprev, rprev, lpost, rpost);
    }

    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L7");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R15");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L7");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R15");
        createEMGEntry(root, sessions, "Gluteus Maximus", lprev, rprev, lpost, rpost);

    }
}

void Spine::createKineticBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions )
{
    // Force
    // 4. Si≥a w talii w p≥aszczyünie strza≥kowej X w osi poprzecznej
    // 5. si≥a w talii w p≥aszczyünie czo≥owej Y (w osi strza≥kowej)

    // 8. moment si≥y w talii w p≥aszczyünie strza≥kowej X
    // 9. moment si≥y w talii w p≥aszczyünie czo≥owej Y
    core::IMemoryDataManager* managerNULL = nullptr;

    DataFilterPtr leftPrev =  createCustomV3Filter<MomentCollectionPtr>(managerNULL, false, "LWaistMoment");
    DataFilterPtr rightPrev = createCustomV3Filter<MomentCollectionPtr>(managerNULL, false, "RWaistMoment");
    DataFilterPtr leftPost =  createCustomV3Filter<MomentCollectionPtr>(managerNULL, true,  "LWaistMoment");
    DataFilterPtr rightPost = createCustomV3Filter<MomentCollectionPtr>(managerNULL, true,  "RWaistMoment");
    createVectorEntry(root, sessions, "Moment w talii (p. strzalkowa)", leftPrev, rightPrev, leftPost, rightPost, 0);
    createVectorEntry(root, sessions, "Moment w talii (p. czolowa)", leftPrev, rightPrev, leftPost, rightPost, 1);

    DataFilterPtr leftPrevF =  createCustomV3Filter<ForceCollectionPtr>(managerNULL, false, "LWaistForce");
    DataFilterPtr rightPrevF = createCustomV3Filter<ForceCollectionPtr>(managerNULL, false, "RWaistForce");
    DataFilterPtr leftPostF =  createCustomV3Filter<ForceCollectionPtr>(managerNULL, true,  "LWaistForce");
    DataFilterPtr rightPostF = createCustomV3Filter<ForceCollectionPtr>(managerNULL, true,  "RWaistForce");
    createVectorEntry(root, sessions, "Sila w talii (p. strzalkowa)", leftPrev, rightPrev, leftPost, rightPost, 0);
    createVectorEntry(root, sessions, "Sila w talii (p. czolowa)", leftPrev, rightPrev, leftPost, rightPost, 1);
}

void Spine::createAngleBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions )
{
    //
    // 6. kπt krÍgos≥upa w p≥aszczyünie strza≥kowej X
    // 7. kπt krÍgos≥upa w p≥aszczyünie czo≥owej Y
    core::IMemoryDataManager* managerNULL = nullptr;

    DataFilterPtr leftPrevA =  createCustomV3Filter<AngleCollectionPtr>(managerNULL, false, "LSpineAngles");
    DataFilterPtr rightPrevA = createCustomV3Filter<AngleCollectionPtr>(managerNULL, false, "RSpineAngles");
    DataFilterPtr leftPostA =  createCustomV3Filter<AngleCollectionPtr>(managerNULL, true, "LSpineAngles");
    DataFilterPtr rightPostA = createCustomV3Filter<AngleCollectionPtr>(managerNULL, true, "RSpineAngles");
    createVectorEntry(root, sessions, "kat kregoslupa (p. strzalkowa)", leftPrevA, rightPrevA, leftPostA, rightPostA, 0);
    createVectorEntry(root, sessions, "kat kregoslupa (p. czolowa)", leftPrevA, rightPrevA, leftPostA, rightPostA, 1);
 }

QTreeWidgetItem* Spine::createTreeBranch( const QString& rootItemName, const std::vector<SessionConstPtr>& sessions )
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
    this->createEMGBranch(root, sessions);
    this->createKineticBranch(root, sessions);
    this->createAngleBranch(root, sessions);
    
    return root;
}


    
