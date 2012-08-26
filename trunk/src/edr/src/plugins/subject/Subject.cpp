#include "SubjectPCH.h"
#include "Subject.h"
#include <core/DataAccessors.h>
#include <plugins/subject/ISession.h>

Subject::Subject(PluginSubject::SubjectID subjectID) : subjectID(subjectID), currentSessionID(0)
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

PluginSubject::SubjectID Subject::getID() const
{
    return subjectID;
}

void Subject::getSessions(PluginSubject::Sessions & sessions) const
{
    PluginSubject::Sessions toFilter;
    core::queryDataPtr(core::getDataManagerReader(), toFilter, true);

    for(auto it = toFilter.begin(); it != toFilter.end(); ++it){
        if((*it)->getSubject().get() == this){
            sessions.push_back(*it);
        }
    }
}

