/********************************************************************
    created:  2011/11/08
    created:  8:11:2011   17:12
    filename: Patient.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___PATIENT_H__
#define HEADER_GUARD___PATIENT_H__

#include <plugins/communication/IPatient.h>

class Patient : public IPatient
{
public:

    Patient(PluginSubject::SubjectID id, const std::string & name, const std::string & surname, const std::string & birthday,
        char gender, const core::shared_ptr<const QPixmap> & photo, const std::vector<Disorder> & disorders);

    virtual ~Patient();

    virtual PluginSubject::SubjectID getID() const;
    virtual const std::string & getName() const;
    virtual const std::string & getSurname() const;
    virtual const std::string & getBirthday() const;
    virtual char getGender() const;
    virtual const core::shared_ptr<const QPixmap> & getPhoto() const;

    virtual unsigned int disordersCount() const;
    virtual const Disorder & disorder(unsigned int idx) const;

private:
    PluginSubject::SubjectID id;
    std::string name;
    std::string surname;
    std::string birthday;
    char gender;
    core::shared_ptr<const QPixmap> photo;
    std::vector<Disorder> disorders;
};

#endif HEADER_GUARD___PATIENT_H__
