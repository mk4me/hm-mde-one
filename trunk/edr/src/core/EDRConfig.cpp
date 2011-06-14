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

const IPath::Path& EDRConfig::getApplicationDataPath() const
{
	UTILS_ASSERT(!applicationDataPath.empty(), "Path should be initialized first");
	return applicationDataPath;
}

const IPath::Path& EDRConfig::getUserDataPath() const
{
	UTILS_ASSERT(!userDataPath.empty(), "Path should be initialized first");
	return userDataPath;
}

const IPath::Path& EDRConfig::getResourcesPath() const
{
	UTILS_ASSERT(!resourcesPath.empty(), "Path should be initialized first");
	return resourcesPath;
}

void EDRConfig::setUserDataPath( const IPath::Path& path )
{
	UTILS_ASSERT(!path.empty(), "Path should not be empty");
	//UTILS_ASSERT(!path.is_relative(), "Path should not be relative");
	//UTILS_ASSERT(!path.has_filename(), "Path should not point to file");
	userDataPath = path;
}

void EDRConfig::setApplicationDataPath( const IPath::Path& path )
{
	UTILS_ASSERT(!path.empty(), "Path should not be empty");
	applicationDataPath = path;
}

void EDRConfig::setResourcesPath(const IPath::Path& path)
{
	UTILS_ASSERT(!path.empty(), "Path should not be empty");
	resourcesPath = path;
}

void EDRConfig::setPaths( EDRConfig& directoriesInfo )
{
	if (!trySetPathsFromRegistry(directoriesInfo) && !trySetDefaultPaths(directoriesInfo)) {
		throw std::runtime_error("Unable to set paths");
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
		core::IPath::Path p(buffer);
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
		directoriesInfo.setApplicationDataPath(core::IPath::Path(buffer));
		RegCloseKey(hKey);
	} else {
		return false;
	}

	lpValueName = "UserDataPath";
	dwSize = sizeof(buffer);
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_PATH, 0, KEY_READ, &hKey);
	if(lResult == ERROR_SUCCESS && RegQueryValueEx(hKey, lpValueName, 0, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
		directoriesInfo.setUserDataPath(core::IPath::Path(buffer));
		RegCloseKey(hKey);
	} 

	return lResult == ERROR_SUCCESS ? true : false;
#else
	return false;
#endif
}

bool EDRConfig::trySetDefaultPaths( EDRConfig& directoriesInfo )
{
	boost::filesystem::path userPath(toString(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)));
	userPath /= "EDR";
	directoriesInfo.setUserDataPath(userPath);

	boost::filesystem::path appDataPath(core::toStdString(QDesktopServices::storageLocation(QDesktopServices::DataLocation)));
	directoriesInfo.setApplicationDataPath(appDataPath);

	boost::filesystem::path resourcesPath(core::toStdString(QDir::currentPath())); 
	resourcesPath /= "resources";
	directoriesInfo.setResourcesPath(resourcesPath);

	boost::filesystem::path test2 = boost::filesystem::initial_path();

	boost::filesystem::path testPath(toString(QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation)));
	return true;
}

}
