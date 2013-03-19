#include "SubjectPCH.h"
#include "Subject.h"
#include <corelib/DataAccessors.h>
#include <plugins/subject/ISession.h>
#include <corelib/BaseDataTypes.h>
#include <corelib/ObjectWrapperCollection.h>
#include <algorithm>

using namespace core;
using namespace plugin;

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

PluginSubject::SubjectID Subject::nextSessionID() const
{
	return currentSessionID++;
}

void Subject::getSessions(core::ConstObjectsList & sessions) const
{
	sessionStorage.getObjects(sessions, typeid(PluginSubject::ISession), false);
}

void Subject::addSession(const core::ObjectWrapperConstPtr & session)
{
	utils::TypeInfoList types;
	session->getSupportedTypes(types);
	if(std::find(types.begin(), types.end(), typeid(PluginSubject::ISession)) == types.end()){
		throw std::runtime_error("Data type does not support PluginSubject::ISession type");
	}

	sessionStorage.addData(session);
}

void Subject::removeSession(const core::ObjectWrapperConstPtr & session)
{
	sessionStorage.removeData(session);
}

void Subject::addData(const core::ObjectWrapperConstPtr & data)
{
	dataStorage.addData(data);
}

void Subject::removeData(const core::ObjectWrapperConstPtr & data)
{
	dataStorage.removeData(data);
}

const bool Subject::tryAddData(const core::ObjectWrapperConstPtr & data)
{
	return dataStorage.tryAddData(data);
}

const bool Subject::tryRemoveData(const core::ObjectWrapperConstPtr & data)
{
	return dataStorage.tryRemoveData(data);
}

void Subject::getObjects(core::ConstObjectsList & objects) const
{
	dataStorage.getObjects(objects);
}

void Subject::getObjects(core::ConstObjectsList & objects, const core::TypeInfo & type, bool exact) const
{
	dataStorage.getObjects(objects, type, exact);
}

void Subject::getObjects(core::ObjectWrapperCollection& objects) const
{
	dataStorage.getObjects(objects);
}

const bool Subject::isManaged(const core::ObjectWrapperConstPtr & object) const
{
	return dataStorage.isManaged(object);
}

const bool Subject::hasObject(const core::TypeInfo & type, bool exact) const
{
	return dataStorage.hasObject(type, exact);
}