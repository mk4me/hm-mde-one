/********************************************************************
	created:	2011/11/06
	created:	6:11:2011   14:44
	filename: 	IllnessUnit.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMM__ILLNESSUNIT_H__
#define HEADER_GUARD_HMM__ILLNESSUNIT_H__

#include <plugins/subject/SubjectDataFilters.h>
#include "FilterCommand.h"
#include "TreeItemHelper.h"
#include <plugins/subject/IMotion.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/ISubject.h>

//! klasa zapewnia filtrowanie po typie przekazanym w szablonie
template<class Collection>
class ChannelExtractorFilter : public SubjectHierarchyEmptyFilter
{
public:
    //! konstruktor
    ChannelExtractorFilter() {}

	virtual void filterSubjects(const core::ConstObjectsList & inputSubjects, core::ConstObjectsList & outputSubjects) const
	{
		for(auto subjectIT = inputSubjects.begin(); subjectIT != inputSubjects.end(); ++subjectIT){
			PluginSubject::SubjectConstPtr s = (*subjectIT)->get();
			core::ConstObjectsList subjectSessions;
			s->getSessions(subjectSessions);
			bool found = false;
			for(auto sessionIT = subjectSessions.begin(); sessionIT != subjectSessions.end(); ++sessionIT){
				PluginSubject::SessionConstPtr s = (*sessionIT)->get();
				core::ConstObjectsList sessionMotions;
				s->getMotions(sessionMotions);
				for(auto motionIT = sessionMotions.begin(); motionIT != sessionMotions.end(); ++motionIT){
					PluginSubject::MotionConstPtr m = (*motionIT)->get();
					if(m->hasObject(typeid(Collection), false) == true){
						outputSubjects.push_back(*subjectIT);
						found = true;
						break;
					}
				}

				if(found == true){
					break;
				}
			}
		}
	}

	virtual void filterSubjectSessions(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectSessions) const
	{
		PluginSubject::SubjectConstPtr s = subject->get();
		core::ConstObjectsList sessions;
		s->getSessions(sessions);
		filterSessions(sessions, outputSubjectSessions);
	}

	virtual void filterSessions(const core::ConstObjectsList & inputSessions, core::ConstObjectsList & outputSessions) const
	{
		for(auto sessionIT = inputSessions.begin(); sessionIT != inputSessions.end(); ++sessionIT){
			PluginSubject::SessionConstPtr s = (*sessionIT)->get();
			core::ConstObjectsList sessionMotions;
			s->getMotions(sessionMotions);
			for(auto motionIT = sessionMotions.begin(); motionIT != sessionMotions.end(); ++motionIT){
				PluginSubject::MotionConstPtr m = (*motionIT)->get();
				if(m->hasObject(typeid(Collection), false) == true){
					outputSessions.push_back(*sessionIT);
					break;
				}
			}
		}
	}

	virtual void filterSessionMotions(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionMotions) const
	{
		PluginSubject::SessionConstPtr s = session->get();
		core::ConstObjectsList sessionMotions;
		s->getMotions(sessionMotions);
		filterMotions(sessionMotions, outputSessionMotions);
	}

	virtual void filterMotions(const core::ConstObjectsList & inputMotions, core::ConstObjectsList & outputMotions) const
	{
		for(auto it = inputMotions.begin(); it != inputMotions.end(); ++it){
			PluginSubject::MotionConstPtr m = (*it)->get();
			if(m->hasObject(typeid(Collection), false) == true){
				outputMotions.push_back(*it);
			}
		}
	}

	virtual void filterMotionData(const core::ObjectWrapperConstPtr & motion, core::ConstObjectsList & outputMotionData) const
	{
		PluginSubject::MotionConstPtr m = motion->get();
		m->getObjects(outputMotionData, typeid(Collection), false);
	}
};

//! klasa zapewnia filtrowanie po numerze sesji pomiarowej
class SessionNumberFilter : public SubjectHierarchyEmptyFilter
{
public:
    //! konstruktor, który umożliwia filtrowanie po jednym nr sesji
    //! \param nr tylko sesje o tym numerze będą przefiltrowane
    SessionNumberFilter(int nr) { numbers.insert(nr); }
    //! konstruktor, który umożliwia filtrowanie po dwóch nr sesji
    //! \param nr1 pierwszy filtrowany nr sesji
    //! \param nr2 drugi filtrowany nr sesji
    SessionNumberFilter(int nr1, int nr2) { numbers.insert(nr1); numbers.insert(nr2); }
    //! konstruktor, który umożliwia filtrowanie po wielu numerach sesji
    //! \param numbers numery do filtrowania
    SessionNumberFilter(const std::vector<int>& numbers) : numbers(numbers.begin(), numbers.end()) { }

	virtual void filterSubjects(const core::ConstObjectsList & inputSubjects, core::ConstObjectsList & outputSubjects) const
	{
		for(auto subjectIT = inputSubjects.begin(); subjectIT != inputSubjects.end(); ++subjectIT){
			PluginSubject::SubjectConstPtr s = (*subjectIT)->get();
			core::ConstObjectsList subjectSessions;
			s->getSessions(subjectSessions);
			bool found = false;
			for(auto sessionIT = subjectSessions.begin(); sessionIT != subjectSessions.end(); ++sessionIT){
				PluginSubject::SessionConstPtr s = (*sessionIT)->get();
				if(numbers.find(s->getLocalID()) != numbers.end()){
					outputSubjects.push_back(*subjectIT);
					break;
				}
			}
		}
	}

	virtual void filterSubjectSessions(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectSessions) const
	{
		PluginSubject::SubjectConstPtr s = subject->get();
		core::ConstObjectsList sessions;
		s->getSessions(sessions);
		filterSessions(sessions, outputSubjectSessions);
	}

	virtual void filterSessions(const core::ConstObjectsList & inputSessions, core::ConstObjectsList & outputSessions) const
	{
		for(auto sessionIT = inputSessions.begin(); sessionIT != inputSessions.end(); ++sessionIT){
			PluginSubject::SessionConstPtr s = (*sessionIT)->get();
			if(numbers.find(s->getLocalID()) != numbers.end()){
				outputSessions.push_back(*sessionIT);
			}
		}
	}

	virtual void filterMotions(const core::ConstObjectsList & inputMotions, core::ConstObjectsList & outputMotions) const
	{
		for(auto it = inputMotions.begin(); it != inputMotions.end(); ++it){
			PluginSubject::MotionConstPtr m = (*it)->get();
			if(numbers.find(m->getUnpackedSession()->getLocalID()) != numbers.end()){
				outputMotions.push_back(*it);
			}
		}
	}

private:
    //! numery do filtrowania sesji
    std::set<int> numbers;
};

//! klasa zapewnia filtrowanie po nazwach obiektów
class NameFilter : public SubjectHierarchyEmptyFilter
{
public:
    //! konstruktor zapewniający filtrowanie obiektów o konkretnej nazwie
    //! \param name nazwa filtrowanego obiektu
    NameFilter(const std::string& name) { names.insert(name); }
    //! konstruktor zapewniający filtrowanie obiektów o podanych nazwach
    //! \param name nazwy filtrowanych obiektów 
    NameFilter(const std::vector<std::string>& names) : names(names.begin(), names.end()) {}

public:

	static const bool getObjectName(const core::ObjectWrapperConstPtr & object, std::string & name)
	{
		return (object->tryGetMeta("core/uiUserName", name) == true || object->tryGetMeta("core/uiName", name) == true
			|| object->tryGetMeta("core/name", name) == true);
	}

	virtual void filterSubjects(const core::ConstObjectsList & inputSubjects, core::ConstObjectsList & outputSubjects) const
	{
		for(auto subjectIT = inputSubjects.begin(); subjectIT != inputSubjects.end(); ++subjectIT){
			PluginSubject::SubjectConstPtr s = (*subjectIT)->get();
			core::ConstObjectsList subjectSessions;
			s->getSessions(subjectSessions);
			bool found = false;
			for(auto sessionIT = subjectSessions.begin(); sessionIT != subjectSessions.end(); ++sessionIT){
				PluginSubject::SessionConstPtr s = (*sessionIT)->get();
				core::ConstObjectsList motions;
				s->getMotions(motions);
				for(auto motionIT = motions.begin(); motionIT != motions.end(); ++motionIT){
					PluginSubject::MotionConstPtr m = (*motionIT)->get();
					core::ConstObjectsList data;
					m->getObjects(data);
					for(auto dataIT = data.begin(); dataIT != data.end(); ++dataIT){
						std::string name;
						if(getObjectName(*dataIT, name) == true && names.find(name) != names.end()){
							outputSubjects.push_back(*subjectIT);
							found = true;
							break;
						}
					}

					if(found == true){
						break;
					}
				}

				if(found == true){
					break;
				}
			}
		}
	}

	virtual void filterSubjectSessions(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectSessions) const
	{
		PluginSubject::SubjectConstPtr s = subject->get();
		core::ConstObjectsList sessions;
		s->getSessions(sessions);
		filterSessions(sessions, outputSubjectSessions);
	}

	virtual void filterSessions(const core::ConstObjectsList & inputSessions, core::ConstObjectsList & outputSessions) const
	{
		for(auto sessionIT = inputSessions.begin(); sessionIT != inputSessions.end(); ++sessionIT){
			PluginSubject::SessionConstPtr s = (*sessionIT)->get();
			core::ConstObjectsList motions;
			s->getMotions(motions);
			bool found = false;
			for(auto motionIT = motions.begin(); motionIT != motions.end(); ++motionIT){
				PluginSubject::MotionConstPtr m = (*motionIT)->get();
				core::ConstObjectsList data;
				m->getObjects(data);
				for(auto dataIT = data.begin(); dataIT != data.end(); ++dataIT){
					std::string name;
					if(getObjectName(*dataIT, name) == true && names.find(name) != names.end()){
						outputSessions.push_back(*sessionIT);
						found = true;
						break;
					}
				}

				if(found == true){
					break;
				}
			}
		}
	}

	virtual void filterMotions(const core::ConstObjectsList & inputMotions, core::ConstObjectsList & outputMotions) const
	{
		for(auto it = inputMotions.begin(); it != inputMotions.end(); ++it){
			PluginSubject::MotionConstPtr m = (*it)->get();
			core::ConstObjectsList data;
			m->getObjects(data);
			for(auto dataIT = data.begin(); dataIT != data.end(); ++dataIT){
				std::string name;
				if(getObjectName(*dataIT, name) == true && names.find(name) != names.end()){
					outputMotions.push_back(*it);
					break;
				}
			}
		}
	}

	virtual void filterMotionData(const core::ObjectWrapperConstPtr & motion, core::ConstObjectsList & outputMotionData) const
	{
		PluginSubject::MotionConstPtr m = motion->get();
		core::ConstObjectsList data;
		m->getObjects(data);
		for(auto dataIT = data.begin(); dataIT != data.end(); ++dataIT){
			std::string name;
			if(getObjectName(*dataIT, name) == true && names.find(name) != names.end()){
				outputMotionData.push_back(*dataIT);
				break;
			}
		}
	}

private:
    //! dostarczone nazwy filtrowanych obiektów
    std::set<std::string> names;
};

//! klasa tworzy drzewo analiz z danymi istotnymi dla analizy zabiegu endoprotezoplastyki
class Endo : public IFilterCommand
{
public:
    //! tworzy element drzewa analiz z przefiltrowanymi danymi
    //! \param rootItemName nazwa elementu głównego
    //! \param sessions sesje, które będą filtrowane
    virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const core::ConstObjectsList& sessions );

private:
    //! tworzy elementy z danymi EMG
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje
    void createEMGBranch( QTreeWidgetItem* root, const core::ConstObjectsList& sessions );
    //! tworzy elementy z danymi kinetycznymi
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje
    void createKineticBranch( QTreeWidgetItem* root, const core::ConstObjectsList& sessions );
    //! tworzy elementy z danymi kątów
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje 
    void createAngleBranch( QTreeWidgetItem* root, const core::ConstObjectsList& sessions );

protected:
    //! Tworzy element zawierający zbiorcze zestawienie wykresów
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje 
    //! \param name nazwa elementu
    //! \param leftPrev wykresy, dla kontekstu lewej nogi, przed zabiegiem
    //! \param rightPrev wykresy, dla kontekstu prawej nogi, przed zabiegiem 
    //! \param leftPost wykresy, dla kontekstu lewej nogi, po zabiegu
    //! \param rightPost wykresy, dla kontekstu prawej nogi, po zabiegu
    //! \param index indeks kanału (0 - x, 1 - y, 2 - z)
    void createVectorEntry(QTreeWidgetItem* root, const core::ConstObjectsList& sessions, const QString& name,
        SubjectHierarchyFilterPtr leftPrev, SubjectHierarchyFilterPtr rightPrev, SubjectHierarchyFilterPtr leftPost, SubjectHierarchyFilterPtr rightPost, int index);
    //! Tworzy element zawierający zbiorcze zestawienie wykresów
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje 
    //! \param name nazwa elementu
    //! \param leftPrev wykresy, dla kontekstu lewej nogi, przed zabiegiem
    //! \param rightPrev wykresy, dla kontekstu prawej nogi, przed zabiegiem 
    //! \param leftPost wykresy, dla kontekstu lewej nogi, po zabiegu
    //! \param rightPost wykresy, dla kontekstu prawej nogi, po zabiegu
    void createEMGEntry(QTreeWidgetItem* root, const core::ConstObjectsList& sessions, const QString& name,
        SubjectHierarchyFilterPtr leftPrev, SubjectHierarchyFilterPtr rightPrev, SubjectHierarchyFilterPtr leftPost, SubjectHierarchyFilterPtr rightPost);
    //! tworzy obiekt, który filtruje elementy emg po nazwie i czasie zabiegu
    //! \param post po lub przed zabiegiem
    //! \param name nazwa filtrowanych elementów
    SubjectHierarchyFilterPtr createCustomEMGFilter(bool post, const std::string& name);
    std::vector<core::ObjectWrapperConstPtr> extractWrappersFromEMG(const core::ConstObjectsList& sessions);
    //! tworzy obiekt, który filtruje elementy z wektorem po nazwie i czasie zabiegu
    //! \param post po lub przed zabiegiem
    //! \param name nazwa filtrowanych elementów
    template<class CollectionPtr>
    SubjectHierarchyFilterPtr createCustomV3Filter(bool post, const std::string& name);
    std::vector<core::ObjectWrapperConstPtr> extractWrappersFromVector( const core::ConstObjectsList& sessions, int scalarIndex );
};

//! klasa tworzy drzewo analiz z danymi istotnymi dla analizy zmian po wylewie
class Stroke : public Endo
{
public:
    //! tworzy element drzewa analiz z przefiltrowanymi danymi
    //! \param rootItemName nazwa elementu głównego
    //! \param sessions sesje, które będą filtrowane
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const core::ConstObjectsList& sessions );
};

//! klasa tworzy drzewo analiz z danymi istotnymi dla analizy urazu kręgosłupa
class Spine : public Endo
{
public:
    //! tworzy element drzewa analiz z przefiltrowanymi danymi
    //! \param rootItemName nazwa elementu głównego
    //! \param sessions sesje, które będą filtrowane
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const core::ConstObjectsList& sessions );

private:
    //! tworzy elementy z danymi EMG
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje
    void createEMGBranch( QTreeWidgetItem* root, const core::ConstObjectsList& sessions );
    //! tworzy elementy z danymi kinetycznymi
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje
    void createKineticBranch( QTreeWidgetItem* root, const core::ConstObjectsList& sessions );
    //! tworzy elementy z danymi kątów
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje 
    void createAngleBranch( QTreeWidgetItem* root, const core::ConstObjectsList& sessions );
};
#endif
