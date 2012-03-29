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

//! Klasa pomagaj¹ca tworzyæ/zarz¹dzaæ p³ytkimi kopiami danych
class DataSourceShallowCopyUtils
{
public:

    //! \param shallowCopy P³ytka kopia bazy danych do wype³nienia
    //! \param motionShallowCopy Œcie¿ka do zapisu pliku danych p³ytkiej kopi danych ruchu
    //! \param motionMetadata Œcie¿ka do zapisu pliku metadanych p³ytkiej kopi danych ruchu
    //! \param medicalShallowCopy Œcie¿ka do zapisu pliku danych p³ytkiej kopi danych medycznych
    //! \param medicalMetadata Œcie¿ka do zapisu pliku metadanych p³ytkiej kopi danych medycznych
    static void createShallowCopy(communication::ShallowCopy & shallowCopy, const core::Filesystem::Path & motionShallowCopy,
        const core::Filesystem::Path & motionMetadata, const core::Filesystem::Path & medicalShallowCopy,
        const core::Filesystem::Path & medicalMetadata);

    //! \param p³ytka kopia bazy danych
    //! \return Czy p³ytka kopia bazy danych jest spójna i poprawna
    static bool checkShallowCopyIntegrity(const communication::ShallowCopy & shallowCopy);

    //! \param P³ytka kopia bazy danych
    //! \return Czy p³ytka kopia bazy danych wymaga odœwie¿enia (œci¹gniêcia nowych plików
    static bool shallowCopyRequiresRefresh(const communication::ShallowCopy & shallowCopy, webservices::DateTime & currentVersion);
};

#endif	//	HEADER_GUARD___DATASOURCESHALLOWCOPYUTILS_H__
