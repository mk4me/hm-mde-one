#include "hmmPCH.h"
#include "IllnessUnit.h"

using namespace PluginSubject;

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

template<class TreeHelperPtr, class Args>
std::pair<HmmTreeItem*, TreeHelperPtr> createHelperItem(const QString& name, const Args& args, QTreeWidgetItem* parent = nullptr) 
{
    TreeHelperPtr helper(new TreeHelperPtr::element_type(args));
    HmmTreeItem* item = new HmmTreeItem(helper);
    item->setItemAndHelperText(name);
    if (parent) {
        parent->addChild(item);
    }
    return std::make_pair(item, helper);
}

std::vector<core::ObjectWrapperConstPtr> Endo::extractWrappersFromVector( const std::vector<SessionConstPtr>& sessions, int scalarIndex = 0 )
{
    std::vector<core::ObjectWrapperConstPtr> xWrappers;
    BOOST_FOREACH(SessionConstPtr session, sessions)
    {   
        Motions motions;
        session->getMotions(motions);
        BOOST_FOREACH(MotionConstPtr motion, motions) {
            if (motion->hasObjectOfType(typeid(VectorChannel))) {
                core::ObjectWrapperConstPtr wrapper = motion->getWrapperOfType(typeid(VectorChannel));
                VectorChannelConstPtr moment = wrapper->get();
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
    DataFilterPtr leftPrev =  createCustomV3Filter<MomentCollection>(false, "LHipMoment");
    DataFilterPtr rightPrev = createCustomV3Filter<MomentCollection>(false, "RHipMoment");
    DataFilterPtr leftPost =  createCustomV3Filter<MomentCollection>(true, "LHipMoment");
    DataFilterPtr rightPost = createCustomV3Filter<MomentCollection>(true, "RHipMoment");
    createVectorEntry(root, sessions, "Moment w stawie biodrowym", leftPrev, rightPrev, leftPost, rightPost, 0);

    
    DataFilterPtr leftPrevF =  createCustomV3Filter<ForceCollection>(false, "LHipForce");
    DataFilterPtr rightPrevF = createCustomV3Filter<ForceCollection>(false, "RHipForce");
    DataFilterPtr leftPostF =  createCustomV3Filter<ForceCollection>(true, "LHipForce");
    DataFilterPtr rightPostF = createCustomV3Filter<ForceCollection>(true, "RHipForce");
    createVectorEntry(root,sessions, "Sila w stawie biodrowym", leftPrevF, rightPrevF, leftPostF, rightPostF, 0);

    DataFilterPtr leftPrevP =  createCustomV3Filter<PowerCollection>(false, "LHipPower");
    DataFilterPtr rightPrevP = createCustomV3Filter<PowerCollection>(false, "RHipPower");
    DataFilterPtr leftPostP =  createCustomV3Filter<PowerCollection>(true, "LHipPower");
    DataFilterPtr rightPostP = createCustomV3Filter<PowerCollection>(true, "RHipPower");
    createVectorEntry(root, sessions, "Moc w stawie biodrowym", leftPrevP, rightPrevP, leftPostP, rightPostP, 0);
}

template<class Collection>
DataFilterPtr Endo::createCustomV3Filter(bool post, const std::string& name )
{
    std::vector<DataFilterPtr> filters;
    filters.push_back(DataFilterPtr(new SessionNumberFilter(post ? 3 : 1, post ? 4 : 2)));
    filters.push_back(DataFilterPtr(new TypeFilter(typeid(Collection))));
    filters.push_back(DataFilterPtr(new ChannelExtractorFilter<Collection>()));
    filters.push_back(DataFilterPtr(new NameFilter(name)));
    return DataFilterPtr(new CustomFilter(filters));
}

void Endo::createEMGBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions )
{
    {
        DataFilterPtr lprev = createCustomEMGFilter(false, "L1");
        DataFilterPtr rprev = createCustomEMGFilter(false, "R9");
        DataFilterPtr lpost = createCustomEMGFilter(true, "L1");
        DataFilterPtr rpost = createCustomEMGFilter(true, "R9");
        createEMGEntry(root, sessions, "miesien prosty uda", lprev, rprev, lpost, rpost);
    }

    {
        DataFilterPtr lprev = createCustomEMGFilter(false, "L5");
        DataFilterPtr rprev = createCustomEMGFilter(false, "R13");
        DataFilterPtr lpost = createCustomEMGFilter(true, "L5");
        DataFilterPtr rpost = createCustomEMGFilter(true, "R13");
        createEMGEntry(root, sessions, "miesien posladkowy sredni", lprev, rprev, lpost, rpost);
    }

    {
        DataFilterPtr lprev = createCustomEMGFilter(false, "L7");
        DataFilterPtr rprev = createCustomEMGFilter(false, "R15");
        DataFilterPtr lpost = createCustomEMGFilter(true, "L7");
        DataFilterPtr rpost = createCustomEMGFilter(true, "R15");
        createEMGEntry(root, sessions, "Gluteus Maximus", lprev, rprev, lpost, rpost);
    }
}

DataFilterPtr Endo::createCustomEMGFilter(bool post, const std::string& name )
{
    std::vector<DataFilterPtr> filters;
    filters.push_back(DataFilterPtr(new SessionNumberFilter(post ? 3 : 1, post ? 4 : 2)));
    filters.push_back(DataFilterPtr(new TypeFilter(typeid(EMGChannel))));
    filters.push_back(DataFilterPtr(new NameFilter(name)));
    return DataFilterPtr(new CustomFilter(filters));
}

std::vector<core::ObjectWrapperConstPtr> Endo::extractWrappersFromEMG(const std::vector<SessionConstPtr>& sessions)
{
    std::vector<core::ObjectWrapperConstPtr> ret;
    BOOST_FOREACH(SessionConstPtr session, sessions) {
        Motions motions;
        session->getMotions(motions);
        BOOST_FOREACH(MotionConstPtr motion, motions) {
            std::vector<core::ObjectWrapperConstPtr> emgs;
            motion->getWrappers(emgs, typeid(EMGChannel));
            ret.insert(ret.end(), emgs.begin(), emgs.end());
        }
    }
    return ret;
}


void Endo::createAngleBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions )
{
    DataFilterPtr leftPrevA =  createCustomV3Filter<AngleCollection>(false, "LHipAngles");
    DataFilterPtr rightPrevA = createCustomV3Filter<AngleCollection>(false, "RHipAngles");
    DataFilterPtr leftPostA =  createCustomV3Filter<AngleCollection>(true, "LHipAngles");
    DataFilterPtr rightPostA = createCustomV3Filter<AngleCollection>(true, "RHipAngles");
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
    std::vector<SessionPtr> filtered = leftPrev->filterData (sessions);
    std::vector<core::ObjectWrapperConstPtr> wrpX1 = extractWrappersFromEMG(std::vector<SessionConstPtr>(filtered.begin(), filtered.end())); 
    filtered = rightPrev->filterData (sessions);
    std::vector<core::ObjectWrapperConstPtr> wrpX2 = extractWrappersFromEMG(std::vector<SessionConstPtr>(filtered.begin(), filtered.end()));
    filtered = leftPost->filterData (sessions);
    std::vector<core::ObjectWrapperConstPtr> wrpX3 = extractWrappersFromEMG(std::vector<SessionConstPtr>(filtered.begin(), filtered.end())); 
    filtered = rightPost->filterData (sessions);
    std::vector<core::ObjectWrapperConstPtr> wrpX4 = extractWrappersFromEMG(std::vector<SessionConstPtr>(filtered.begin(), filtered.end()));

    std::vector<core::ObjectWrapperConstPtr> wrpX12 = wrpX1; wrpX12.insert(wrpX12.end(), wrpX2.begin(), wrpX2.end());
    std::vector<core::ObjectWrapperConstPtr> wrpX34 = wrpX3; wrpX34.insert(wrpX34.end(), wrpX4.begin(), wrpX4.end());

    ChildrenVisualizers* hipMoment = createItem<ChildrenVisualizers> (name, root);

    auto hipMomentPrev = createHelperItem< NewMultiserieHelperPtr >("Pre",  wrpX12,hipMoment);
    auto hipMomentPost = createHelperItem< NewMultiserieHelperPtr >("Post", wrpX34,hipMoment);

    //hipMomentPrev->setColorPolicy(NewMultiserieHelper::HalfRedHalfGreen);
    //hipMomentPost->setColorPolicy(NewMultiserieHelper::HalfRedHalfGreen);

    auto l1 = createHelperItem<NewMultiserieHelperPtr>("Left",  wrpX1, hipMomentPrev.first);
    auto r1 = createHelperItem<NewMultiserieHelperPtr>("Right", wrpX2, hipMomentPrev.first);
    auto l2 = createHelperItem<NewMultiserieHelperPtr>("Left",  wrpX3, hipMomentPost.first);
    auto r2 = createHelperItem<NewMultiserieHelperPtr>("Right", wrpX4, hipMomentPost.first);

    IMultiserieColorStrategyPtr redStrategy(new RandomBetweenMultiserieColorStrategy(QColor(200, 0, 0), QColor(255, 0,0)));
    IMultiserieColorStrategyPtr greenStrategy(new RandomBetweenMultiserieColorStrategy(QColor(0, 200, 0), QColor(0, 255, 0)));
    l1.second->setColorStrategy(redStrategy);
    r1.second->setColorStrategy(greenStrategy);
    l2.second->setColorStrategy(redStrategy);
    r2.second->setColorStrategy(greenStrategy);

    l1.second->setTitle(QString("Multichart - %1 (%2)").arg(name).arg("Left"));
    r1.second->setTitle(QString("Multichart - %1 (%2)").arg(name).arg("Right"));
    l2.second->setTitle(QString("Multichart - %1 (%2)").arg(name).arg("Left"));
    r2.second->setTitle(QString("Multichart - %1 (%2)").arg(name).arg("Right"));
}

void Endo::createVectorEntry(QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions, const QString& name,
    DataFilterPtr leftPrev, DataFilterPtr rightPrev, DataFilterPtr leftPost, DataFilterPtr rightPost, int index)
{
    std::vector<SessionPtr> filtered = leftPrev->filterData (sessions);
    std::vector<core::ObjectWrapperConstPtr> wrpX1 = extractWrappersFromVector(std::vector<SessionConstPtr>(filtered.begin(), filtered.end()), index); 
    filtered = rightPrev->filterData (sessions);
    std::vector<core::ObjectWrapperConstPtr> wrpX2 = extractWrappersFromVector(std::vector<SessionConstPtr>(filtered.begin(), filtered.end()), index);
    filtered = leftPost->filterData (sessions);
    std::vector<core::ObjectWrapperConstPtr> wrpX3 = extractWrappersFromVector(std::vector<SessionConstPtr>(filtered.begin(), filtered.end()), index); 
    filtered = rightPost->filterData (sessions);
    std::vector<core::ObjectWrapperConstPtr> wrpX4 = extractWrappersFromVector(std::vector<SessionConstPtr>(filtered.begin(), filtered.end()), index);

    std::vector<core::ObjectWrapperConstPtr> wrpX12 = wrpX1; wrpX12.insert(wrpX12.end(), wrpX2.begin(), wrpX2.end());
    std::vector<core::ObjectWrapperConstPtr> wrpX34 = wrpX3; wrpX34.insert(wrpX34.end(), wrpX4.begin(), wrpX4.end());

    ChildrenVisualizers* hipMoment = createItem<ChildrenVisualizers> (name, root);
    hipMoment->setPolicy(ChildrenVisualizers::Vertical);

    auto hipMomentPrev = createHelperItem< NewMultiserieHelperPtr >(QObject::tr("Pre"),  wrpX12, hipMoment);
    auto hipMomentPost = createHelperItem< NewMultiserieHelperPtr >(QObject::tr("Post"), wrpX12, hipMoment);

    //hipMomentPrev->setColorPolicy(NewMultiserieHelper::HalfRedHalfGreen);
    hipMomentPrev.second->setTitle(QString("Multichart - %1").arg(name));

    //hipMomentPost->setColorPolicy(NewMultiserieHelper::HalfRedHalfGreen);

    auto lHelper1 = createHelperItem<NewMultiserieHelperPtr >(QObject::tr("Left"),  wrpX1, hipMomentPrev.first);
    auto rHelper1 = createHelperItem<NewMultiserieHelperPtr >(QObject::tr("Right"), wrpX2, hipMomentPrev.first);
    IMultiserieColorStrategyPtr redStrategy(new RandomBetweenMultiserieColorStrategy(QColor(200, 0, 0), QColor(255, 0,0)));
    IMultiserieColorStrategyPtr greenStrategy(new RandomBetweenMultiserieColorStrategy(QColor(0, 200, 0), QColor(0, 255, 0)));
    lHelper1.second->setColorStrategy(redStrategy);
    rHelper1.second->setColorStrategy(greenStrategy);
    lHelper1.second->setTitle(QString("Multichart - %1 (%2)").arg(name).arg("Left"));
    rHelper1.second->setTitle(QString("Multichart - %1 (%2)").arg(name).arg("Right"));

    auto lHelper2 = createHelperItem<NewMultiserieHelperPtr >("Left",  wrpX1, hipMomentPost.first);
    auto rHelper2 = createHelperItem<NewMultiserieHelperPtr >("Right", wrpX2, hipMomentPost.first);
    lHelper2.second->setColorStrategy(redStrategy);
    rHelper2.second->setColorStrategy(greenStrategy);
    lHelper2.second->setTitle(QString("Multichart - %1 (%2)").arg(name).arg("Left"));
    rHelper2.second->setTitle(QString("Multichart - %1 (%2)").arg(name).arg("Right"));
}

QTreeWidgetItem* Stroke::createTreeBranch( const QString& rootItemName, const std::vector<SessionConstPtr>& sessions )
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
    {
        DataFilterPtr lprev = createCustomEMGFilter(false, "L1");
        DataFilterPtr rprev = createCustomEMGFilter(false, "R9");
        DataFilterPtr lpost = createCustomEMGFilter(true, "L1");
        DataFilterPtr rpost = createCustomEMGFilter(true, "R9");
        createEMGEntry(root, sessions, "miesien prosty uda", lprev, rprev, lpost, rpost);
    }
    
    {
        DataFilterPtr lprev = createCustomEMGFilter(false, "L2");
        DataFilterPtr rprev = createCustomEMGFilter(false, "R10");
        DataFilterPtr lpost = createCustomEMGFilter(true, "L2");
        DataFilterPtr rpost = createCustomEMGFilter(true, "R10");
        createEMGEntry(root, sessions, "miesien piszczelowy sredni (?)", lprev, rprev, lpost, rpost);

    }

    return root;
}

void Spine::createEMGBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions )
{
    //1. miÍsieÒ prostownik grzbietu (najd≥uøszy) (na wys. T12)  (UB > L4 i R12)
    //2. miÍsieÒ kapturowy (powyøej C3) (UB > L1 i R9)
    //3. miÍsieÒ poúladkowy wielki (LB > L7 i R15)

    {
        DataFilterPtr lprev = createCustomEMGFilter(false, "L1");
        DataFilterPtr rprev = createCustomEMGFilter(false, "R9");
        DataFilterPtr lpost = createCustomEMGFilter(true, "L1");
        DataFilterPtr rpost = createCustomEMGFilter(true, "R9");
        createEMGEntry(root, sessions, "miesien prosty uda", lprev, rprev, lpost, rpost);
    }

    {
        DataFilterPtr lprev = createCustomEMGFilter(false, "L5");
        DataFilterPtr rprev = createCustomEMGFilter(false, "R13");
        DataFilterPtr lpost = createCustomEMGFilter(true, "L5");
        DataFilterPtr rpost = createCustomEMGFilter(true, "R13");
        createEMGEntry(root, sessions, "miesien posladkowy sredni", lprev, rprev, lpost, rpost);
    }

    {
        DataFilterPtr lprev = createCustomEMGFilter(false, "L7");
        DataFilterPtr rprev = createCustomEMGFilter(false, "R15");
        DataFilterPtr lpost = createCustomEMGFilter(true, "L7");
        DataFilterPtr rpost = createCustomEMGFilter(true, "R15");
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

    DataFilterPtr leftPrev =  createCustomV3Filter<MomentCollection>(false, "LWaistMoment");
    DataFilterPtr rightPrev = createCustomV3Filter<MomentCollection>(false, "RWaistMoment");
    DataFilterPtr leftPost =  createCustomV3Filter<MomentCollection>(true,  "LWaistMoment");
    DataFilterPtr rightPost = createCustomV3Filter<MomentCollection>(true,  "RWaistMoment");
    createVectorEntry(root, sessions, "Moment w talii (p. strzalkowa)", leftPrev, rightPrev, leftPost, rightPost, 0);
    createVectorEntry(root, sessions, "Moment w talii (p. czolowa)", leftPrev, rightPrev, leftPost, rightPost, 1);

    DataFilterPtr leftPrevF =  createCustomV3Filter<ForceCollection>(false, "LWaistForce");
    DataFilterPtr rightPrevF = createCustomV3Filter<ForceCollection>(false, "RWaistForce");
    DataFilterPtr leftPostF =  createCustomV3Filter<ForceCollection>(true,  "LWaistForce");
    DataFilterPtr rightPostF = createCustomV3Filter<ForceCollection>(true,  "RWaistForce");
    createVectorEntry(root, sessions, "Sila w talii (p. strzalkowa)", leftPrev, rightPrev, leftPost, rightPost, 0);
    createVectorEntry(root, sessions, "Sila w talii (p. czolowa)", leftPrev, rightPrev, leftPost, rightPost, 1);
}

void Spine::createAngleBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions )
{
    //
    // 6. kπt krÍgos≥upa w p≥aszczyünie strza≥kowej X
    // 7. kπt krÍgos≥upa w p≥aszczyünie czo≥owej Y

    DataFilterPtr leftPrevA =  createCustomV3Filter<AngleCollection>(false, "LSpineAngles");
    DataFilterPtr rightPrevA = createCustomV3Filter<AngleCollection>(false, "RSpineAngles");
    DataFilterPtr leftPostA =  createCustomV3Filter<AngleCollection>(true, "LSpineAngles");
    DataFilterPtr rightPostA = createCustomV3Filter<AngleCollection>(true, "RSpineAngles");
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


    
