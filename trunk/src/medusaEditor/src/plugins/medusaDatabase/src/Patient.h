/********************************************************************
    created:  2011/11/08
    created:  8:11:2011   17:12
    filename: Patient.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_medusaDatabase__PATIENT_H__
#define HEADER_GUARD_medusaDatabase__PATIENT_H__

#include <plugins/medusaDatabase/IPatient.h>

namespace medusaDatabase
{

	class Patient : public medusaDatabase::IPatient
	{
	public:

		Patient(PluginSubject::SubjectID id, const std::string & name, const std::string & surname, const std::string & birthday,
			Gender gender, const utils::shared_ptr<const QPixmap> & photo, const std::vector<medusaDatabase::Disorder> & disorders);

		virtual ~Patient();

		virtual PluginSubject::SubjectID getID() const;

		virtual const std::string & getName() const;
		virtual const std::string & getSurname() const;
		virtual const std::string & getBirthday() const;
		virtual Gender getGender() const;
		virtual const utils::shared_ptr<const QPixmap> & getPhoto() const;

		void setName(const std::string & name);
		void setSurname(const std::string & surname);
		void setBirthday(const std::string & birthday);
		void setGender(Gender gender);
		void setPhoto(const utils::shared_ptr<const QPixmap> & photo);

		virtual unsigned int disordersCount() const;
		virtual const medusaDatabase::Disorder & disorder(unsigned int idx) const;

		static Gender decodeGender(char g);

	private:
		PluginSubject::SubjectID id;
		PluginSubject::SubjectPtr subject;
		PluginSubject::SubjectConstPtr constSubject;
		std::string name;
		std::string surname;
		std::string birthday;
		Gender gender;
		utils::shared_ptr<const QPixmap> photo;
		std::vector<medusaDatabase::Disorder> disorders;
	};

}

#endif HEADER_GUARD_medusaDatabase__PATIENT_H__
