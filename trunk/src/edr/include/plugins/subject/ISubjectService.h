﻿/********************************************************************
    created:  2011/10/28
    created:  28:10:2011   11:39
    filename: ISubjectService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISUBJECTSERVICE_H__
#define HEADER_GUARD___ISUBJECTSERVICE_H__

#include <vector>
#include <plugins/subject/Types.h>
#include <core/TypeInfo.h>
#include <core/ObjectWrapper.h>

namespace PluginSubject {

class ISubjectService
{
    friend class IDataFilter;

public:
    virtual ~ISubjectService() {}

    virtual SubjectPtr createSubject() = 0;

    virtual SessionPtr createSession(const SubjectConstPtr & subject, const std::vector<core::ObjectWrapperConstPtr> & wrappers) = 0;

    virtual MotionPtr createMotion(const SessionConstPtr & session,
        const std::vector<core::ObjectWrapperConstPtr> & wrappers) = 0;    

private:

    virtual const FilteredDataFacoryPtr & getFilteredDataFacotry() const = 0;
};

}

#endif //   HEADER_GUARD___ISUBJECTSERVICE_H__

