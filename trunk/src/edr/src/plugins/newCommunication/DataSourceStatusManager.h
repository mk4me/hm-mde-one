/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   11:32
    filename: DataSourceStatusManager.h
    author:   Mateusz Janiak
    
    purpose:  Klasa udostepniaj¹ca informacje o statusie podstawowych elementów
              Dodatkowo umozliwa ich odœwie¿anie.
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCESTATUSMANAGER_H__
#define HEADER_GUARD___DATASOURCESTATUSMANAGER_H__

#include <map>
#include <set>

#include <plugins/newCommunication/IDataSourceStatusManager.h>
#include <core/IDataManager.h>

#include <plugins/newCommunication/DataSourceShallowCopy.h>
#include "DataSourceStatus.h"
#include "DataSourceLocalStorage.h"
#include <QtGui/QIcon>

//! Kalsa zarz¹dzaj¹ca statusem plików. Ma charakter singletona
class FileStatusManager
{
private:

    //! Struktura na u¿ytek wewnêtrzny opisuj¹ca ka¿dy plik
    struct FileStatus{
        //! Identyfikator pliku
        int fileID;
        //! Œcie¿ka pliku
        core::Filesystem::Path filePath;
        //! Status pliku
        DataStatus fileStatus;
    };

    //! Typ kolekcji plików
    typedef std::map<int, FileStatus> FilesStatus;

public:
    //! Prywatny konstruktor
    //! \param fileDataManager Manager plików w oparciu o którego bêd¹ sprawdzane przypadki u¿ycia plików
    FileStatusManager(const core::IFileDataManager * fileDataManager);
    //! Destruktor
    ~FileStatusManager();

    //! \param fileID Identyfikator pliku
    //! \param path Œcie¿ka do pliku
    //! \param status Status pliku
    void addFile(int fileID, const core::Filesystem::Path & path, const DataStatus & status = DataStatus(communication::Remote, communication::Unloaded));

    //! \param fileID Identyfikator pliku
    void removeFile(int fileID);
	//! Usuwa informacje o wszysktich plikach
	void removeAllFiles();

	//! \param files [out] Pliki obs³ugiwane przez FileStatusManager
	void managedFiles(std::set<int> & files) const;

	//! \param fileID Identyfikator pliku
	//! \return Czy plik jest pod kontrol¹ managera
	bool fileExists(int fileID);
    
    //! \param fileID Identyfikator pliku
    //! \param status Status pliku
    void setFileStatus(int fileID, const DataStatus & status);
    //! \param fileID Identyfikator pliku
    //! \param usage Stan u¿ycia pliku
    void setFileUsage(int fileID, communication::DataUsage usage);
    //! \param fileID Identyfikator pliku
    //! \param storage Sposób przechowywania pliku
    void setFileStorage(int fileID, communication::DataStorage storage);
	//! \param fileID Identyfikator pliku
	//! \param path Œcie¿ka do pliku
	void setFilePath(int fileID, const core::Filesystem::Path & path);

    //! \param fileID Identyfikator pliku
    //! \return Œcie¿ka do pliku
    const core::Filesystem::Path & filePath(int fileID) const;
    //! \param fileID Identyfikator pliku
    //! \return Status pliku
    const DataStatus & fileStatus(int fileID) const;
    //! \param fileID Identyfikator pliku
    //! \return Sposób u¿ycia pliku
    const communication::DataUsage fileUsage(int fileID) const;
    //! \param fileID Identyfikator pliku
    //! \return Sposób przechowywania pliku
    const communication::DataStorage fileStorage(int fileID) const;

    //! Odœwie¿a status wszystkich plików
    void refreshFilesStatus();

    //! Odœwie¿a status zadanych plików
    void refreshFilesStatus(const std::set<int> & files);

    //! Odœwie¿a status zadanego pliku
    //! \param fileID Identyfikator pliku
    void refreshFileStatus(int fileID);

private:
    //! \param fileStatus Opis statusu pliku który aktualizujemy
    void refreshFileStatus(FileStatus & fileStatus);

private:

    //! Informacje o statusie plików
    FilesStatus filesStatus;
	std::set<int> files;
    //! Manager plików do weryfikacji ich statusów
    const core::IFileDataManager * fileDataManager;
};

//! Klasa udostêpniaj¹ca statusy danych na ró¿nych poziomach hierarchii
class DataSourceStatusManager : public communication::IDataSourceStatusManager
{
public:
    //Konstruktor domyœlny
    DataSourceStatusManager(const FileStatusManager * fileStatusManager);
    //! Destruktor wirtualny
    virtual ~DataSourceStatusManager();

    //! \param shallowCopy P³ytka kopia bazy danych na bazie ktorej bêd¹ tworzone statusy (odtworzona ich hierarchia)
    void setShallowCopy(const communication::ShallowCopy * shallowCopy);

    //! \param modifiedFilesStatus Zbiór plików które zmieni³y swój status. Ich nowy status dostaniemy poprzez obiekt FileStatusManager)
    void refreshDataStatus(const std::set<int> & modifiedFilesStatus);

    //! \param disorderID Identyfikator schorzenia którego status chcemy pobraæ
    //! \return Status schorzenia
    virtual const DataStatus & disorderStatus(int disorderID) const;
    //! \param patientID Identyfikator pacjenta którego status chcemy pobraæ
    //! \return Status pacjenta
    virtual const DataStatus & patientStatus(int patientID) const;
    //! \param subjectID Identyfikator subjecta którego status chcemy pobraæ
    //! \return Status subjecta
    virtual const DataStatus & subjectStatus(int subjectID) const;
    //! \param sessionID Identyfikator sesji której status chcemy pobraæ
    //! \return Status sesji
    virtual const DataStatus & sessionStatus(int sessionID) const;
    //! \param motionID Identyfikator motiona którego status chcemy pobraæ
    //! \return Status motiona
    virtual const DataStatus & motionStatus(int motionID) const;
    //! \param fileID Identyfikator pliku którego status chcemy pobraæ
    //! \return Status pliku
    virtual const DataStatus & fileStatus(int fileID) const;
	//! \param status Status danych dla którego chcemy pobrac ikonê
	//! \return Ikona dla danego statusu
	static QIcon statusIcon(const communication::IDataStatus & status);
	//! \param status Status danych dla którego chcemy pobrac ikonê
	//! \param icon Ikona dla danego statusu
	static void setStatusIcon(const communication::IDataStatus & status, const QIcon & icon);

protected:
    //! Aktualizuje ca³y status danych - u¿ycie i dostepnoœæ
    void rebuildDataStatus();

    //! Czyœci informacje o statusach wszystkich znanych obiektów
    void clearAllStatusData();

private:
    //! Mapa statusów schorzeñ
    std::map<int, DataStatus> disordersStatus;
    //! Mapa statusów pacjentów
    std::map<int, DataStatus> patientsStatus;
    //! Mapa statusów subjectów
    std::map<int, DataStatus> subjectsStatus;
    //! Mapa statusów sesji
    std::map<int, DataStatus> sessionsStatus;
    //! Mapa statusów motionów
    std::map<int, DataStatus> motionsStatus;
    //! P³ytka kopia dla której generujemy statusy
    const communication::ShallowCopy * shallowCopy;
	//! Manager statusu plików
	const FileStatusManager * fileStatusManager;
	//! Ikony statusów
	static std::map<communication::DataStorage, std::map<communication::DataUsage, QIcon>> statusIcons;
};

#endif	//	HEADER_GUARD___DATASOURCESTATUSMANAGER_H__
