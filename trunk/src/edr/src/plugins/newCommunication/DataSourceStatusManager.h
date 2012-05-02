/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   11:32
    filename: DataSourceStatusManager.h
    author:   Mateusz Janiak
    
    purpose:  Klasa udostepniaj�ca informacje o statusie podstawowych element�w
              Dodatkowo umozliwa ich od�wie�anie.
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

//! Kalsa zarz�dzaj�ca statusem plik�w. Ma charakter singletona
class FileStatusManager
{
private:

    //! Struktura na u�ytek wewn�trzny opisuj�ca ka�dy plik
    struct FileStatus{
        //! Identyfikator pliku
        int fileID;
        //! �cie�ka pliku
        core::Filesystem::Path filePath;
        //! Status pliku
        DataStatus fileStatus;
    };

    //! Typ kolekcji plik�w
    typedef std::map<int, FileStatus> FilesStatus;

public:
    //! Prywatny konstruktor
    //! \param fileDataManager Manager plik�w w oparciu o kt�rego b�d� sprawdzane przypadki u�ycia plik�w
    FileStatusManager(const core::IFileDataManager * fileDataManager);
    //! Destruktor
    ~FileStatusManager();

    //! \param fileID Identyfikator pliku
    //! \param path �cie�ka do pliku
    //! \param status Status pliku
    void addFile(int fileID, const core::Filesystem::Path & path, const DataStatus & status = DataStatus(communication::Remote, communication::Unloaded));

    //! \param fileID Identyfikator pliku
    void removeFile(int fileID);
	//! Usuwa informacje o wszysktich plikach
	void removeAllFiles();

	//! \param files [out] Pliki obs�ugiwane przez FileStatusManager
	void managedFiles(std::set<int> & files) const;

	//! \param fileID Identyfikator pliku
	//! \return Czy plik jest pod kontrol� managera
	bool fileExists(int fileID);
    
    //! \param fileID Identyfikator pliku
    //! \param status Status pliku
    void setFileStatus(int fileID, const DataStatus & status);
    //! \param fileID Identyfikator pliku
    //! \param usage Stan u�ycia pliku
    void setFileUsage(int fileID, communication::DataUsage usage);
    //! \param fileID Identyfikator pliku
    //! \param storage Spos�b przechowywania pliku
    void setFileStorage(int fileID, communication::DataStorage storage);
	//! \param fileID Identyfikator pliku
	//! \param path �cie�ka do pliku
	void setFilePath(int fileID, const core::Filesystem::Path & path);

    //! \param fileID Identyfikator pliku
    //! \return �cie�ka do pliku
    const core::Filesystem::Path & filePath(int fileID) const;
    //! \param fileID Identyfikator pliku
    //! \return Status pliku
    const DataStatus & fileStatus(int fileID) const;
    //! \param fileID Identyfikator pliku
    //! \return Spos�b u�ycia pliku
    const communication::DataUsage fileUsage(int fileID) const;
    //! \param fileID Identyfikator pliku
    //! \return Spos�b przechowywania pliku
    const communication::DataStorage fileStorage(int fileID) const;

    //! Od�wie�a status wszystkich plik�w
    void refreshFilesStatus();

    //! Od�wie�a status zadanych plik�w
    void refreshFilesStatus(const std::set<int> & files);

    //! Od�wie�a status zadanego pliku
    //! \param fileID Identyfikator pliku
    void refreshFileStatus(int fileID);

private:
    //! \param fileStatus Opis statusu pliku kt�ry aktualizujemy
    void refreshFileStatus(FileStatus & fileStatus);

private:

    //! Informacje o statusie plik�w
    FilesStatus filesStatus;
	std::set<int> files;
    //! Manager plik�w do weryfikacji ich status�w
    const core::IFileDataManager * fileDataManager;
};

//! Klasa udost�pniaj�ca statusy danych na r�nych poziomach hierarchii
class DataSourceStatusManager : public communication::IDataSourceStatusManager
{
public:
    //Konstruktor domy�lny
    DataSourceStatusManager(const FileStatusManager * fileStatusManager);
    //! Destruktor wirtualny
    virtual ~DataSourceStatusManager();

    //! \param shallowCopy P�ytka kopia bazy danych na bazie ktorej b�d� tworzone statusy (odtworzona ich hierarchia)
    void setShallowCopy(const communication::ShallowCopy * shallowCopy);

    //! \param modifiedFilesStatus Zbi�r plik�w kt�re zmieni�y sw�j status. Ich nowy status dostaniemy poprzez obiekt FileStatusManager)
    void refreshDataStatus(const std::set<int> & modifiedFilesStatus);

    //! \param disorderID Identyfikator schorzenia kt�rego status chcemy pobra�
    //! \return Status schorzenia
    virtual const DataStatus & disorderStatus(int disorderID) const;
    //! \param patientID Identyfikator pacjenta kt�rego status chcemy pobra�
    //! \return Status pacjenta
    virtual const DataStatus & patientStatus(int patientID) const;
    //! \param subjectID Identyfikator subjecta kt�rego status chcemy pobra�
    //! \return Status subjecta
    virtual const DataStatus & subjectStatus(int subjectID) const;
    //! \param sessionID Identyfikator sesji kt�rej status chcemy pobra�
    //! \return Status sesji
    virtual const DataStatus & sessionStatus(int sessionID) const;
    //! \param motionID Identyfikator motiona kt�rego status chcemy pobra�
    //! \return Status motiona
    virtual const DataStatus & motionStatus(int motionID) const;
    //! \param fileID Identyfikator pliku kt�rego status chcemy pobra�
    //! \return Status pliku
    virtual const DataStatus & fileStatus(int fileID) const;
	//! \param status Status danych dla kt�rego chcemy pobrac ikon�
	//! \return Ikona dla danego statusu
	static QIcon statusIcon(const communication::IDataStatus & status);
	//! \param status Status danych dla kt�rego chcemy pobrac ikon�
	//! \param icon Ikona dla danego statusu
	static void setStatusIcon(const communication::IDataStatus & status, const QIcon & icon);

protected:
    //! Aktualizuje ca�y status danych - u�ycie i dostepno��
    void rebuildDataStatus();

    //! Czy�ci informacje o statusach wszystkich znanych obiekt�w
    void clearAllStatusData();

private:
    //! Mapa status�w schorze�
    std::map<int, DataStatus> disordersStatus;
    //! Mapa status�w pacjent�w
    std::map<int, DataStatus> patientsStatus;
    //! Mapa status�w subject�w
    std::map<int, DataStatus> subjectsStatus;
    //! Mapa status�w sesji
    std::map<int, DataStatus> sessionsStatus;
    //! Mapa status�w motion�w
    std::map<int, DataStatus> motionsStatus;
    //! P�ytka kopia dla kt�rej generujemy statusy
    const communication::ShallowCopy * shallowCopy;
	//! Manager statusu plik�w
	const FileStatusManager * fileStatusManager;
	//! Ikony status�w
	static std::map<communication::DataStorage, std::map<communication::DataUsage, QIcon>> statusIcons;
};

#endif	//	HEADER_GUARD___DATASOURCESTATUSMANAGER_H__
