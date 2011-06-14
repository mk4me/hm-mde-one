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
#include <core/PluginCommon.h>
namespace core {

class EDRConfig : public IPath
{
public:
	//! ustawia pelna sciezke do folderu "MyDocuments\EDR"
	void setUserDataPath(const IPath::Path& path);
	//! ustawia pelna sciezke do folderu "ApplicationData\EDR"
	void setResourcesPath(const IPath::Path& path);
	//! ustawia pelna sciezke do folderu z zasobami aplikacji
	void setApplicationDataPath(const IPath::Path& path);

public:
	//! zwraca pelna sciezke do folderu "MyDocuments\EDR"
	virtual const IPath::Path& getUserDataPath() const;
	//! zwraca pelna sciezke do folderu "ApplicationData\EDR"
	virtual const IPath::Path& getApplicationDataPath() const;
	//! zwraca pelna sciezke do folderu z zasobami aplikacji
	virtual const IPath::Path& getResourcesPath() const;

public:
	static void setPaths(EDRConfig& directoriesInfo);
	static EDRConfig* getInstance()
	{
		return static_cast<EDRConfig*>(getPathInterface());
	}

protected:
	static bool trySetPathsFromRegistry(EDRConfig& directoriesInfo);
	static bool trySetDefaultPaths(EDRConfig& directoriesInfo);

private:
	//! sciezka do folderu z zasobami aplikacji
	Path resourcesPath;
	//! sciezka do folderu "ApplicationData\EDR"
	Path applicationDataPath;
	//! sciezka do folderu "MyDocuments\EDR"
	Path userDataPath;
};

}

#endif
