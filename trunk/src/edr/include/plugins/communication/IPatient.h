/********************************************************************
    created:  2011/10/28
    created:  28:10:2011   14:10
    filename: IPatient.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IPATIENT_H__
#define HEADER_GUARD___IPATIENT_H__

#include <QtGui/QPixmap>
#include <string>
#include <plugins/subject/ISubject.h>
#include <core/SmartPtr.h>

struct Disorder
{
    std::string name;
    std::string diagnosisDate;
    std::string focus;
    std::string comments;
    std::map<std::string, std::string> attributes;
};

struct AntropometricData
{

};

class IPatient
{
public:
    virtual ~IPatient() {};

    virtual SubjectID getID() const = 0;
    virtual const std::string & getName() const = 0;
    virtual const std::string & getSurname() const = 0;
    virtual const std::string & getBirthday() const = 0;
    virtual char getGender() const = 0;
    virtual const core::shared_ptr<const QPixmap> & getPhoto() const = 0;

    virtual unsigned int disordersCount() const = 0;
    virtual const Disorder & disorder(unsigned int idx) const = 0;
};

typedef core::shared_ptr<IPatient> PatientPtr;
typedef core::shared_ptr<const IPatient> PatientConstPtr;

CORE_DEFINE_WRAPPER(IPatient, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif HEADER_GUARD___IPATIENT_H__