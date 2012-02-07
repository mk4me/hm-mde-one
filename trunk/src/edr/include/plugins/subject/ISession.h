/********************************************************************
    created:  2011/11/03
    created:  3:11:2011   10:04
    filename: ISession.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISESSION_H__
#define HEADER_GUARD___ISESSION_H__

#include <plugins/subject/Types.h>
#include <core/ObjectWrapper.h>

//! Abstrakcyjna klasa reprezentuj�ca zbi�r ruch�w w ramach jednego celu/badania/grupy
class ISession
{
public:
    virtual ~ISession() {}

    //! \return Unikalne, globalne ID sesji. Po usuni�ci i dodaniu mo�e by� inne!
    virtual SubjectID getID() const = 0;
    //! \return Unikalne, lokalne ID sesji w obr�bie sesji tego samego obiektu wykonuj�cego ruch. Podobnie jak globalne - po usuni�ci i ponownym dodaniu ID mo�e ulec zmianie!
    virtual SubjectID getLocalID() const = 0;
    //! \param motions Zbi�r wype�niany konkretnymi ruchami danego obiektu w ramach aktualnej sesji
    virtual void getMotions(Motions & motions) const = 0;

    //! \return Obiekt z kt�rym zwi�zana jest ta sesja
    virtual const SubjectConstPtr & getSubject() const = 0;

    //! \return Globalna nazwa sesji
    virtual const std::string & getName() const = 0;
    //! \return Lokalna nazwa sesji
    virtual const std::string & getLocalName() const = 0;

    //! \return Rok wykonania sesji
    virtual unsigned int getYear() const = 0;
    //! \return Miesi�c wykonania sesji
    virtual unsigned char getMonth() const = 0;
    //! \return Dzie� wykonania sesji
    virtual unsigned char getDay() const = 0;

    //! \param wrappers Zbi�r OW zwi�zanych z t� sesj� (ale nie z Motions)
    virtual void getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const = 0;

    virtual core::ObjectWrapperConstPtr getWrapperOfType(const core::TypeInfo& type, bool exact = false) const
    {
        std::vector<core::ObjectWrapperConstPtr> wrappers;
        getWrappers(wrappers);
        for(auto it = wrappers.begin(); it != wrappers.end(); it++){
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

        for(auto it = wrappers.begin(); it != wrappers.end(); it++){
            if((*it)->getTypeInfo() == type || (exact == false && (*it)->isSupported(type))){
                return true;
            }
        }

        return false;
    }
};

CORE_DEFINE_WRAPPER(ISession, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif //   HEADER_GUARD___ISESSION_H__