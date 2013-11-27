#include "HierarchyTest.h"
#include <corelib/IHierarchyItem.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include "coreui/HierarchyTreeModel.h"
#include <QtGui/QTreeView>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( HierarchyTest );


DEFINE_WRAPPER(int, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

void HierarchyTest::setUp()
{
}

void HierarchyTest::testHierarchyHelper()
{
    core::IHierarchyItemPtr rootItem = utils::make_shared<core::HierarchyItem>("ROOT", "ROOT_DESC", QIcon());

    CPPUNIT_ASSERT(rootItem->getParent() == core::IHierarchyItemPtr());
    CPPUNIT_ASSERT(rootItem->getNumChildren() == 0);
    CPPUNIT_ASSERT(rootItem->getIndex() == 0);
    CPPUNIT_ASSERT(rootItem->getName() == QString("ROOT"));\
    CPPUNIT_ASSERT(rootItem->getDescription() == QString("ROOT_DESC"));

    core::IHierarchyItemPtr child1 = utils::make_shared<core::HierarchyItem>("CHILD1", "CHILD1_DESC", QIcon());
    rootItem->appendChild(child1);
    CPPUNIT_ASSERT(rootItem->getNumChildren() == 1);

    utils::ObjectWrapperPtr wrapper = utils::ObjectWrapper::create<int>();
    wrapper->set(utils::make_shared<int>(20));
    core::IHierarchyItemPtr child2 = utils::make_shared<core::HierarchyDataItem>(wrapper, QIcon(), "CHILD2", "CHILD2_DESC");
    rootItem->appendChild(child2);


    
    CPPUNIT_ASSERT(rootItem->getNumChildren() == 2);
    CPPUNIT_ASSERT(child1->getIndex() == 0);
    CPPUNIT_ASSERT(child2->getIndex() == 1);
    CPPUNIT_ASSERT(child1->getParent() == rootItem && child2->getParent() == rootItem);
    CPPUNIT_ASSERT(rootItem->getChild(1) == child2);

    rootItem->removeChild(child1);
    CPPUNIT_ASSERT(rootItem->getNumChildren() == 1);
    CPPUNIT_ASSERT(child1->getIndex() == 0);
    CPPUNIT_ASSERT(child2->getIndex() == 0);
    CPPUNIT_ASSERT(child1->getParent() == core::IHierarchyItemPtr());
    CPPUNIT_ASSERT(rootItem->getChild(0) == child2);

    child1->setParent(rootItem);
    CPPUNIT_ASSERT(rootItem->getNumChildren() == 2);
    CPPUNIT_ASSERT(child1->getIndex() == 1);
    CPPUNIT_ASSERT(child2->getIndex() == 0);
    CPPUNIT_ASSERT(child1->getParent() == rootItem && child2->getParent() == rootItem);

    core::IHierarchyItemPtr nonChild = utils::make_shared<core::HierarchyItem>("NON_CHILD", "NON_CHILD_DESC", QIcon());
    CPPUNIT_ASSERT_THROW(rootItem->removeChild(nonChild), std::runtime_error);
    CPPUNIT_ASSERT_THROW(rootItem->getChild(-1), std::out_of_range);
    CPPUNIT_ASSERT_THROW(rootItem->getChild(2), std::out_of_range);


    core::IHierarchyItemPtr rootItem2 = utils::make_shared<core::HierarchyItem>("ROOT2", "ROOT2_DESC", QIcon());
    child2->setParent(rootItem2);
    CPPUNIT_ASSERT(rootItem->getNumChildren() == 1 &&  rootItem2->getNumChildren() == 1);
    CPPUNIT_ASSERT(child1->getIndex() == 0);
    CPPUNIT_ASSERT(child2->getIndex() == 0);
    CPPUNIT_ASSERT(child1->getParent() == rootItem && child2->getParent() == rootItem2);

    core::IHierarchyDataItemPtr dataItem = utils::dynamic_pointer_cast<core::IHierarchyDataItem>(child2);
    std::list<core::HierarchyHelperPtr> helpers = dataItem->getHelpers();
    utils::ObjectWrapperConstPtr data = dataItem->getData();
    CPPUNIT_ASSERT(data == wrapper);
    CPPUNIT_ASSERT(helpers.size() == 1);
    CPPUNIT_ASSERT(utils::dynamic_pointer_cast<core::WrappedItemHelper>(*helpers.begin()));
    
}

void HierarchyTest::testShallowCopy()
{
    core::IHierarchyItemPtr rootItem = createSimpleTree();
    core::IHierarchyItemConstPtr child1 = rootItem->getChild(0);
    core::HierarchyDataItemConstPtr child11 = utils::dynamic_pointer_cast<const core::HierarchyDataItem>(child1->getChild(0));
    core::HierarchyDataItemConstPtr child2 = utils::dynamic_pointer_cast<const core::HierarchyDataItem>(rootItem->getChild(1));
    
    core::IHierarchyItemPtr rootCopy = rootItem->shallowCopy();
    CPPUNIT_ASSERT(rootCopy->getNumChildren() == 2);
    CPPUNIT_ASSERT(rootCopy != rootItem);
    CPPUNIT_ASSERT(rootCopy->getChildIndex(child1) == -1);
    CPPUNIT_ASSERT(rootItem->getChildIndex(child1) == 0);
    CPPUNIT_ASSERT(rootCopy->getChild(1) != rootItem->getChild(1));
    core::HierarchyItemConstPtr child1Copy = utils::dynamic_pointer_cast<const core::HierarchyItem>(rootCopy->getChild(0));
    CPPUNIT_ASSERT(child1Copy);
    core::HierarchyDataItemConstPtr child11Copy = utils::dynamic_pointer_cast<const core::HierarchyDataItem>(child1Copy->getChild(0));
    CPPUNIT_ASSERT(child11Copy);
    core::HierarchyDataItemConstPtr child2Copy = utils::dynamic_pointer_cast<const core::HierarchyDataItem>(rootCopy->getChild(1));
    CPPUNIT_ASSERT(child2Copy);
    
    CPPUNIT_ASSERT(child1Copy != child1);
    CPPUNIT_ASSERT(child11Copy != child11);
    CPPUNIT_ASSERT(child2Copy != child2);

    CPPUNIT_ASSERT(!child11->getData());
    CPPUNIT_ASSERT(!child11Copy->getData());

    CPPUNIT_ASSERT(*child11Copy->getHelpers().begin() == *child11->getHelpers().begin());
    CPPUNIT_ASSERT(*child2Copy->getHelpers().begin() == *child2->getHelpers().begin());
    CPPUNIT_ASSERT(child2Copy->getData() == child2->getData());
}

void HierarchyTest::testModel()
{
    coreUI::HierarchyTreeModel model;
    core::IHierarchyItemPtr rootItem1 = createSimpleTree();
    core::IHierarchyItemPtr rootItem2 = createSimpleTree();
    core::IHierarchyItemPtr rootItem3 = createSimpleTree();

    model.addRootItem(rootItem1);
    model.addRootItem(rootItem2);

    CPPUNIT_ASSERT(model.getNumChildren() == 2);
    CPPUNIT_ASSERT(!rootItem1->getParent());
    CPPUNIT_ASSERT_THROW(model.addRootItem(rootItem1), std::runtime_error);
    CPPUNIT_ASSERT_THROW(model.removeRootItem(rootItem3), std::runtime_error);
    model.addRootItem(rootItem3);
    CPPUNIT_ASSERT(model.getNumChildren() == 3);
    CPPUNIT_ASSERT(model.getChild(0) == rootItem1);
    CPPUNIT_ASSERT(model.getChild(2) == rootItem3);
    model.removeRootItem(rootItem3);
    CPPUNIT_ASSERT(model.getNumChildren() == 2);

    

    core::IMemoryDataManagerHierarchy::HierarchyChange change;
    change.value = rootItem3;
    change.modification = core::IDataManagerReader::ADD_OBJECT;
    model.applyChange(change);
    CPPUNIT_ASSERT(model.getNumChildren() == 3);
    CPPUNIT_ASSERT(model.getChild(2) == rootItem3);

    core::IMemoryDataManagerHierarchy::HierarchyChange change1;
    core::IMemoryDataManagerHierarchy::HierarchyChange change2;
    core::IMemoryDataManagerHierarchy::HierarchyChange change3;

    change1.value = createSimpleTree();
    change1.modification = core::IDataManagerReader::ADD_OBJECT;

    change2.value = rootItem2;
    change2.modification = core::IDataManagerReader::UPDATE_OBJECT;

    change3.value = rootItem1;
    change3.modification = core::IDataManagerReader::REMOVE_OBJECT;
    
    core::IMemoryDataManagerHierarchy::HierarchyChangeList l;
    l.push_back(change1);
    l.push_back(change2);
    l.push_back(change3);

    model.applyChanges(l);
    CPPUNIT_ASSERT(model.getNumChildren() == 3);
    CPPUNIT_ASSERT(model.getChild(2) == change1.value);

    CPPUNIT_ASSERT_THROW(model.applyChanges(l), std::runtime_error);

    CPPUNIT_ASSERT(model.getNumChildren() == 3);
    CPPUNIT_ASSERT(model.getChild(2) == change1.value);
}

void HierarchyTest::testModelView()
{
    core::IHierarchyItemPtr rootItem1 = createSimpleTree();
    core::IHierarchyItemPtr rootItem2 = createSimpleTree();
    core::IHierarchyItemPtr rootItem3 = createSimpleTree();
    coreUI::HierarchyTreeModel model;
    model.addRootItem(rootItem1);
    model.addRootItem(rootItem2);
    model.addRootItem(rootItem3);
    QTreeView view;
    view.setModel(&model);
}

core::IHierarchyItemPtr HierarchyTest::createSimpleTree()
{
    core::IHierarchyItemPtr rootItem = utils::make_shared<core::HierarchyItem>("ROOT", "ROOT_DESC", QIcon());
    core::IHierarchyItemPtr child1 = utils::make_shared<core::HierarchyItem>("CHILD1", "CHILD1_DESC", QIcon());
    rootItem->appendChild(child1);

    utils::ObjectWrapperPtr wrapper11 = utils::ObjectWrapper::create<int>();
    wrapper11->set(utils::make_shared<int>(11));
    core::HierarchyHelperPtr helper = utils::make_shared<core::WrappedItemHelper>(wrapper11);
    core::HierarchyDataItemPtr child11 = utils::make_shared<core::HierarchyDataItem>(QIcon(), "CHILD11", "CHILD11_DESC", helper);
    child1->appendChild(child11);

    utils::ObjectWrapperPtr wrapper = utils::ObjectWrapper::create<int>();
    wrapper->set(utils::make_shared<int>(2));
    core::HierarchyDataItemPtr child2 = utils::make_shared<core::HierarchyDataItem>(wrapper, QIcon(), "CHILD2", "CHILD2_DESC");
    rootItem->appendChild(child2);

    return rootItem;
}
