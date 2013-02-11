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
#include <corelib/BaseDataTypes.h>
#include <corelib/IMemoryDataManager.h>

namespace PluginSubject {

//! Abstrakcyjna klasa reprezentująca zbiór ruchów w ramach jednego celu/badania/grupy
class ISession : public core::IDataManagerReaderOperations
{
public:
    virtual ~ISession() {}

    //! \return Unikalne, globalne ID sesji. Po usunięci i dodaniu może być inne!
    virtual SubjectID getID() const = 0;
    //! \return Unikalne, lokalne ID sesji w obrębie sesji tego samego obiektu wykonującego ruch. Podobnie jak globalne - po usunięci i ponownym dodaniu ID może ulec zmianie!
    virtual SubjectID getLocalID() const = 0;
    //! \param motions Zbiór wypełniany konkretnymi ruchami danego obiektu w ramach aktualnej sesji
    virtual void getMotions(core::ConstObjectsList & motions) const = 0;

	virtual void addMotion(core::ObjectWrapperConstPtr & motion) = 0;
	virtual void removeMotion(core::ObjectWrapperConstPtr & motion) = 0;

    //! \return Obiekt z którym związana jest ta sesja
    virtual const core::ObjectWrapperConstPtr & getSubject() const = 0;
	virtual const SubjectConstPtr & getUnpackedSubject() const = 0;

    //! \return Globalna nazwa sesji
    virtual const std::string & getName() const = 0;
    //! \return Lokalna nazwa sesji
    virtual const std::string & getLocalName() const = 0;

	//! \data Dane wchodzące pod kontrolę DM
	virtual void addData(const core::ObjectWrapperConstPtr & data) = 0;
	//! Dane usuwane z DM
	virtual void removeData(const core::ObjectWrapperConstPtr & data) = 0;

	virtual const bool tryAddData(const core::ObjectWrapperConstPtr & data) = 0;

	virtual const bool tryRemoveData(const core::ObjectWrapperConstPtr & data) = 0;

	virtual void getObjects(core::ConstObjectsList & objects) const = 0;

	virtual void getObjects(core::ConstObjectsList & objects, const core::TypeInfo & type, bool exact) const = 0;

	virtual void getObjects(core::ObjectWrapperCollection& objects) const = 0;

	virtual const bool isManaged(const core::ObjectWrapperConstPtr & object) const = 0;

	virtual const bool hasObject(const core::TypeInfo & type, bool exact) const = 0;
};

}

DEFINE_WRAPPER(PluginSubject::ISession, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif //   HEADER_GUARD___ISESSION_H__
