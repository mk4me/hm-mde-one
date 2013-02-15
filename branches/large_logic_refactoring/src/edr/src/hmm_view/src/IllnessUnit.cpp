#include "hmmPCH.h"
#include "IllnessUnit.h"
#include "TreeItemHelper.h"
#include <plugins/subject/SubjectDataFilters.h>

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
    TreeHelperPtr helper(new typename TreeHelperPtr::element_type(args));
    HmmTreeItem* item = new HmmTreeItem(helper);
    item->setItemAndHelperText(name);
    if (parent) {
        parent->addChild(item);
    }
    return std::make_pair(item, helper);
}

std::vector<core::ObjectWrapperConstPtr> Endo::extractWrappersFromVector( const core::ConstObjectsList& sessions, int scalarIndex = 0 )
{
    std::vector<core::ObjectWrapperConstPtr> xWrappers;
    BOOST_FOREACH(core::ObjectWrapperConstPtr session, sessions)
    {
		PluginSubject::SessionConstPtr s = session->get();
		core::ConstObjectsList motions;
		s->getMotions(motions);
		BOOST_FOREACH(core::ObjectWrapperConstPtr motion, motions) {
			PluginSubject::MotionConstPtr m = motion->get();
			if (m->hasObject(typeid(VectorChannel), false)) {
				core::ConstObjectsList wrappers;
				m->getObjects(wrappers, typeid(VectorChannel), false);
				core::ObjectWrapperConstPtr wrapper = wrappers.front();
				if(wrapper != nullptr){
					VectorChannelConstPtr moment;
					bool ok = wrapper->tryGet(moment);
					if(ok && moment != nullptr){
						ScalarChannelReaderInterfacePtr x(new VectorToScalarAdaptor(moment, scalarIndex));

						core::ObjectWrapperPtr wrapperX = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
						wrapperX->set(x);
						static int number = 0;
						std::string suffix = boost::lexical_cast<std::string>(number++);

						(*wrapperX)["core/name"] = "Wrapper_" + suffix;
						(*wrapperX)["core/source"] = "IlnessUnit";
						xWrappers.push_back(wrapperX);
					}
				}
			}
		}
    }

    return xWrappers;
}

void Endo::createKineticBranch( QTreeWidgetItem* root, const core::ConstObjectsList& sessions )
{
    SubjectHierarchyFilterPtr leftPrev =  createCustomV3Filter<MomentCollection>(false, "LHipMoment");
    SubjectHierarchyFilterPtr rightPrev = createCustomV3Filter<MomentCollection>(false, "RHipMoment");
    SubjectHierarchyFilterPtr leftPost =  createCustomV3Filter<MomentCollection>(true, "LHipMoment");
    SubjectHierarchyFilterPtr rightPost = createCustomV3Filter<MomentCollection>(true, "RHipMoment");
    createVectorEntry(root, sessions, "Moment w stawie biodrowym", leftPrev, rightPrev, leftPost, rightPost, 0);


    SubjectHierarchyFilterPtr leftPrevF =  createCustomV3Filter<ForceCollection>(false, "LHipForce");
    SubjectHierarchyFilterPtr rightPrevF = createCustomV3Filter<ForceCollection>(false, "RHipForce");
    SubjectHierarchyFilterPtr leftPostF =  createCustomV3Filter<ForceCollection>(true, "LHipForce");
    SubjectHierarchyFilterPtr rightPostF = createCustomV3Filter<ForceCollection>(true, "RHipForce");
    createVectorEntry(root,sessions, "Sila w stawie biodrowym", leftPrevF, rightPrevF, leftPostF, rightPostF, 0);

    SubjectHierarchyFilterPtr leftPrevP =  createCustomV3Filter<PowerCollection>(false, "LHipPower");
    SubjectHierarchyFilterPtr rightPrevP = createCustomV3Filter<PowerCollection>(false, "RHipPower");
    SubjectHierarchyFilterPtr leftPostP =  createCustomV3Filter<PowerCollection>(true, "LHipPower");
    SubjectHierarchyFilterPtr rightPostP = createCustomV3Filter<PowerCollection>(true, "RHipPower");
    createVectorEntry(root, sessions, "Moc w stawie biodrowym", leftPrevP, rightPrevP, leftPostP, rightPostP, 0);
}

template<class Collection>
SubjectHierarchyFilterPtr Endo::createCustomV3Filter(bool post, const std::string& name )
{
    SubjectHierarchyCompoundFilter::Filters filters;
    filters.push_back(SubjectHierarchyFilterPtr(new SessionNumberFilter(post ? 3 : 1, post ? 4 : 2)));
    filters.push_back(SubjectHierarchyFilterPtr(new SubjectHierarchyTypeFilter(typeid(Collection))));
    filters.push_back(SubjectHierarchyFilterPtr(new ChannelExtractorFilter<Collection>()));
    filters.push_back(SubjectHierarchyFilterPtr(new NameFilter(name)));
    return SubjectHierarchyFilterPtr(new SubjectHierarchyCompoundFilter(filters));
}

void Endo::createEMGBranch( QTreeWidgetItem* root, const core::ConstObjectsList& sessions )
{
    {
        SubjectHierarchyFilterPtr lprev = createCustomEMGFilter(false, "L1");
        SubjectHierarchyFilterPtr rprev = createCustomEMGFilter(false, "R9");
        SubjectHierarchyFilterPtr lpost = createCustomEMGFilter(true, "L1");
        SubjectHierarchyFilterPtr rpost = createCustomEMGFilter(true, "R9");
        createEMGEntry(root, sessions, "miesien prosty uda", lprev, rprev, lpost, rpost);
    }

    {
        SubjectHierarchyFilterPtr lprev = createCustomEMGFilter(false, "L5");
        SubjectHierarchyFilterPtr rprev = createCustomEMGFilter(false, "R13");
        SubjectHierarchyFilterPtr lpost = createCustomEMGFilter(true, "L5");
        SubjectHierarchyFilterPtr rpost = createCustomEMGFilter(true, "R13");
        createEMGEntry(root, sessions, "miesien posladkowy sredni", lprev, rprev, lpost, rpost);
    }

    {
        SubjectHierarchyFilterPtr lprev = createCustomEMGFilter(false, "L7");
        SubjectHierarchyFilterPtr rprev = createCustomEMGFilter(false, "R15");
        SubjectHierarchyFilterPtr lpost = createCustomEMGFilter(true, "L7");
        SubjectHierarchyFilterPtr rpost = createCustomEMGFilter(true, "R15");
        createEMGEntry(root, sessions, "Gluteus Maximus", lprev, rprev, lpost, rpost);
    }
}

SubjectHierarchyFilterPtr Endo::createCustomEMGFilter(bool post, const std::string& name )
{
    SubjectHierarchyCompoundFilter::Filters filters;
    filters.push_back(SubjectHierarchyFilterPtr(new SessionNumberFilter(post ? 3 : 1, post ? 4 : 2)));
    filters.push_back(SubjectHierarchyFilterPtr(new SubjectHierarchyTypeFilter(typeid(EMGChannel))));
    filters.push_back(SubjectHierarchyFilterPtr(new NameFilter(name)));
    return SubjectHierarchyFilterPtr(new SubjectHierarchyCompoundFilter(filters));
}

std::vector<core::ObjectWrapperConstPtr> Endo::extractWrappersFromEMG(const core::ConstObjectsList& sessions)
{
    std::vector<core::ObjectWrapperConstPtr> ret;
    BOOST_FOREACH(core::ObjectWrapperConstPtr session, sessions) {
		PluginSubject::SessionConstPtr s = session->get();
		core::ConstObjectsList motions;
		s->getMotions(motions);
		BOOST_FOREACH(core::ObjectWrapperConstPtr motion, motions) {
			PluginSubject::MotionConstPtr m = motion->get();
			core::ConstObjectsList emgs;
			m->getObjects(emgs, typeid(EMGChannel), false);
			ret.insert(ret.end(), emgs.begin(), emgs.end());
		}
    }
    return ret;
}


void Endo::createAngleBranch( QTreeWidgetItem* root, const core::ConstObjectsList& sessions )
{
    SubjectHierarchyFilterPtr leftPrevA =  createCustomV3Filter<AngleCollection>(false, "LHipAngles");
    SubjectHierarchyFilterPtr rightPrevA = createCustomV3Filter<AngleCollection>(false, "RHipAngles");
    SubjectHierarchyFilterPtr leftPostA =  createCustomV3Filter<AngleCollection>(true, "LHipAngles");
    SubjectHierarchyFilterPtr rightPostA = createCustomV3Filter<AngleCollection>(true, "RHipAngles");
    createVectorEntry(root, sessions, "kat zgiecia stawu biodrowego w plaszczyznie strzalkowej", leftPrevA, rightPrevA, leftPostA, rightPostA, 0);
    createVectorEntry(root, sessions, "kat odwodzenia stawu biodrowego w plaszczyznie czolowej", leftPrevA, rightPrevA, leftPostA, rightPostA, 1);
    createVectorEntry(root, sessions, "kat rotacji biodra", leftPrevA, rightPrevA, leftPostA, rightPostA, 2);
}

QTreeWidgetItem* Endo::createTreeBranch( const QString& rootItemName, const core::ConstObjectsList& sessions )
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
    this->createEMGBranch(root, sessions);
    this->createKineticBranch(root, sessions);
    this->createAngleBranch(root, sessions);
    return root;
}

void Endo::createEMGEntry(QTreeWidgetItem* root, const core::ConstObjectsList& sessions, const QString& name,
    SubjectHierarchyFilterPtr leftPrev, SubjectHierarchyFilterPtr rightPrev, SubjectHierarchyFilterPtr leftPost, SubjectHierarchyFilterPtr rightPost)
{
	core::ConstObjectsList filteredSessions;
	leftPrev->filterSessions(sessions, filteredSessions);
    std::vector<core::ObjectWrapperConstPtr> wrpX1 = extractWrappersFromEMG(filteredSessions);
	core::ConstObjectsList().swap(filteredSessions);
    rightPrev->filterSessions(sessions, filteredSessions);
    std::vector<core::ObjectWrapperConstPtr> wrpX2 = extractWrappersFromEMG(filteredSessions);
	core::ConstObjectsList().swap(filteredSessions);
	leftPost->filterSessions(sessions, filteredSessions);
    std::vector<core::ObjectWrapperConstPtr> wrpX3 = extractWrappersFromEMG(filteredSessions);
	core::ConstObjectsList().swap(filteredSessions);
	rightPost->filterSessions(sessions, filteredSessions);    
    std::vector<core::ObjectWrapperConstPtr> wrpX4 = extractWrappersFromEMG(filteredSessions);

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

void Endo::createVectorEntry(QTreeWidgetItem* root, const core::ConstObjectsList& sessions, const QString& name,
    SubjectHierarchyFilterPtr leftPrev, SubjectHierarchyFilterPtr rightPrev, SubjectHierarchyFilterPtr leftPost, SubjectHierarchyFilterPtr rightPost, int index)
{
	core::ConstObjectsList filteredSessions;
	leftPrev->filterSessions(sessions, filteredSessions);
	std::vector<core::ObjectWrapperConstPtr> wrpX1 = extractWrappersFromVector(filteredSessions);
	core::ConstObjectsList().swap(filteredSessions);
	rightPrev->filterSessions(sessions, filteredSessions);
	std::vector<core::ObjectWrapperConstPtr> wrpX2 = extractWrappersFromVector(filteredSessions);
	core::ConstObjectsList().swap(filteredSessions);
	leftPost->filterSessions(sessions, filteredSessions);
	std::vector<core::ObjectWrapperConstPtr> wrpX3 = extractWrappersFromVector(filteredSessions);
	core::ConstObjectsList().swap(filteredSessions);
	rightPost->filterSessions(sessions, filteredSessions);    
	std::vector<core::ObjectWrapperConstPtr> wrpX4 = extractWrappersFromVector(filteredSessions);

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

QTreeWidgetItem* Stroke::createTreeBranch( const QString& rootItemName, const core::ConstObjectsList& sessions )
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
    {
        SubjectHierarchyFilterPtr lprev = createCustomEMGFilter(false, "L1");
        SubjectHierarchyFilterPtr rprev = createCustomEMGFilter(false, "R9");
        SubjectHierarchyFilterPtr lpost = createCustomEMGFilter(true, "L1");
        SubjectHierarchyFilterPtr rpost = createCustomEMGFilter(true, "R9");
        createEMGEntry(root, sessions, "miesien prosty uda", lprev, rprev, lpost, rpost);
    }

    {
        SubjectHierarchyFilterPtr lprev = createCustomEMGFilter(false, "L2");
        SubjectHierarchyFilterPtr rprev = createCustomEMGFilter(false, "R10");
        SubjectHierarchyFilterPtr lpost = createCustomEMGFilter(true, "L2");
        SubjectHierarchyFilterPtr rpost = createCustomEMGFilter(true, "R10");
        createEMGEntry(root, sessions, "miesien piszczelowy sredni (?)", lprev, rprev, lpost, rpost);

    }

    return root;
}

void Spine::createEMGBranch( QTreeWidgetItem* root, const core::ConstObjectsList& sessions )
{
    //1. mięsień prostownik grzbietu (najdłuższy) (na wys. T12)  (UB > L4 i R12)
    //2. mięsień kapturowy (powyżej C3) (UB > L1 i R9)
    //3. mięsień pośladkowy wielki (LB > L7 i R15)

    {
        SubjectHierarchyFilterPtr lprev = createCustomEMGFilter(false, "L1");
        SubjectHierarchyFilterPtr rprev = createCustomEMGFilter(false, "R9");
        SubjectHierarchyFilterPtr lpost = createCustomEMGFilter(true, "L1");
        SubjectHierarchyFilterPtr rpost = createCustomEMGFilter(true, "R9");
        createEMGEntry(root, sessions, "miesien prosty uda", lprev, rprev, lpost, rpost);
    }

    {
        SubjectHierarchyFilterPtr lprev = createCustomEMGFilter(false, "L5");
        SubjectHierarchyFilterPtr rprev = createCustomEMGFilter(false, "R13");
        SubjectHierarchyFilterPtr lpost = createCustomEMGFilter(true, "L5");
        SubjectHierarchyFilterPtr rpost = createCustomEMGFilter(true, "R13");
        createEMGEntry(root, sessions, "miesien posladkowy sredni", lprev, rprev, lpost, rpost);
    }

    {
        SubjectHierarchyFilterPtr lprev = createCustomEMGFilter(false, "L7");
        SubjectHierarchyFilterPtr rprev = createCustomEMGFilter(false, "R15");
        SubjectHierarchyFilterPtr lpost = createCustomEMGFilter(true, "L7");
        SubjectHierarchyFilterPtr rpost = createCustomEMGFilter(true, "R15");
        createEMGEntry(root, sessions, "Gluteus Maximus", lprev, rprev, lpost, rpost);

    }
}

void Spine::createKineticBranch( QTreeWidgetItem* root, const core::ConstObjectsList& sessions )
{
    // Force
    // 4. Siła w talii w płaszczyźnie strzałkowej X w osi poprzecznej
    // 5. siła w talii w płaszczyźnie czołowej Y (w osi strzałkowej)

    // 8. moment siły w talii w płaszczyźnie strzałkowej X
    // 9. moment siły w talii w płaszczyźnie czołowej Y

    SubjectHierarchyFilterPtr leftPrev =  createCustomV3Filter<MomentCollection>(false, "LWaistMoment");
    SubjectHierarchyFilterPtr rightPrev = createCustomV3Filter<MomentCollection>(false, "RWaistMoment");
    SubjectHierarchyFilterPtr leftPost =  createCustomV3Filter<MomentCollection>(true,  "LWaistMoment");
    SubjectHierarchyFilterPtr rightPost = createCustomV3Filter<MomentCollection>(true,  "RWaistMoment");
    createVectorEntry(root, sessions, "Moment w talii (p. strzalkowa)", leftPrev, rightPrev, leftPost, rightPost, 0);
    createVectorEntry(root, sessions, "Moment w talii (p. czolowa)", leftPrev, rightPrev, leftPost, rightPost, 1);

    SubjectHierarchyFilterPtr leftPrevF =  createCustomV3Filter<ForceCollection>(false, "LWaistForce");
    SubjectHierarchyFilterPtr rightPrevF = createCustomV3Filter<ForceCollection>(false, "RWaistForce");
    SubjectHierarchyFilterPtr leftPostF =  createCustomV3Filter<ForceCollection>(true,  "LWaistForce");
    SubjectHierarchyFilterPtr rightPostF = createCustomV3Filter<ForceCollection>(true,  "RWaistForce");
    createVectorEntry(root, sessions, "Sila w talii (p. strzalkowa)", leftPrev, rightPrev, leftPost, rightPost, 0);
    createVectorEntry(root, sessions, "Sila w talii (p. czolowa)", leftPrev, rightPrev, leftPost, rightPost, 1);
}

void Spine::createAngleBranch( QTreeWidgetItem* root, const core::ConstObjectsList& sessions )
{
    //
    // 6. kąt kręgosłupa w płaszczyźnie strzałkowej X
    // 7. kąt kręgosłupa w płaszczyźnie czołowej Y

    SubjectHierarchyFilterPtr leftPrevA =  createCustomV3Filter<AngleCollection>(false, "LSpineAngles");
    SubjectHierarchyFilterPtr rightPrevA = createCustomV3Filter<AngleCollection>(false, "RSpineAngles");
    SubjectHierarchyFilterPtr leftPostA =  createCustomV3Filter<AngleCollection>(true, "LSpineAngles");
    SubjectHierarchyFilterPtr rightPostA = createCustomV3Filter<AngleCollection>(true, "RSpineAngles");
    createVectorEntry(root, sessions, "kat kregoslupa (p. strzalkowa)", leftPrevA, rightPrevA, leftPostA, rightPostA, 0);
    createVectorEntry(root, sessions, "kat kregoslupa (p. czolowa)", leftPrevA, rightPrevA, leftPostA, rightPostA, 1);
 }

QTreeWidgetItem* Spine::createTreeBranch( const QString& rootItemName, const core::ConstObjectsList& sessions )
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
    this->createEMGBranch(root, sessions);
    this->createKineticBranch(root, sessions);
    this->createAngleBranch(root, sessions);

    return root;
}



