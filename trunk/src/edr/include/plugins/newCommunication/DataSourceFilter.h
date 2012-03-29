/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   11:15
    filename: IDataSourceFilter.h
    author:   Mateusz Janiak
    
    purpose:  Interfejs filtrów danych
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEFILTER_H__
#define HEADER_GUARD___DATASOURCEFILTER_H__

#include <webserviceslib/Entity.h>

//! Interfejs filtrów danych
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
    //! \return true je¿eli schorzenie pomyœlnie przesz³o weryfikacjê
    virtual bool filterDisorder(const webservices::MedicalShallowCopy::Disorder* disorder) const
    {
        return true;
    }
    //! Pacjent uzale¿niony jest od weryfikacji subjecta!!
    //! \param patient Pacjent do weryfikacji
    //! \return true je¿eli pacjent pomyœlnie przeszed³ weryfikacjê
    virtual bool filterPatient(const webservices::MedicalShallowCopy::Patient* patient) const
    {
        return true;
    }

    //! \param subject Subject do weryfikacji
    //! \return true je¿eli subject pomyœlnie przeszed³ weryfikacjê
    virtual bool filterSubject(const webservices::MotionShallowCopy::Performer* subject) const
    {
        return true;
    }
    //! \param session Sesja do weryfikacji
    //! \return true je¿eli sesja pomyœlnie przesz³a weryfikacjê
    virtual bool filterSession(const webservices::MotionShallowCopy::Session* session) const
    {
        return true;
    }
    //! \param motion Motion do weryfikacji
    //! \return true je¿eli motion pomyœlnie przeszed³ weryfikacjê
    virtual bool filterMotion(const webservices::MotionShallowCopy::Trial* motion) const
    {
        return true;
    }
    //! \param file Plik do weryfikacji
    //! \return true je¿eli plik pomyœlnie przeszed³ weryfikacjê
    virtual bool filterFile(const webservices::MotionShallowCopy::File* file) const
    {
        return true;
    }
};

#endif	//	HEADER_GUARD___DATASOURCEFILTER_H__
