/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   11:15
    filename: IDataSourceFilter.h
    author:   Mateusz Janiak
    
    purpose:  Interfejs filtr�w danych
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEFILTER_H__
#define HEADER_GUARD___DATASOURCEFILTER_H__

#include <webserviceslib/Entity.h>

//! Interfejs filtr�w danych
class DataSourceFilter
{
public:
    
    //! Wirtualny destruktor
    virtual ~DataSourceFilter() {}

    //! \return Nazwa filtra
    virtual std::string name() const
	{
		return "Empty filter";
	}

    //! \param disorder Schorzenie do weryfikacji
    //! \return true je�eli schorzenie pomy�lnie przesz�o weryfikacj�
    virtual bool filterDisorder(const webservices::MedicalShallowCopy::Disorder* disorder) const
    {
        return true;
    }
    //! Pacjent uzale�niony jest od weryfikacji subjecta!!
    //! \param patient Pacjent do weryfikacji
    //! \return true je�eli pacjent pomy�lnie przeszed� weryfikacj�
    virtual bool filterPatient(const webservices::MedicalShallowCopy::Patient* patient) const
    {
        return true;
    }

    //! \param subject Subject do weryfikacji
    //! \return true je�eli subject pomy�lnie przeszed� weryfikacj�
    virtual bool filterSubject(const webservices::MotionShallowCopy::Performer* subject) const
    {
        return true;
    }
    //! \param session Sesja do weryfikacji
    //! \return true je�eli sesja pomy�lnie przesz�a weryfikacj�
    virtual bool filterSession(const webservices::MotionShallowCopy::Session* session) const
    {
        return true;
    }
    //! \param motion Motion do weryfikacji
    //! \return true je�eli motion pomy�lnie przeszed� weryfikacj�
    virtual bool filterMotion(const webservices::MotionShallowCopy::Trial* motion) const
    {
        return true;
    }
    //! \param file Plik do weryfikacji
    //! \return true je�eli plik pomy�lnie przeszed� weryfikacj�
    virtual bool filterFile(const webservices::MotionShallowCopy::File* file) const
    {
        return true;
    }
};

#endif	//	HEADER_GUARD___DATASOURCEFILTER_H__
