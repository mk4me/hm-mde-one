/********************************************************************
    created:  2011/10/23
    created:  23:10:2011   17:29
    filename: ManagerHelper.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MANAGERHELPER_H__
#define HEADER_GUARD___MANAGERHELPER_H__

template<class Manager>
class ManagerHelper
{
public:
    virtual ~ManagerHelper() {}

    inline static Manager * getInstance()
    {
        return manager;
    }

public:
    static Manager * manager;
};

#endif HEADER_GUARD___MANAGERHELPER_H__
