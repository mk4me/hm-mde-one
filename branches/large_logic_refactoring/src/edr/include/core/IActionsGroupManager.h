/********************************************************************
    created:  2012/01/27
    created:  27:1:2012   12:30
    filename: IActionsGroupManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IACTIONSGROUPMANAGER_H__
#define HEADER_GUARD_CORE__IACTIONSGROUPMANAGER_H__

#include <QtCore/QString>

class QObject;

namespace plugin {

    class IActionsGroupManager
    {
    public:
        typedef int GroupID;

    public:
        virtual ~IActionsGroupManager() {};

        virtual GroupID createGroup(const QString & groupName) = 0;
        virtual void addGroupAction(GroupID groupID, QObject * action) = 0;
    };

}

#endif
