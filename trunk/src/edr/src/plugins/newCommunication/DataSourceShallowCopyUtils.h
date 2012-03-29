/********************************************************************
    created:  2012/03/08
    created:  8:3:2012   11:09
    filename: DataSourceShallowCopyUtils.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCESHALLOWCOPYUTILS_H__
#define HEADER_GUARD___DATASOURCESHALLOWCOPYUTILS_H__

#include <plugins/newCommunication/DataSourceShallowCopy.h>
#include <core/Filesystem.h>

//! Klasa pomagaj�ca tworzy�/zarz�dza� p�ytkimi kopiami danych
class DataSourceShallowCopyUtils
{
public:

    //! \param shallowCopy P�ytka kopia bazy danych do wype�nienia
    //! \param motionShallowCopy �cie�ka do zapisu pliku danych p�ytkiej kopi danych ruchu
    //! \param motionMetadata �cie�ka do zapisu pliku metadanych p�ytkiej kopi danych ruchu
    //! \param medicalShallowCopy �cie�ka do zapisu pliku danych p�ytkiej kopi danych medycznych
    //! \param medicalMetadata �cie�ka do zapisu pliku metadanych p�ytkiej kopi danych medycznych
    static void createShallowCopy(communication::ShallowCopy & shallowCopy, const core::Filesystem::Path & motionShallowCopy,
        const core::Filesystem::Path & motionMetadata, const core::Filesystem::Path & medicalShallowCopy,
        const core::Filesystem::Path & medicalMetadata);

    //! \param p�ytka kopia bazy danych
    //! \return Czy p�ytka kopia bazy danych jest sp�jna i poprawna
    static bool checkShallowCopyIntegrity(const communication::ShallowCopy & shallowCopy);

    //! \param P�ytka kopia bazy danych
    //! \return Czy p�ytka kopia bazy danych wymaga od�wie�enia (�ci�gni�cia nowych plik�w
    static bool shallowCopyRequiresRefresh(const communication::ShallowCopy & shallowCopy, webservices::DateTime & currentVersion);
};

#endif	//	HEADER_GUARD___DATASOURCESHALLOWCOPYUTILS_H__
