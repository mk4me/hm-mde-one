/********************************************************************
	created:  2012/02/04
	created:  4:2:2012   11:15
	filename: IDataSourceFilter.h
	author:   Mateusz Janiak

	purpose:  Interfejs filtrów danych
	*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEFILTER_H__
#define HEADER_GUARD___DATASOURCEFILTER_H__

#include <hmdbserviceslib/ShallowCopy.h>

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
	//! \return true jeżeli schorzenie pomyślnie przeszło weryfikację
	virtual bool filterDisorder(const hmdbServices::MedicalShallowCopy::Disorder* disorder) const
	{
		return true;
	}
	//! Pacjent uzależniony jest od weryfikacji subjecta!!
	//! \param patient Pacjent do weryfikacji
	//! \return true jeżeli pacjent pomyślnie przeszedł weryfikację
	virtual bool filterPatient(const hmdbServices::MedicalShallowCopy::Patient* patient) const
	{
		return true;
	}

	//! \param subject Subject do weryfikacji
	//! \return true jeżeli subject pomyślnie przeszedł weryfikację
	virtual bool filterSubject(const hmdbServices::MotionShallowCopy::Performer* subject) const
	{
		return true;
	}
	//! \param session Sesja do weryfikacji
	//! \return true jeżeli sesja pomyślnie przeszła weryfikację
	virtual bool filterSession(const hmdbServices::MotionShallowCopy::Session* session) const
	{
		return true;
	}
	//! \param motion Motion do weryfikacji
	//! \return true jeżeli motion pomyślnie przeszedł weryfikację
	virtual bool filterMotion(const hmdbServices::MotionShallowCopy::Trial* motion) const
	{
		return true;
	}
	//! \param file Plik do weryfikacji
	//! \return true jeżeli plik pomyślnie przeszedł weryfikację
	virtual bool filterFile(const hmdbServices::MotionShallowCopy::File* file) const
	{
		return true;
	}
};

#endif	//	HEADER_GUARD___DATASOURCEFILTER_H__
