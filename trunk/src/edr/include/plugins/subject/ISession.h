/********************************************************************
    created:  2011/11/03
    created:  3:11:2011   10:04
    filename: ISession.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISESSION_H__
#define HEADER_GUARD___ISESSION_H__

#include <plugins/subject/Types.h>
#include <core/ObjectWrapper.h>

class ISession
{
public:
    virtual ~ISession() {}

    virtual SubjectID getID() const = 0;
    virtual SubjectID getLocalID() const = 0;
    virtual void getMotions(Motions & motions) const = 0;

    virtual const SubjectConstPtr & getSubject() const = 0;

    virtual const std::string & getName() const = 0;
    virtual const std::string & getLocalName() const = 0;

    virtual unsigned int getYear() const = 0;
    virtual unsigned char getMonth() const = 0;
    virtual unsigned char getDay() const = 0;

    virtual void getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const = 0;

    virtual core::ObjectWrapperConstPtr getWrapperOfType(const core::TypeInfo& type, bool exact = false) const
    {
        std::vector<core::ObjectWrapperConstPtr> wrappers;
        getWrappers(wrappers);
        for(auto it = wrappers.begin(); it != wrappers.end(); it++){
            if((*it)->getTypeInfo() == type || (exact == false && (*it)->isSupported(type))){
                return (*it);
            }
        }

        //throw std::runtime_error("Object type not stored in Session");
        return core::ObjectWrapperConstPtr();
    }

    virtual bool hasObjectOfType(const core::TypeInfo& type, bool exact = false) const
    {
        std::vector<core::ObjectWrapperConstPtr> wrappers;
        getWrappers(wrappers);

        for(auto it = wrappers.begin(); it != wrappers.end(); it++){
            if((*it)->getTypeInfo() == type || (exact == false && (*it)->isSupported(type))){
                return true;
            }
        }

        return false;
    }
};

CORE_DEFINE_WRAPPER(ISession, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif //   HEADER_GUARD___ISESSION_H__