#include "SubjectPCH.h"
#include "Subject.h"
#include <core/DataAccessors.h>
#include <plugins/subject/ISession.h>

PluginSubject::SubjectID Subject::globalID = 0;

PluginSubject::SubjectID Subject::nextGlobalID()
{
	if(globalID == std::numeric_limits<PluginSubject::SubjectID>::max()){
		throw std::runtime_error("Subjects overflow");
	}

	return globalID++;
}

Subject::Subject() : subjectID(nextGlobalID()), currentSessionID(0)
{
    generateName(name, subjectID);
}

Subject::~Subject()
{

}

void Subject::generateName(std::string & name, PluginSubject::SubjectID subjectID)
{
	std::stringstream ss;
	ss.fill('0');
	ss.flags(std::ios::right);
	ss.width(4); ss << subjectID;
	name = "Subject";
	name.append(ss.str());
}

const std::string & Subject::getName() const
{
    return name;
}

PluginSubject::SubjectID Subject::getID() const
{
    return subjectID;
}

void Subject::getSessions(core::ConstObjectsList & sessions) const
{
	static const core::TypeInfo type = core::TypeInfo(typeid(PluginSubject::ISession));
    core::ConstObjectsList toFilter;
    core::getDataManagerReader()->getObjects(toFilter, type, false);

    for(auto it = toFilter.begin(); it != toFilter.end(); ++it){
		PluginSubject::SessionConstPtr session;
		bool ok = (*it)->tryGet(session, false);
		if(ok && session != nullptr && session->getUnpackedSubject()->getID() == subjectID ){
            sessions.push_back(*it);
        }
    }
}

PluginSubject::SubjectID Subject::nextSessionID() const
{
	return currentSessionID++;
}

FilteredSubject::FilteredSubject(const core::ObjectWrapperPtr & originalSubject, const PluginSubject::SubjectConstPtr & originalUnpackedSubject, const core::ConstObjectsList & sessions)
	: originalSubject(originalSubject), originalUnpackedSubject(originalUnpackedSubject), sessions(sessions)
{

}

FilteredSubject::~FilteredSubject()
{

}

const std::string & FilteredSubject::getName() const
{
	return originalUnpackedSubject->getName();
}

PluginSubject::SubjectID FilteredSubject::getID() const
{
	return originalUnpackedSubject->getID();
}

void FilteredSubject::getSessions(core::ConstObjectsList & sessions) const
{
	sessions.insert(sessions.end(), this->sessions.begin(), this->sessions.end());
}

