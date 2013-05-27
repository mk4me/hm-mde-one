#include "hmmLibPCH.h"
#include "FilterCommand.h"
#include <corelib/HierarchyItem.h>

using namespace core;

SimpleFilterCommand::SimpleFilterCommand( SubjectHierarchyFilterPtr dataFilter ) :
    dataFilter(dataFilter)
{
}

core::IHierarchyItemPtr SimpleFilterCommand::getFilteredTree( core::IHierarchyItemConstPtr root )
{
    core::HierarchyItemPtr hi(new core::HierarchyItem(root->getName(), root->getIcon()));
    return hi;
}

void SimpleFilterCommand::reset()
{
    
}

QWidget* SimpleFilterCommand::getConfigurationWidget()
{
    return nullptr;
}
