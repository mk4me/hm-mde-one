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

//! klasa zapewnia filtrowanie po typie przekazanym w szablonie
template<class Collection>
class ChannelExtractorFilter : public SubjectHierarchyEmptyFilter
{
public:
    //! konstruktor
    ChannelExtractorFilter() {}

	virtual void filterSubjects(const core::ConstObjectsList & inputSubjects, core::ConstObjectsList & outputSubjects) const;

	virtual void filterSubjectData(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectData) const;

	virtual void filterSubjectSessions(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectSessions) const;

	virtual void filterSessions(const core::ConstObjectsList & inputSessions, core::ConstObjectsList & outputSessions) const;

	virtual void filterSessionData(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionData) const;

	virtual void filterSessionMotions(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionMotions) const;

	virtual void filterMotions(const core::ConstObjectsList & inputMotion, core::ConstObjectsList & outputMotions) const;

	virtual void filterMotionData(const core::ObjectWrapperConstPtr & motion, core::ConstObjectsList & outputMotionData) const;

    //! wywołuje filtrowanie sesji po typie przekazanym w szablonie
    //! \param session filtrowane sesje
    //! \return sesje zawierające tylko konkretny typ
    virtual PluginSubject::SessionPtr doDataFiltering(const PluginSubject::SessionConstPtr & session) const
    {
        typedef typename core::ObjectWrapperT<Collection>::Ptr CollectionPtr;
        typedef typename core::ObjectWrapperT<Collection>::ConstPtr CollectionConstPtr;
        typedef typename Collection::ChannelType Channel;
        typedef typename core::ObjectWrapperT<Channel>::Ptr ChannelPtr;
        typedef typename core::ObjectWrapperT<Channel>::ConstPtr ChannelConstPtr;
        PluginSubject::Motions motions;

        std::vector<PluginSubject::MotionPtr> newMotions;

        session->getMotions(motions);
        BOOST_FOREACH(PluginSubject::MotionConstPtr motion, motions) {
            if (motion->hasObject(typeid(Collection), false)) {
                std::vector<core::ObjectWrapperConstPtr> newMotionObjects;
                //m->setName(motion->getName());
                std::vector<core::ObjectWrapperConstPtr> objects;
                motion->getWrappers(objects, typeid(Collection));
                BOOST_FOREACH(core::ObjectWrapperConstPtr ow, objects) {
                    CollectionConstPtr c = ow->get();
                    for (int i = 0 ; i < c->getNumChannels(); ++i) {
                       auto channel = c->getChannel(i);
                       core::ObjectWrapperPtr cw = core::ObjectWrapper::create<Channel>();
                       cw->set(core::const_pointer_cast<Channel>(channel));
					   (*cw)["core/name"] = channel->getName();
					   (*cw)["core/source"] = channel->getName();                       
                       newMotionObjects.push_back(cw);
                    }
                }

                newMotions.push_back(createFilteredMotion(motion, newMotionObjects));
            }
        }

        std::vector<core::ObjectWrapperConstPtr> objects;
        session->getWrappers(objects);

        return createFilteredSession(session, newMotions, objects);
    }
};

//! klasa zapewnia filtrowanie po numerze sesji pomiarowej
class SessionNumberFilter : public SubjectHierarchyEmptyFilter
{
public:
    //! konstruktor, który umożliwia filtrowanie po jednym nr sesji
    //! \param nr tylko sesje o tym numerze będą przefiltrowane
    SessionNumberFilter(int nr) { numbers.push_back(nr); }
    //! konstruktor, który umożliwia filtrowanie po dwóch nr sesji
    //! \param nr1 pierwszy filtrowany nr sesji
    //! \param nr2 drugi filtrowany nr sesji
    SessionNumberFilter(int nr1, int nr2) { numbers.push_back(nr1); numbers.push_back(nr2); }
    //! konstruktor, który umożliwia filtrowanie po wielu numerach sesji
    //! \param numbers numery do filtrowania
    SessionNumberFilter(const std::vector<int>& numbers) : numbers(numbers) { }
    //! wywołuje właściwe filtrowanie sesji po dostarczonych numerach
    //! \param session filtrowane sesje
    //! \return sesje zawierające spełniające warunek 
    virtual PluginSubject::SessionPtr doDataFiltering(const PluginSubject::SessionConstPtr & session ) const
    {
        PluginSubject::SessionPtr ret;

        bool found = false;
        PluginSubject::SubjectID id = session->getLocalID();
        for(auto it = numbers.begin(); it != numbers.end(); ++it){
            if(*it == id){
                found = true;
                break;
            }
        }

        if(found == true){
            ret = cloneSession(session);
        }

        return ret;
    }

private:
    //! numery do filtrowania sesji
    std::vector<int> numbers;
};

//! klasa zapewnia filtrowanie po nazwach obiektów
class NameFilter : public SubjectHierarchyEmptyFilter
{
public:
    //! konstruktor zapewniający filtrowanie obiektów o konkretnej nazwie
    //! \param name nazwa filtrowanego obiektu
    NameFilter(const std::string& name) { names.push_back(name); }
    //! konstruktor zapewniający filtrowanie obiektów o podanych nazwach
    //! \param name nazwy filtrowanych obiektów 
    NameFilter(const std::vector<std::string>& names) : names(names) {}

public:
    //! wywołuje właściwe filtrowanie sesji po dostarczych nazwach
    //! \param session filtrowane sesje
    //! \return sesje zawierające tylko obiekty o dostarczonych nazwach
    virtual PluginSubject::SessionPtr doDataFiltering( const PluginSubject::SessionConstPtr & session ) const
    {
        PluginSubject::SessionPtr ret;
        std::vector<PluginSubject::MotionPtr> newMotions;
        core::ConstObjectsList objects;
        session->getObjects(objects);

        core::ConstObjectsList motions;
        session->getMotions(motions);

        for(auto motionIT = motions.begin(); motionIT != motions.end(); ++motionIT){

			PluginSubject::MotionPtr motion = (*motionIT)->get();

            core::ConstObjectsList objects;
            core::ConstObjectsList validObjects;
            motion->getObjects(objects);

            for(auto objectIT = objects.begin(); objectIT != objects.end(); ++objectIT){
                for (auto it = names.begin(); it != names.end(); ++it) {
					std::string name;
					if((*objectIT)->tryGetMeta("core/name", name) == true && name == *it)
                        validObjects.push_back(*objectIT);
                        break;
                    }
                }
            }

            if(objects.empty() == true || validObjects.empty() == false){
                newMotions.push_back(createFilteredMotion(*motionIT, validObjects));
            }
        }

        return createFilteredSession(session, newMotions, objects);
    }

private:
    //! dostarczone nazwy filtrowanych obiektów
    std::vector<std::string> names;
};

//! klasa tworzy drzewo analiz z danymi istotnymi dla analizy zabiegu endoprotezoplastyki
class Endo : public IFilterCommand
{
public:
    //! tworzy element drzewa analiz z przefiltrowanymi danymi
    //! \param rootItemName nazwa elementu głównego
    //! \param sessions sesje, które będą filtrowane
    virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions );

private:
    //! tworzy elementy z danymi EMG
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje
    void createEMGBranch( QTreeWidgetItem* root, const std::vector<PluginSubject::SessionConstPtr>& sessions );
    //! tworzy elementy z danymi kinetycznymi
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje
    void createKineticBranch( QTreeWidgetItem* root, const std::vector<PluginSubject::SessionConstPtr>& sessions );
    //! tworzy elementy z danymi kątów
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje 
    void createAngleBranch( QTreeWidgetItem* root, const std::vector<PluginSubject::SessionConstPtr>& sessions );

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
    void createVectorEntry(QTreeWidgetItem* root, const std::vector<PluginSubject::SessionConstPtr>& sessions, const QString& name,
        PluginSubject::DataFilterPtr leftPrev, PluginSubject::DataFilterPtr rightPrev, PluginSubject::DataFilterPtr leftPost, PluginSubject::DataFilterPtr rightPost, int index);
    //! Tworzy element zawierający zbiorcze zestawienie wykresów
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje 
    //! \param name nazwa elementu
    //! \param leftPrev wykresy, dla kontekstu lewej nogi, przed zabiegiem
    //! \param rightPrev wykresy, dla kontekstu prawej nogi, przed zabiegiem 
    //! \param leftPost wykresy, dla kontekstu lewej nogi, po zabiegu
    //! \param rightPost wykresy, dla kontekstu prawej nogi, po zabiegu
    void createEMGEntry(QTreeWidgetItem* root, const std::vector<PluginSubject::SessionConstPtr>& sessions, const QString& name,
        PluginSubject::DataFilterPtr leftPrev, PluginSubject::DataFilterPtr rightPrev, PluginSubject::DataFilterPtr leftPost, PluginSubject::DataFilterPtr rightPost);
    //! tworzy obiekt, który filtruje elementy emg po nazwie i czasie zabiegu
    //! \param post po lub przed zabiegiem
    //! \param name nazwa filtrowanych elementów
    PluginSubject::DataFilterPtr createCustomEMGFilter(bool post, const std::string& name);
    std::vector<core::ObjectWrapperConstPtr> extractWrappersFromEMG(const std::vector<PluginSubject::SessionPtr>& sessions);
    //! tworzy obiekt, który filtruje elementy z wektorem po nazwie i czasie zabiegu
    //! \param post po lub przed zabiegiem
    //! \param name nazwa filtrowanych elementów
    template<class CollectionPtr>
    PluginSubject::DataFilterPtr createCustomV3Filter(bool post, const std::string& name);
    std::vector<core::ObjectWrapperConstPtr> extractWrappersFromVector( const std::vector<PluginSubject::SessionPtr>& sessions, int scalarIndex );
};

//! klasa tworzy drzewo analiz z danymi istotnymi dla analizy zmian po wylewie
class Stroke : public Endo
{
public:
    //! tworzy element drzewa analiz z przefiltrowanymi danymi
    //! \param rootItemName nazwa elementu głównego
    //! \param sessions sesje, które będą filtrowane
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions );
};

//! klasa tworzy drzewo analiz z danymi istotnymi dla analizy urazu kręgosłupa
class Spine : public Endo
{
public:
    //! tworzy element drzewa analiz z przefiltrowanymi danymi
    //! \param rootItemName nazwa elementu głównego
    //! \param sessions sesje, które będą filtrowane
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions );

private:
    //! tworzy elementy z danymi EMG
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje
    void createEMGBranch( QTreeWidgetItem* root, const std::vector<PluginSubject::SessionConstPtr>& sessions );
    //! tworzy elementy z danymi kinetycznymi
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje
    void createKineticBranch( QTreeWidgetItem* root, const std::vector<PluginSubject::SessionConstPtr>& sessions );
    //! tworzy elementy z danymi kątów
    //! \param root korzeń, do którego zostaną podpiete elementy
    //! \param sessions filtrowane sesje 
    void createAngleBranch( QTreeWidgetItem* root, const std::vector<PluginSubject::SessionConstPtr>& sessions );
};
#endif
