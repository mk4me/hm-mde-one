#include "CorePCH.h"
#include "ActionsGroupManager.h"

#include <QtCore/QObject>
#include <QtGui/QWidget>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <stdexcept>

using namespace core;

ActionsGroup::ActionsGroup(const QString & name) : name_(name)
{

}

ActionsGroup::~ActionsGroup()
{

}

void ActionsGroup::addAction(QObject * action)
{
    auto it = allActions.find(action);

    if(it != allActions.end()){
        throw std::runtime_error("Action already added to group");
    }

    int pos = allActions.size();

    allActions[action] = pos;

    if(QMenu * menu = qobject_cast<QMenu*>(action)){
        menus[pos] = menu;
    }else if(QAction * realAction = qobject_cast<QAction*>(action)){
        actions[pos] = realAction;
    }else if(QWidget * widget = qobject_cast<QWidget*>(action)){
        widgets[pos] = widget;
    }else {
        objects[pos] = action;
    }
}

const QString & ActionsGroup::name() const
{
    return name_;
}

void ActionsGroup::getAllObjects(std::map<int, QObject *> & allObjects) const
{
    for(auto it = allActions.begin(); it != allActions.end(); ++it){
        allObjects.insert( std::map<int, QObject *>::value_type(it->second, it->first) );
    }
}

void ActionsGroup::getWidgets(std::map<int, QWidget *> & widgets) const
{
    widgets.insert(this->widgets.begin(), this->widgets.end());
}

void ActionsGroup::getActions(std::map<int, QAction *> & actions) const
{
    actions.insert(this->actions.begin(), this->actions.end());
}

void ActionsGroup::getMenus(std::map<int, QMenu *> & menus) const
{
    menus.insert(this->menus.begin(), this->menus.end());
}

void ActionsGroup::getObjects(std::map<int, QObject *> & objects) const
{
    objects.insert(this->objects.begin(), this->objects.end());
}



ActionsGroupManager::ActionsGroupManager()
{

}

ActionsGroupManager::~ActionsGroupManager()
{

}

ActionsGroupManager::GroupID ActionsGroupManager::createGroup(const QString & groupName)
{
    auto it = nameToPos.find(groupName);

    if(it != nameToPos.end()){
        throw std::runtime_error("Group with given name already registered");
    }

    GroupID ret = groups.size();

    groups.push_back(ActionsGroup(groupName));

    nameToPos[groupName] = ret;

    return ret;
}

void ActionsGroupManager::addGroupAction(GroupID groupID, QObject * action)
{
    groups[groupID].addAction(action);
}

ActionsGroupManager::size_type ActionsGroupManager::size() const
{
    return groups.size();
}

bool ActionsGroupManager::empty() const
{
    return groups.empty();
}

ActionsGroupManager::const_iterator ActionsGroupManager::begin() const
{
    return groups.begin();
}

ActionsGroupManager::const_iterator ActionsGroupManager::end() const
{
    return groups.end();
}

ActionsGroupManager::const_reverse_iterator ActionsGroupManager::rbegin() const
{
    return groups.rbegin();
}

ActionsGroupManager::const_reverse_iterator ActionsGroupManager::rend() const
{
    return groups.rend();
}
