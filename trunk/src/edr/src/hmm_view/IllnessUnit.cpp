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

    /*wrpX1 = extractWrappersFromVector(leftPrevF->filterSessions (sessions), 0); 
    wrpX2 = extractWrappersFromVector(rightPrevF->filterSessions(sessions), 0);
    wrpX3 = extractWrappersFromVector(leftPostF->filterSessions (sessions), 0); 
    wrpX4 = extractWrappersFromVector(rightPostF->filterSessions(sessions), 0);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("LHipForces - prev", wrpX1, root)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("RHipForces - prev", wrpX2, root)->setColorPolicy(MultiserieHelper::GreenRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("LHipForces - post", wrpX3, root)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("RHipForces - post", wrpX4, root)->setColorPolicy(MultiserieHelper::GreenRandom);*/

    DataFilterPtr leftPrevP =  createCustomV3Filter<PowerCollectionPtr>(managerNULL, false, "LHipPower");
    DataFilterPtr rightPrevP = createCustomV3Filter<PowerCollectionPtr>(managerNULL, false, "RHipPower");
    DataFilterPtr leftPostP =  createCustomV3Filter<PowerCollectionPtr>(managerNULL, true, "LHipPower");
    DataFilterPtr rightPostP = createCustomV3Filter<PowerCollectionPtr>(managerNULL, true, "RHipPower");
    createVectorEntry(root, sessions, "Moc w stawie biodrowym", leftPrevP, rightPrevP, leftPostP, rightPostP, 0);

    //wrpX1 = extractWrappersFromVector(leftPrevP->filterSessions (sessions), 0); 
    //wrpX2 = extractWrappersFromVector(rightPrevP->filterSessions(sessions), 0);
    //wrpX3 = extractWrappersFromVector(leftPostP->filterSessions (sessions), 0); 
    //wrpX4 = extractWrappersFromVector(rightPostP->filterSessions(sessions), 0);
    //createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("LHipPowers - prev", wrpX1, root)->setColorPolicy(MultiserieHelper::RedRandom);
    //createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("RHipPowers - prev", wrpX2, root)->setColorPolicy(MultiserieHelper::GreenRandom);
    //createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("LHipPowers - post", wrpX3, root)->setColorPolicy(MultiserieHelper::RedRandom);
    //createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("RHipPowers - post", wrpX4, root)->setColorPolicy(MultiserieHelper::GreenRandom);
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

        /*std::vector<core::ObjectWrapperPtr> wrpX1 = extractWrappersFromEMG(lprev->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX2 = extractWrappersFromEMG(rprev->filterSessions(sessions));
        std::vector<core::ObjectWrapperPtr> wrpX3 = extractWrappersFromEMG(lpost->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX4 = extractWrappersFromEMG(rpost->filterSessions(sessions));
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien prosty uda L - prev", wrpX1, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien prosty uda R - prev", wrpX2, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien prosty uda L - post", wrpX3, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien prosty uda R - post", wrpX4, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);*/

    }

    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L5");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R13");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L5");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R13");
        createEMGEntry(root, sessions, "miesien posladkowy sredni", lprev, rprev, lpost, rpost);
        /*std::vector<core::ObjectWrapperPtr> wrpX1 = extractWrappersFromEMG(lprev->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX2 = extractWrappersFromEMG(rprev->filterSessions(sessions));
        std::vector<core::ObjectWrapperPtr> wrpX3 = extractWrappersFromEMG(lpost->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX4 = extractWrappersFromEMG(rpost->filterSessions(sessions));
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien posladkowy sredni L - prev", wrpX1, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien posladkowy sredni R - prev", wrpX2, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien posladkowy sredni L - post", wrpX3, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien posladkowy sredni R - post", wrpX4, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);*/
    }

    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L7");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R15");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L7");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R15");
        createEMGEntry(root, sessions, "Gluteus Maximus", lprev, rprev, lpost, rpost);

        /*std::vector<core::ObjectWrapperPtr> wrpX1 = extractWrappersFromEMG(lprev->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX2 = extractWrappersFromEMG(rprev->filterSessions(sessions));
        std::vector<core::ObjectWrapperPtr> wrpX3 = extractWrappersFromEMG(lpost->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX4 = extractWrappersFromEMG(rpost->filterSessions(sessions));
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("Gluteus Maximus L - prev", wrpX1, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("Gluteus Maximus R - prev", wrpX2, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("Gluteus Maximus L - post", wrpX3, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("Gluteus Maximus R - post", wrpX4, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);*/

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

    /*std::vector<core::ObjectWrapperPtr> wrpLXPrev = extractWrappersFromVector(leftPrevA->filterSessions (sessions), 0); 
    std::vector<core::ObjectWrapperPtr> wrpRXPrev = extractWrappersFromVector(rightPrevA->filterSessions(sessions), 0);
    std::vector<core::ObjectWrapperPtr> wrpLXPost = extractWrappersFromVector(leftPostA->filterSessions (sessions), 0); 
    std::vector<core::ObjectWrapperPtr> wrpRXPost = extractWrappersFromVector(rightPostA->filterSessions(sessions), 0);

    std::vector<core::ObjectWrapperPtr> wrpLYPrev = extractWrappersFromVector(leftPrevA->filterSessions (sessions), 1); 
    std::vector<core::ObjectWrapperPtr> wrpRYPrev = extractWrappersFromVector(rightPrevA->filterSessions(sessions), 1);
    std::vector<core::ObjectWrapperPtr> wrpLYPost = extractWrappersFromVector(leftPostA->filterSessions (sessions), 1); 
    std::vector<core::ObjectWrapperPtr> wrpRYPost = extractWrappersFromVector(rightPostA->filterSessions(sessions), 1);

    std::vector<core::ObjectWrapperPtr> wrpLZPrev = extractWrappersFromVector(leftPrevA->filterSessions (sessions), 2); 
    std::vector<core::ObjectWrapperPtr> wrpRZPrev = extractWrappersFromVector(rightPrevA->filterSessions(sessions), 2);
    std::vector<core::ObjectWrapperPtr> wrpLZPost = extractWrappersFromVector(leftPostA->filterSessions (sessions), 2); 
    std::vector<core::ObjectWrapperPtr> wrpRZPost = extractWrappersFromVector(rightPostA->filterSessions(sessions), 2);

    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L kat zgiecia stawu biodrowego w plaszczyznie strzalkowej - prev", wrpLXPrev, root)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R kat zgiecia stawu biodrowego w plaszczyznie strzalkowej - prev", wrpRXPrev, root)->setColorPolicy(MultiserieHelper::GreenRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L kat zgiecia stawu biodrowego w plaszczyznie strzalkowej - post", wrpLXPost, root)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R kat zgiecia stawu biodrowego w plaszczyznie strzalkowej - post", wrpRXPost, root)->setColorPolicy(MultiserieHelper::GreenRandom);

    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L kat odwodzenia stawu biodrowego w plaszczyznie czolowej - prev", wrpLYPrev, root)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R kat odwodzenia stawu biodrowego w plaszczyznie czolowej - prev", wrpRYPrev, root)->setColorPolicy(MultiserieHelper::GreenRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L kat odwodzenia stawu biodrowego w plaszczyznie czolowej - post", wrpLYPost, root)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R kat odwodzenia stawu biodrowego w plaszczyznie czolowej - post", wrpRYPost, root)->setColorPolicy(MultiserieHelper::GreenRandom);

    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L kat rotacji biodra - prev", wrpLZPrev, root)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R kat rotacji biodra - prev", wrpRZPrev, root)->setColorPolicy(MultiserieHelper::GreenRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L kat rotacji biodra - post", wrpLZPost, root)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R kat rotacji biodra - post", wrpRZPost, root)->setColorPolicy(MultiserieHelper::GreenRandom);*/
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
    MultiserieHelper* hipMomentPrev = createArgsItem< MultiserieHelper >("Pre",  wrpX12,hipMoment);
    MultiserieHelper* hipMomentPost = createArgsItem< MultiserieHelper >("Post", wrpX34,hipMoment);

    hipMomentPrev->setColorPolicy(MultiserieHelper::HalfRedHalfGreen);
    hipMomentPost->setColorPolicy(MultiserieHelper::HalfRedHalfGreen);

    createArgsItem< MultiserieHelper >("Left",  wrpX1, hipMomentPrev)->setColorPolicy(MultiserieHelper::Red);
    createArgsItem< MultiserieHelper >("Right", wrpX2, hipMomentPrev)->setColorPolicy(MultiserieHelper::Green);
    createArgsItem< MultiserieHelper >("Left",  wrpX3, hipMomentPost)->setColorPolicy(MultiserieHelper::Red);
    createArgsItem< MultiserieHelper >("Right", wrpX4, hipMomentPost)->setColorPolicy(MultiserieHelper::Green);
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
        /*std::vector<core::ObjectWrapperPtr> wrpX1 = extractWrappersFromEMG(lprev->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX2 = extractWrappersFromEMG(rprev->filterSessions(sessions));
        std::vector<core::ObjectWrapperPtr> wrpX3 = extractWrappersFromEMG(lpost->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX4 = extractWrappersFromEMG(rpost->filterSessions(sessions));
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien prosty uda L - prev", wrpX1, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien prosty uda R - prev", wrpX2, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien prosty uda L - post", wrpX3, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien prosty uda R - post", wrpX4, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);*/

    }
    
    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L2");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R10");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L2");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R10");
        createEMGEntry(root, sessions, "Byc moze miesien piszczelowy sredni", lprev, rprev, lpost, rpost);
        /*std::vector<core::ObjectWrapperPtr> wrpX1 = extractWrappersFromEMG(lprev->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX2 = extractWrappersFromEMG(rprev->filterSessions(sessions));
        std::vector<core::ObjectWrapperPtr> wrpX3 = extractWrappersFromEMG(lpost->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX4 = extractWrappersFromEMG(rpost->filterSessions(sessions));
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("Byc moze miesien piszczelowy sredni L - prev", wrpX1, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("Byc moze miesien piszczelowy sredni R - prev", wrpX2, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("Byc moze miesien piszczelowy sredni L - post", wrpX3, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("Byc moze miesien piszczelowy sredni R - post", wrpX4, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom)*/;

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

        /*std::vector<core::ObjectWrapperPtr> wrpX1 = extractWrappersFromEMG(lprev->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX2 = extractWrappersFromEMG(rprev->filterSessions(sessions));
        std::vector<core::ObjectWrapperPtr> wrpX3 = extractWrappersFromEMG(lpost->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX4 = extractWrappersFromEMG(rpost->filterSessions(sessions));
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien prosty uda L - prev", wrpX1, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien prosty uda R - prev", wrpX2, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien prosty uda L - post", wrpX3, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien prosty uda R - post", wrpX4, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);*/

    }

    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L5");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R13");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L5");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R13");
        createEMGEntry(root, sessions, "miesien posladkowy sredni", lprev, rprev, lpost, rpost);
        /*std::vector<core::ObjectWrapperPtr> wrpX1 = extractWrappersFromEMG(lprev->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX2 = extractWrappersFromEMG(rprev->filterSessions(sessions));
        std::vector<core::ObjectWrapperPtr> wrpX3 = extractWrappersFromEMG(lpost->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX4 = extractWrappersFromEMG(rpost->filterSessions(sessions));
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien posladkowy sredni L - prev", wrpX1, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien posladkowy sredni R - prev", wrpX2, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien posladkowy sredni L - post", wrpX3, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("miesien posladkowy sredni R - post", wrpX4, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);*/
    }

    {
        DataFilterPtr lprev = createCustomEMGFilter(nullptr, false, "L7");
        DataFilterPtr rprev = createCustomEMGFilter(nullptr, false, "R15");
        DataFilterPtr lpost = createCustomEMGFilter(nullptr, true, "L7");
        DataFilterPtr rpost = createCustomEMGFilter(nullptr, true, "R15");
        createEMGEntry(root, sessions, "Gluteus Maximus", lprev, rprev, lpost, rpost);
        /*std::vector<core::ObjectWrapperPtr> wrpX1 = extractWrappersFromEMG(lprev->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX2 = extractWrappersFromEMG(rprev->filterSessions(sessions));
        std::vector<core::ObjectWrapperPtr> wrpX3 = extractWrappersFromEMG(lpost->filterSessions(sessions)); 
        std::vector<core::ObjectWrapperPtr> wrpX4 = extractWrappersFromEMG(rpost->filterSessions(sessions));
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("Gluteus Maximus L - prev", wrpX1, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("Gluteus Maximus R - prev", wrpX2, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("Gluteus Maximus L - post", wrpX3, emgItem)->setColorPolicy(MultiserieHelper::RedRandom);
        createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("Gluteus Maximus R - post", wrpX4, emgItem)->setColorPolicy(MultiserieHelper::GreenRandom);*/

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
    //std::vector<core::ObjectWrapperPtr> wrpX1 = extractWrappersFromVector(leftPrev->filterSessions (sessions), 0); 
    //std::vector<core::ObjectWrapperPtr> wrpX2 = extractWrappersFromVector(rightPrev->filterSessions(sessions), 0);
    //std::vector<core::ObjectWrapperPtr> wrpX3 = extractWrappersFromVector(leftPost->filterSessions (sessions), 0); 
    //std::vector<core::ObjectWrapperPtr> wrpX4 = extractWrappersFromVector(rightPost->filterSessions(sessions), 0);
    //createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L Moment w talii (p. strzalkowa) - prev", wrpX1, momentsItem)->setColorPolicy(MultiserieHelper::RedRandom);
    //createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R Moment w talii (p. strzalkowa) - prev", wrpX2, momentsItem)->setColorPolicy(MultiserieHelper::GreenRandom);
    //createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L Moment w talii (p. strzalkowa) - post", wrpX3, momentsItem)->setColorPolicy(MultiserieHelper::RedRandom);
    //createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R Moment w talii (p. strzalkowa) - post", wrpX4, momentsItem)->setColorPolicy(MultiserieHelper::GreenRandom);
    
    /*wrpX1 = extractWrappersFromVector(leftPrev->filterSessions (sessions), 1); 
    wrpX2 = extractWrappersFromVector(rightPrev->filterSessions(sessions), 1);
    wrpX3 = extractWrappersFromVector(leftPost->filterSessions (sessions), 1); 
    wrpX4 = extractWrappersFromVector(rightPost->filterSessions(sessions), 1);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L Moment w talii (p. czolowa) - prev", wrpX1, momentsItem)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R Moment w talii (p. czolowa) - prev", wrpX2, momentsItem)->setColorPolicy(MultiserieHelper::GreenRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L Moment w talii (p. czolowa) - post", wrpX3, momentsItem)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R Moment w talii (p. czolowa) - post", wrpX4, momentsItem)->setColorPolicy(MultiserieHelper::GreenRandom);*/
    
  /*  
    QTreeWidgetItem* forcesItem = new QTreeWidgetItem();
    forcesItem->setText(0, QObject::tr("Forces"));
    rootItem->addChild(forcesItem);*/

    DataFilterPtr leftPrevF =  createCustomV3Filter<ForceCollectionPtr>(managerNULL, false, "LWaistForce");
    DataFilterPtr rightPrevF = createCustomV3Filter<ForceCollectionPtr>(managerNULL, false, "RWaistForce");
    DataFilterPtr leftPostF =  createCustomV3Filter<ForceCollectionPtr>(managerNULL, true,  "LWaistForce");
    DataFilterPtr rightPostF = createCustomV3Filter<ForceCollectionPtr>(managerNULL, true,  "RWaistForce");
    createVectorEntry(root, sessions, "Sila w talii (p. strzalkowa)", leftPrev, rightPrev, leftPost, rightPost, 0);
    createVectorEntry(root, sessions, "Sila w talii (p. czolowa)", leftPrev, rightPrev, leftPost, rightPost, 1);

    /*wrpX1 = extractWrappersFromVector(leftPrevF->filterSessions (sessions), 0); 
    wrpX2 = extractWrappersFromVector(rightPrevF->filterSessions(sessions), 0);
    wrpX3 = extractWrappersFromVector(leftPostF->filterSessions (sessions), 0); 
    wrpX4 = extractWrappersFromVector(rightPostF->filterSessions(sessions), 0);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L Sila w talii (p. strzalkowa) - prev", wrpX1, forcesItem)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R Sila w talii (p. strzalkowa) - prev", wrpX2, forcesItem)->setColorPolicy(MultiserieHelper::GreenRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L Sila w talii (p. strzalkowa) - post", wrpX3, forcesItem)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R Sila w talii (p. strzalkowa) - post", wrpX4, forcesItem)->setColorPolicy(MultiserieHelper::GreenRandom);

    wrpX1 = extractWrappersFromVector(leftPrevF->filterSessions (sessions), 1); 
    wrpX2 = extractWrappersFromVector(rightPrevF->filterSessions(sessions), 1);
    wrpX3 = extractWrappersFromVector(leftPostF->filterSessions (sessions), 1); 
    wrpX4 = extractWrappersFromVector(rightPostF->filterSessions(sessions), 1);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L Sila w talii (p. czolowa) - prev", wrpX1, forcesItem)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R Sila w talii (p. czolowa) - prev", wrpX2, forcesItem)->setColorPolicy(MultiserieHelper::GreenRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L Sila w talii (p. czolowa) - post", wrpX3, forcesItem)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R Sila w talii (p. czolowa) - post", wrpX4, forcesItem)->setColorPolicy(MultiserieHelper::GreenRandom);*/
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

    /*std::vector<core::ObjectWrapperPtr> wrpLXPrev = extractWrappersFromVector(leftPrevA->filterSessions (sessions), 0); 
    std::vector<core::ObjectWrapperPtr> wrpRXPrev = extractWrappersFromVector(rightPrevA->filterSessions(sessions), 0);
    std::vector<core::ObjectWrapperPtr> wrpLXPost = extractWrappersFromVector(leftPostA->filterSessions (sessions), 0); 
    std::vector<core::ObjectWrapperPtr> wrpRXPost = extractWrappersFromVector(rightPostA->filterSessions(sessions), 0);

    std::vector<core::ObjectWrapperPtr> wrpLYPrev = extractWrappersFromVector(leftPrevA->filterSessions (sessions), 1); 
    std::vector<core::ObjectWrapperPtr> wrpRYPrev = extractWrappersFromVector(rightPrevA->filterSessions(sessions), 1);
    std::vector<core::ObjectWrapperPtr> wrpLYPost = extractWrappersFromVector(leftPostA->filterSessions (sessions), 1); 
    std::vector<core::ObjectWrapperPtr> wrpRYPost = extractWrappersFromVector(rightPostA->filterSessions(sessions), 1);


    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L kat kregoslupa (p. strzalkowa) - prev", wrpLXPrev, root)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R kat kregoslupa (p. strzalkowa) - prev", wrpRXPrev, root)->setColorPolicy(MultiserieHelper::GreenRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L kat kregoslupa (p. strzalkowa) - post", wrpLXPost, root)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R kat kregoslupa (p. strzalkowa) - post", wrpRXPost, root)->setColorPolicy(MultiserieHelper::GreenRandom);

    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L kat kregoslupa (p. czolowa) - prev", wrpLYPrev, root)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R kat kregoslupa (p. czolowa) - prev", wrpRYPrev, root)->setColorPolicy(MultiserieHelper::GreenRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("L kat kregoslupa (p. czolowa) - post", wrpLYPost, root)->setColorPolicy(MultiserieHelper::RedRandom);
    createArgsItem< HmmTreePolicyItem<MultiserieHelper> >("R kat kregoslupa (p. czolowa) - post", wrpRYPost, root)->setColorPolicy(MultiserieHelper::GreenRandom);*/

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


    
