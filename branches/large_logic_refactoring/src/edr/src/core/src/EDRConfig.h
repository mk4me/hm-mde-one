/********************************************************************
	created:	2011/06/07
	created:	7:6:2011   12:58
	filename: 	EDRCOnfig.h
	author:		Wojciech Knieć
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__DIRECTORIESINFO_H__
#define HEADER_GUARD_CORE__DIRECTORIESINFO_H__

#include <core/IPath.h>

namespace core {

class EDRConfig : public plugin::IPath
{
public:
	//! ustawia pelna sciezke do folderu "MyDocuments\EDR"
	void setUserDataPath(const Filesystem::Path& path);
	//! ustawia pelna sciezke do folderu "ApplicationData\EDR"
	void setResourcesPath(const Filesystem::Path& path);
	//! ustawia pelna sciezke do folderu z zasobami aplikacji
	void setApplicationDataPath(const Filesystem::Path& path);

    //! sprawdza czy katalog dla danych tymczasowych istnieje, jeśli nie tworzy go
    void ensureTempDirectory() const;
    //! czyści katalog danych tymczasowych przy zamykaniu aplikacji
    void clearTempDirectory() const;

public:
	//! zwraca pelna sciezke do folderu "MyDocuments\EDR"
	virtual const Filesystem::Path& getUserDataPath() const;
	//! zwraca pelna sciezke do folderu "ApplicationData\EDR"
	virtual const Filesystem::Path& getApplicationDataPath() const;
	//! zwraca pelna sciezke do folderu z zasobami aplikacji
	virtual const Filesystem::Path& getResourcesPath() const;
    //! \return Zwraca pełną ścieżkę do tymczasowego folderu, który jest czyszczony podczas każdego zamykania aplikacji
    virtual const Filesystem::Path& getTmpPath() const;

public:
	static void setPaths(EDRConfig& directoriesInfo);
	static EDRConfig* getInstance();

protected:
	static bool trySetPathsFromRegistry(EDRConfig& directoriesInfo);
	static bool trySetDefaultPaths(EDRConfig& directoriesInfo);

private:
	//! ścieżka do folderu z zasobami aplikacji
	Filesystem::Path resourcesPath;
	//! ścieżka do folderu "ApplicationData\EDR"
	Filesystem::Path applicationDataPath;
	//! ścieżka do folderu "MyDocuments\EDR"
	Filesystem::Path userDataPath;
    //! ścieżka do folderu Tempów ("MyDocuments\EDR\Temp"
    Filesystem::Path tempDataPath;
};

}

#endif
