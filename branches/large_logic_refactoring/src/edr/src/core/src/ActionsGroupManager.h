/********************************************************************
    created:  2012/01/27
    created:  27:1:2012   12:34
    filename: ActionsGroupManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__ACTIONSGROUPMANAGER_H__
#define HEADER_GUARD_CORE__ACTIONSGROUPMANAGER_H__

#include <core/IActionsGroupManager.h>

#include <map>
#include <vector>
#include <QtCore/QString>

class QObject;
class QWidget;
class QMenu;
class QAction;

namespace core {

class ActionsGroup
{
public:
    ActionsGroup(const QString & name);
    ~ActionsGroup();

    void addAction(QObject * action);

    const QString & name() const;
    void getAllObjects(std::map<int, QObject *> & allObjects) const;
    void getWidgets(std::map<int, QWidget *> & widgets) const;
    void getActions(std::map<int, QAction *> & actions) const;
    void getMenus(std::map<int, QMenu *> & menus) const;
    void getObjects(std::map<int, QObject *> & objects) const;

private:
    QString name_;
    std::map<QObject *, int> allActions;
    std::map<int, QObject *> objects;
    std::map<int, QAction *> actions;
    std::map<int, QMenu *> menus;
    std::map<int, QWidget *> widgets;

};

class ActionsGroupManager : public IActionsGroupManager
{
public:
    typedef std::vector<ActionsGroup> Groups;
    typedef  Groups::size_type size_type;
    typedef  Groups::const_iterator const_iterator;
    typedef  Groups::const_reverse_iterator const_reverse_iterator;


public:
    ActionsGroupManager();
    virtual ~ActionsGroupManager();

    virtual GroupID createGroup(const QString & groupName);
    virtual void addGroupAction(GroupID groupID, QObject * action);

    size_type size() const;
    bool empty() const;

    const_iterator begin() const;
    const_iterator end() const;

    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

private:

    Groups groups;
    std::map<QString, int> nameToPos;

};

}

#endif
