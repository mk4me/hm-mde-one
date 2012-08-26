/********************************************************************
    created:  2011/11/03
    created:  3:11:2011   10:06
    filename: ISubject.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISUBJECT_H__
#define HEADER_GUARD___ISUBJECT_H__

#include <plugins/subject/Types.h>
#include <core/ObjectWrapper.h>

namespace PluginSubject {

//! Abstrakcyjna klasa reprezentująca obiekt wykonujący ruch - człowiek, zwierzę, avatar

class ISubject
{
public:

    virtual ~ISubject() {}

    //! \return Unikalny identyfikator obiektu. Po usunięciu i ponownym dodaniu zostanie przyznany nowy!!
    virtual SubjectID getID() const = 0;
    //! \return "Przyjazna" lub "naturalna" nazwa obiektu.
    virtual const std::string & getName() const = 0;
    //! \param sessions Lista sesji związanych z tym obiektem - dynamicznie pobierana z DM!!
    virtual void getSessions(Sessions & sessions) const = 0;
};

}

CORE_DEFINE_WRAPPER(PluginSubject::ISubject, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif //   HEADER_GUARD___ISUBJECT_H__

