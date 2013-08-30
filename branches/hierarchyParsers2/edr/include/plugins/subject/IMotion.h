/********************************************************************
    created:  2011/11/03
    created:  3:11:2011   10:02
    filename: IMotion.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___IMOTION_H__
#define HEADER_GUARD___IMOTION_H__

#include <stdexcept>
#include <plugins/subject/Types.h>
#include <corelib/BaseDataTypes.h>
#include <plugins/subject/ISession.h>
#include <corelib/IMemoryDataManager.h>

namespace PluginSubject{

//! Abstrakcyjna klasa reprezentująca pojedynczy ruch obiektu w ramach sesji
class IMotion : public core::IDataManagerReaderOperations
{
public:

    virtual ~IMotion() {}

    //! \return Globalny, unikalny ID ruchu
    virtual SubjectID getID() const = 0;
    //! \return Lokalny, unikalny ID w ramach sesji
    virtual SubjectID getLocalID() const = 0;
    //! \return Sesja z którą związany jest ten ruch
    virtual const core::ObjectWrapperConstPtr & getSession() const = 0;
	virtual const SessionConstPtr & getUnpackedSession() const = 0;

    //! \return Globalna nazwa ruchu
    virtual const std::string & getName() const = 0;
    //! \return Lokalna nazwa ruchu w ramach sesji
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

DEFINE_WRAPPER(PluginSubject::IMotion, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif //   HEADER_GUARD___IMOTION_H__
