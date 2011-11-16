/********************************************************************
    created:  2011/11/02
    created:  2:11:2011   12:57
    filename: Types.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_SUBJECT__TYPES_H__
#define HEADER_GUARD_SUBJECT__TYPES_H__

#include <core/SmartPtr.h>
#include <vector>

class ISubjectService;

typedef unsigned int SubjectID;

class ISubject;
typedef core::shared_ptr<ISubject> SubjectPtr;
typedef core::shared_ptr<const ISubject> SubjectConstPtr;

class ISession;
typedef core::shared_ptr<ISession> SessionPtr;
typedef core::shared_ptr<const ISession> SessionConstPtr;
typedef std::vector<SessionConstPtr> Sessions;

class IMotion;
typedef core::shared_ptr<IMotion> MotionPtr;
typedef core::shared_ptr<const IMotion> MotionConstPtr;
typedef std::vector<MotionConstPtr> Motions;

namespace Sub{
    class IDataFilter;
    class IFilteredDataFactory;
}

typedef core::shared_ptr<Sub::IDataFilter> DataFilterPtr;
typedef core::shared_ptr<const Sub::IDataFilter> DataFilterConstPtr;

typedef core::shared_ptr<Sub::IFilteredDataFactory> FilteredDataFacoryPtr;
typedef core::shared_ptr<const Sub::IFilteredDataFactory> FilteredDataFacoryConstPtr;

#endif HEADER_GUARD_SUBJECT__TYPES_H__