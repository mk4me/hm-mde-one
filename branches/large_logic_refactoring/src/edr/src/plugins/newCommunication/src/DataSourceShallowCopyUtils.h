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
#include <corelib/Filesystem.h>

//! Klasa pomagająca tworzyć/zarządzać płytkimi kopiami danych
class DataSourceShallowCopyUtils
{
public:

    //! \param shallowCopy Płytka kopia bazy danych do wypełnienia
    //! \param motionShallowCopy Ścieżka do zapisu pliku danych płytkiej kopi danych ruchu
    //! \param motionMetadata Ścieżka do zapisu pliku metadanych płytkiej kopi danych ruchu
    //! \param medicalShallowCopy Ścieżka do zapisu pliku danych płytkiej kopi danych medycznych
    //! \param medicalMetadata Ścieżka do zapisu pliku metadanych płytkiej kopi danych medycznych
    static void createShallowCopy(communication::ShallowCopy & shallowCopy, const core::Filesystem::Path & motionShallowCopy,
        const core::Filesystem::Path & motionMetadata, const core::Filesystem::Path & medicalShallowCopy,
        const core::Filesystem::Path & medicalMetadata);

    //! \param płytka kopia bazy danych
    //! \return Czy płytka kopia bazy danych jest spójna i poprawna
    static bool checkShallowCopyIntegrity(const communication::ShallowCopy & shallowCopy);

    //! \param Płytka kopia bazy danych
    //! \return Czy płytka kopia bazy danych wymaga odświeżenia (ściągnięcia nowych plików
    static bool shallowCopyRequiresRefresh(const communication::ShallowCopy & shallowCopy, webservices::DateTime & currentVersion);
};

#endif	//	HEADER_GUARD___DATASOURCESHALLOWCOPYUTILS_H__
