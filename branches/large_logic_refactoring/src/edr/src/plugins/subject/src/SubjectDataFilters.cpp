#include <corelib/SubjectDataFilters.h>
#include <plugins/subject/IMotion.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/ISubject.h>

SubjectHierarchyEmptyFilter::SubjectHierarchyEmptyFilter()
{

}

SubjectHierarchyEmptyFilter::~SubjectHierarchyEmptyFilter()
{

}

void SubjectHierarchyEmptyFilter::filterSubjects(const core::ConstObjectsList & inputSubjects, core::ConstObjectsList & outputSubjects) const
{
	outputSubjects.insert(outputSubjects.end(), inputSubjects.begin(), inputSubjects.end());
}

void SubjectHierarchyEmptyFilter::filterSubjectData(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectData) const
{
	PluginSubject::SubjectConstPtr s = subject->get();
	s->getObjects(outputSubjectData);
}

void SubjectHierarchyEmptyFilter::filterSubjectSessions(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectSessions) const
{
	PluginSubject::SubjectConstPtr s = subject->get();
	s->getSessions(outputSubjectSessions);
}

void SubjectHierarchyEmptyFilter::filterSessions(const core::ConstObjectsList & inputSessions, core::ConstObjectsList & outputSessions) const
{
	outputSessions.insert(outputSessions.end(), inputSessions.begin(), inputSessions.end());
}

void SubjectHierarchyEmptyFilter::filterSessionData(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionData) const
{
	PluginSubject::SessionConstPtr s = session->get();
	s->getObjects(outputSessionData);
}

void SubjectHierarchyEmptyFilter::filterSessionMotions(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionMotions) const
{
	PluginSubject::SessionConstPtr s = session->get();
	s->getMotions(outputSessionMotions);
}

void SubjectHierarchyEmptyFilter::filterMotions(const core::ConstObjectsList & inputMotion, core::ConstObjectsList & outputMotions) const
{
	outputMotions.insert(outputMotions.end(), inputMotion.begin(), inputMotion.end());
}

void SubjectHierarchyEmptyFilter::filterMotionData(const core::ObjectWrapperConstPtr & motion, core::ConstObjectsList & outputMotionData) const
{
	PluginSubject::MotionConstPtr s = motion->get();
	s->getObjects(outputMotionData);	
}

SubjectHierarchyCompoundFilter::SubjectHierarchyCompoundFilter(const Filters & filters) : filters(filters)
{

}

SubjectHierarchyCompoundFilter::~SubjectHierarchyCompoundFilter()
{

}

void SubjectHierarchyCompoundFilter::filterSubjects(const core::ConstObjectsList & inputSubjects, core::ConstObjectsList & outputSubjects) const
{
	core::ConstObjectsList localInput(inputSubjects);

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstObjectsList localOutput;
		(*it)->filterSubjects(localInput, localOutput);
		localInput.swap(localOutput);			
	}

	outputSubjects.insert(outputSubjects.end(), localInput.begin(), localInput.end());
}

void SubjectHierarchyCompoundFilter::filterSubjectData(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectData) const
{
	PluginSubject::SubjectConstPtr s = subject->get();
	core::ConstObjects localInputSet;
	{
		core::ConstObjectsList localInput;
		s->getObjects(localInput);
		localInputSet.insert(localInput.begin(), localInput.end());
	}		

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstObjectsList localOutput;
		(*it)->filterSubjectData(subject, localOutput);
		core::ConstObjects localOutputSet(localOutput.begin(), localOutput.end());
		core::ConstObjectsList localIntersection(std::min(localOutputSet.size(), localInputSet.size()));
		auto endIT = std::set_intersection(localInputSet.begin(), localInputSet.end(), localOutputSet.begin(), localOutputSet.end(), localIntersection.begin());
		localInputSet = core::ConstObjects(localIntersection.begin(), endIT);
	}

	outputSubjectData.insert(outputSubjectData.end(), localInputSet.begin(), localInputSet.end());
}

void SubjectHierarchyCompoundFilter::filterSubjectSessions(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectSessions) const
{
	PluginSubject::SubjectConstPtr s = subject->get();
	core::ConstObjects localInputSet;
	{
		core::ConstObjectsList localInput;
		s->getSessions(localInput);
		localInputSet.insert(localInput.begin(), localInput.end());
	}		

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstObjectsList localOutput;
		(*it)->filterSubjectSessions(subject, localOutput);
		core::ConstObjects localOutputSet(localOutput.begin(), localOutput.end());
		core::ConstObjectsList localIntersection(std::min(localOutputSet.size(), localInputSet.size()));
		auto endIT = std::set_intersection(localInputSet.begin(), localInputSet.end(), localOutputSet.begin(), localOutputSet.end(), localIntersection.begin());
		localInputSet = core::ConstObjects(localIntersection.begin(), endIT);
	}

	outputSubjectSessions.insert(outputSubjectSessions.end(), localInputSet.begin(), localInputSet.end());
}

void SubjectHierarchyCompoundFilter::filterSessions(const core::ConstObjectsList & inputSessions, core::ConstObjectsList & outputSessions) const
{
	core::ConstObjectsList localInput(inputSessions);

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstObjectsList localOutput;
		(*it)->filterSessions(localInput, localOutput);
		localInput.swap(localOutput);
	}

	outputSessions.insert(outputSessions.end(), localInput.begin(), localInput.end());
}

void SubjectHierarchyCompoundFilter::filterSessionData(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionData) const
{
	PluginSubject::SessionConstPtr s = session->get();
	core::ConstObjects localInputSet;
	{
		core::ConstObjectsList localInput;
		s->getObjects(localInput);
		localInputSet.insert(localInput.begin(), localInput.end());
	}		

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstObjectsList localOutput;
		(*it)->filterSessionData(session, localOutput);
		core::ConstObjects localOutputSet(localOutput.begin(), localOutput.end());
		core::ConstObjectsList localIntersection(std::min(localOutputSet.size(), localInputSet.size()));
		auto endIT = std::set_intersection(localInputSet.begin(), localInputSet.end(), localOutputSet.begin(), localOutputSet.end(), localIntersection.begin());
		localInputSet = core::ConstObjects(localIntersection.begin(), endIT);
	}

	outputSessionData.insert(outputSessionData.end(), localInputSet.begin(), localInputSet.end());
}

void SubjectHierarchyCompoundFilter::filterSessionMotions(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionMotions) const
{
	PluginSubject::SessionConstPtr s = session->get();
	core::ConstObjects localInputSet;
	{
		core::ConstObjectsList localInput;
		s->getMotions(localInput);
		localInputSet.insert(localInput.begin(), localInput.end());
	}		

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstObjectsList localOutput;
		(*it)->filterSessionMotions(session, localOutput);
		core::ConstObjects localOutputSet(localOutput.begin(), localOutput.end());
		core::ConstObjectsList localIntersection(std::min(localOutputSet.size(), localInputSet.size()));
		auto endIT = std::set_intersection(localInputSet.begin(), localInputSet.end(), localOutputSet.begin(), localOutputSet.end(), localIntersection.begin());
		localInputSet = core::ConstObjects(localIntersection.begin(), endIT);
	}

	outputSessionMotions.insert(outputSessionMotions.end(), localInputSet.begin(), localInputSet.end());
}

void SubjectHierarchyCompoundFilter::filterMotions(const core::ConstObjectsList & inputMotion, core::ConstObjectsList & outputMotions) const
{
	core::ConstObjectsList localInput(inputMotion);

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstObjectsList localOutput;
		(*it)->filterSessions(localInput, localOutput);
		localInput.swap(localOutput);
	}

	outputMotions.insert(outputMotions.end(), localInput.begin(), localInput.end());
}

void SubjectHierarchyCompoundFilter::filterMotionData(const core::ObjectWrapperConstPtr & motion, core::ConstObjectsList & outputMotionData) const
{
	PluginSubject::MotionConstPtr m = motion->get();
	core::ConstObjects localInputSet;
	{
		core::ConstObjectsList localInput;
		m->getObjects(localInput);
		localInputSet.insert(localInput.begin(), localInput.end());
	}		

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstObjectsList localOutput;
		(*it)->filterMotionData(motion, localOutput);
		core::ConstObjects localOutputSet(localOutput.begin(), localOutput.end());
		core::ConstObjectsList localIntersection(std::min(localOutputSet.size(), localInputSet.size()));
		auto endIT = std::set_intersection(localInputSet.begin(), localInputSet.end(), localOutputSet.begin(), localOutputSet.end(), localIntersection.begin());
		localInputSet = core::ConstObjects(localIntersection.begin(), endIT);
	}

	outputMotionData.insert(outputMotionData.end(), localInputSet.begin(), localInputSet.end());
}