/********************************************************************
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

    virtual core::ObjectWrapperPtr createSubject() = 0;

    virtual core::ObjectWrapperPtr createSession(const core::ObjectWrapperConstPtr & subject, const core::ConstObjectsList & wrappers) = 0;

    virtual core::ObjectWrapperPtr createMotion(const core::ObjectWrapperConstPtr & session,
        const core::ConstObjectsList & wrappers) = 0;    

private:

    virtual const FilteredDataFacoryPtr & getFilteredDataFacotry() const = 0;
};

}

#endif //   HEADER_GUARD___ISUBJECTSERVICE_H__

