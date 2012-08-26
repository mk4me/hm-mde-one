/********************************************************************
    created:  2011/04/29
    created:  29:4:2011   14:42
    filename: IEDRTitleBar.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IEDRTITLEBAR_H__
#define HEADER_GUARD_CORE__IEDRTITLEBAR_H__

class QObject;

class IEDRTitleBar
{
public:

    typedef enum {Left, Right} SideType;

public:

    virtual void addObject(QObject * object, SideType side) = 0;
    virtual void removeObject(QObject * object) = 0;

    virtual void clearSide(SideType side) = 0;
    virtual void clear() = 0;


};

#endif  //  HEADER_GUARD_CORE__IEDRTITLEBAR_H__
