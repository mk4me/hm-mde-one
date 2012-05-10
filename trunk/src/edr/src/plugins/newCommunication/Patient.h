/********************************************************************
    created:  2011/11/08
    created:  8:11:2011   17:12
    filename: Patient.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___PATIENT_H__
#define HEADER_GUARD___PATIENT_H__

#include <plugins/newCommunication/IPatient.h>

class Patient : public communication::IPatient
{
public:

    Patient(PluginSubject::SubjectID id, const std::string & name, const std::string & surname, const std::string & birthday,
		Gender gender, const core::shared_ptr<const QPixmap> & photo, const std::vector<communication::Disorder> & disorders);

    virtual ~Patient();

    virtual PluginSubject::SubjectID getID() const;

    virtual const std::string & getName() const;
    virtual const std::string & getSurname() const;
    virtual const std::string & getBirthday() const;
    virtual Gender getGender() const;
    virtual const core::shared_ptr<const QPixmap> & getPhoto() const;

	void setName(const std::string & name);
	void setSurname(const std::string & surname);
	void setBirthday(const std::string & birthday);
	void setGender(Gender gender);
	void setPhoto(const core::shared_ptr<const QPixmap> & photo);

    virtual unsigned int disordersCount() const;
    virtual const communication::Disorder & disorder(unsigned int idx) const;

	static Gender decodeGender(char g);

private:
    PluginSubject::SubjectID id;
	PluginSubject::SubjectPtr subject;
	PluginSubject::SubjectConstPtr constSubject;
    std::string name;
    std::string surname;
    std::string birthday;
    Gender gender;
    core::shared_ptr<const QPixmap> photo;
    std::vector<communication::Disorder> disorders;
};

#endif HEADER_GUARD___PATIENT_H__