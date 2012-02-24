#include "CommunicationPCH.h"
#include "Patient.h"

Patient::Patient(PluginSubject::SubjectID id, const std::string & name, const std::string & surname, const std::string & birthday,
    char gender, const core::shared_ptr<const QPixmap> & photo, const std::vector<Disorder> & disorders)
    :id(id), name(name), surname(surname), birthday(birthday), gender(gender), photo(photo), disorders(disorders)
{

}

Patient::~Patient()
{

}

PluginSubject::SubjectID Patient::getID() const
{
    return id;
}

const std::string & Patient::getName() const
{
    return name;
}

const std::string & Patient::getSurname() const
{
    return surname;
}

const std::string & Patient::getBirthday() const
{
    return birthday;
}

char Patient::getGender() const
{
    return gender;
}

const core::shared_ptr<const QPixmap> & Patient::getPhoto() const
{
    return photo;
}

unsigned int Patient::disordersCount() const
{
    return disorders.size();
}

const Disorder & Patient::disorder(unsigned int idx) const
{
    return disorders[idx];
}