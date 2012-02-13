#include "CorePCH.h"
#include "EDRConfig.h"

#ifdef WIN32
#include <Windows.h>
//#define KEY_PATH1 TEXT("Software\\Wow6432Node\\PJWSTK\\EDR")
// Od Visty dodawane sa przedrostki typu Wow6432Node do sciezki w rejestrach
// adres podawany do oczytu klucza powinien byc automatycznie konwertowany.
#define KEY_PATH TEXT("Software\\PJWSTK\\EDR")
#endif


namespace core 
{

const Filesystem::Path& EDRConfig::getApplicationDataPath() const
{
	UTILS_ASSERT(!applicationDataPath.empty(), "Path should be initialized first");
	return applicationDataPath;
}

const Filesystem::Path& EDRConfig::getUserDataPath() const
{
	UTILS_ASSERT(!userDataPath.empty(), "Path should be initialized first");
	return userDataPath;
}

const Filesystem::Path& EDRConfig::getResourcesPath() const
{
	UTILS_ASSERT(!resourcesPath.empty(), "Path should be initialized first");
	return resourcesPath;
}

const Filesystem::Path& EDRConfig::getTmpPath() const
{
    UTILS_ASSERT(!tempDataPath.empty(), "Path should be initialized first");
    return tempDataPath;
}

void EDRConfig::setUserDataPath( const Filesystem::Path& path )
{
	UTILS_ASSERT(!path.empty(), "Path should not be empty");
	//UTILS_ASSERT(!path.is_relative(), "Path should not be relative");
	//UTILS_ASSERT(!path.has_filename(), "Path should not point to file");
	userDataPath = path;
    tempDataPath = userDataPath / "tmp";
}

void EDRConfig::setApplicationDataPath( const Filesystem::Path& path )
{
	UTILS_ASSERT(!path.empty(), "Path should not be empty");
	applicationDataPath = path;
}

void EDRConfig::setResourcesPath(const Filesystem::Path& path)
{
	UTILS_ASSERT(!path.empty(), "Path should not be empty");
	resourcesPath = path;
}

void EDRConfig::setPaths( EDRConfig& directoriesInfo )
{
	if (!trySetPathsFromRegistry(directoriesInfo)) {
	    if (!trySetDefaultPaths(directoriesInfo)) {
			throw std::runtime_error("Unable to set paths");
		}
	}
}

//! sprawdza czy katalog dla danych tymczasowych istnieje, jeœli nie tworzy go
void EDRConfig::ensureTempDirectory() const
{
    //sprawdzamy czy katalog dla danych tymczasowych istnieje - jesli nie tworzymy go
    if(Filesystem::pathExists(getTmpPath()) == false){
        Filesystem::createDirectory(getTmpPath());
    }
}
//! czyœci katalog danych tymczasowych przy zamykaniu aplikacji
void EDRConfig::clearTempDirectory() const
{
    //sprawdzamy czy katalog dla danych tymczasowych istnieje - jesli nie tworzymy go
    if(Filesystem::pathExists(getTmpPath()) == true){
        Filesystem::deleteDirectory(getTmpPath());
    }
}

bool EDRConfig::trySetPathsFromRegistry( EDRConfig& directoriesInfo )
{
#ifdef WIN32
	DWORD dwDefault = 0;
	
	HKEY hKey;
	LONG lResult;
	char buffer[1024];
	DWORD dwType, dwSize = sizeof(buffer);
	
	LPTSTR lpValueName = "ProgramFilesPath";
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_PATH, 0, KEY_READ, &hKey);
	if(lResult == ERROR_SUCCESS && RegQueryValueEx(hKey, lpValueName, 0, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
		core::Filesystem::Path p(buffer);
		p /= "bin";
		p /= "resources";
		directoriesInfo.setResourcesPath(p);
		RegCloseKey(hKey);
	} else {
		return false;
	}

	lpValueName = "ApplicationDataPath";
	dwSize = sizeof(buffer);
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_PATH, 0, KEY_READ, &hKey);
	if(lResult == ERROR_SUCCESS && RegQueryValueEx(hKey, lpValueName, 0, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
		directoriesInfo.setApplicationDataPath(core::Filesystem::Path(buffer));
		RegCloseKey(hKey);
	} else {
		return false;
	}

	lpValueName = "UserDataPath";
	dwSize = sizeof(buffer);
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_PATH, 0, KEY_READ, &hKey);
	if(lResult == ERROR_SUCCESS && RegQueryValueEx(hKey, lpValueName, 0, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
		directoriesInfo.setUserDataPath(core::Filesystem::Path(buffer));
		RegCloseKey(hKey);
	} 

	return lResult == ERROR_SUCCESS ? true : false;
#else
	return false;
#endif
}

bool EDRConfig::trySetDefaultPaths( EDRConfig& directoriesInfo )
{
	core::Filesystem::Path userPath(toString(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)));
	userPath /= "PJWSTK";
	userPath /= "EDR";
	directoriesInfo.setUserDataPath(userPath);

	core::Filesystem::Path appDataPath(core::toStdString(QDesktopServices::storageLocation(QDesktopServices::DataLocation)));
	directoriesInfo.setApplicationDataPath(appDataPath);

	core::Filesystem::Path resourcesPath(core::toStdString(QDir::currentPath())); 
	resourcesPath /= "resources";
	directoriesInfo.setResourcesPath(resourcesPath);
	return true;
}

}
