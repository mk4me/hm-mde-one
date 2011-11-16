#include "SubjectPCH.h"
#include <limits>
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <plugins/subject/Subject.h>
#include <core/DataAccessors.h>
#include <plugins/subject/ISession.h>

Subject::Subject(core::IMemoryDataManager * memoryDataManager, SubjectID subjectID) : memoryDataManager(memoryDataManager), subjectID(subjectID), currentSessionID(0)
{
    std::stringstream ss;
    ss.fill('0');
    ss.flags(std::ios::right);
    ss.width(4); ss << subjectID;
    name = "Subject";
    name.append(ss.str());
}

Subject::~Subject()
{

}

const std::string & Subject::getName() const
{
    return name;
}

//void Subject::setCurrentMotion( MotionConstPtr motion )
//{	 
//	 
//}	 
//	 
//void Subject::addSystem( ISubjectSystemPtr system )
//{
//
//}

SubjectID Subject::getID() const
{
    return subjectID;
}

//const std::string & Subject::getName() const
//{
//    return name;
//}

void Subject::getSessions(Sessions & sessions) const
{
    Sessions toFilter;
    core::queryDataPtr(memoryDataManager, toFilter, true);

    for(auto it = toFilter.begin(); it != toFilter.end(); it++){
        if((*it)->getSubject().get() == this){
            sessions.push_back(*it);
        }
    }
}

//Subject::Subject( const Subject& obj ) : subjectID(obj.subjectID)
//{
//	throw std::runtime_error("not implemented");
//}

