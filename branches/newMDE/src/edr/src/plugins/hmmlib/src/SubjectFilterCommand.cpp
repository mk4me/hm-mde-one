#include "hmmLibPCH.h"
#include "SubjectFilterCommand.h"
#include <corelib/HierarchyItem.h>
#include <corelib/IHierarchyDataItem.h>

using namespace core;


AbstractFilterCommand::AbstractFilterCommand( const QString& name, const QIcon& icon ) :
    name(name),
    icon(icon)
{

}

core::IHierarchyItemPtr AbstractFilterCommand::getFilteredTree( core::IHierarchyItemConstPtr root )
{
    core::HierarchyItemPtr hi(new core::HierarchyItem(getName(), root->getIcon()));
    traverse(root, hi);
    return hi;
}

void AbstractFilterCommand::traverse(core::IHierarchyItemConstPtr source, core::HierarchyItemPtr parent) 
{
    if (checkItem(source)) {
        parent->appendChild(source);
    }

    auto count = source->getNumChildren();
    for (auto i = 0; i < count; ++i) {
        traverse(source->getChild(i), parent);
    }
}

SimpleFilterCommand::SimpleFilterCommand( const std::set<core::TypeInfo>& typeinfos, const QString& name, const QIcon& icon ) :
    AbstractFilterCommand(name, icon),
    typeinfos(typeinfos)
{
}

SimpleFilterCommand::SimpleFilterCommand( const utils::TypeInfo& typeinfo, const QString& name, const QIcon& icon ) :
    AbstractFilterCommand(name, icon)
{
    typeinfos.insert(typeinfo);
}

bool SimpleFilterCommand::checkItem( core::IHierarchyItemConstPtr item )
{
    core::IHierarchyDataItemConstPtr dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
    if (dataItem && dataItem->getData() ) {
        auto info = dataItem->getData()->getTypeInfo();
        if (typeinfos.find(info) != typeinfos.end()) {
            return true; 
        }
    }

    return false;
}




