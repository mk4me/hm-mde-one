/********************************************************************
    created:  2011/11/03
    created:  3:11:2011   10:04
    filename: ISession.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___ISESSION_H__
#define HEADER_GUARD___ISESSION_H__

#include <stdexcept>
#include <plugins/subject/Types.h>
#include <core/ObjectWrapper.h>

namespace PluginSubject {

//! Abstrakcyjna klasa reprezentująca zbiór ruchów w ramach jednego celu/badania/grupy
class ISession
{
public:
    virtual ~ISession() {}

    //! \return Unikalne, globalne ID sesji. Po usunięci i dodaniu może być inne!
    virtual SubjectID getID() const = 0;
    //! \return Unikalne, lokalne ID sesji w obrębie sesji tego samego obiektu wykonującego ruch. Podobnie jak globalne - po usunięci i ponownym dodaniu ID może ulec zmianie!
    virtual SubjectID getLocalID() const = 0;
    //! \param motions Zbiór wypełniany konkretnymi ruchami danego obiektu w ramach aktualnej sesji
    virtual void getMotions(Motions & motions) const = 0;

    //! \return Obiekt z którym związana jest ta sesja
    virtual const SubjectConstPtr & getSubject() const = 0;

    //! \return Globalna nazwa sesji
    virtual const std::string & getName() const = 0;
    //! \return Lokalna nazwa sesji
    virtual const std::string & getLocalName() const = 0;

    //! \param wrappers Zbiór OW związanych z tą sesją (ale nie z Motions)
    virtual void getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const = 0;

    virtual core::ObjectWrapperConstPtr getWrapperOfType(const core::TypeInfo& type, bool exact = false) const
    {
        std::vector<core::ObjectWrapperConstPtr> wrappers;
        getWrappers(wrappers);
        for(auto it = wrappers.begin(); it != wrappers.end(); ++it){
            if((*it)->getTypeInfo() == type || (exact == false && (*it)->isSupported(type))){
                return (*it);
            }
        }

        //throw std::runtime_error("Object type not stored in Session");
        return core::ObjectWrapperConstPtr();
    }

    virtual bool hasObjectOfType(const core::TypeInfo& type, bool exact = false) const
    {
        std::vector<core::ObjectWrapperConstPtr> wrappers;
        getWrappers(wrappers);

        for(auto it = wrappers.begin(); it != wrappers.end(); ++it){
            if((*it)->getTypeInfo() == type || (exact == false && (*it)->isSupported(type))){
                return true;
            }
        }

        return false;
    }
};

}

CORE_DEFINE_WRAPPER(PluginSubject::ISession, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif //   HEADER_GUARD___ISESSION_H__
