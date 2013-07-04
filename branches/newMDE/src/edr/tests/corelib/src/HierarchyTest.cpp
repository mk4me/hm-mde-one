#include "HierarchyTest.h"
#include <corelib/IHierarchyItem.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>

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
