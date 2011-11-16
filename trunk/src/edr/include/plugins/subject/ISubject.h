/********************************************************************
    created:  2011/11/03
    created:  3:11:2011   10:06
    filename: ISubject.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISUBJECT_H__
#define HEADER_GUARD___ISUBJECT_H__

#include <plugins/subject/Types.h>
#include <core/ObjectWrapper.h>

class ISubject
{
public:

    virtual ~ISubject() {}

    virtual SubjectID getID() const = 0;
    virtual const std::string & getName() const = 0;
    virtual void getSessions(Sessions & sessions) const = 0;
};

CORE_DEFINE_WRAPPER(ISubject, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif //   HEADER_GUARD___ISUBJECT_H__

