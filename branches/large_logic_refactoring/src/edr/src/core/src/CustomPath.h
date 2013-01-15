/********************************************************************
    created:  2013/01/07
    created:  7:1:2013   15:49
    filename: CustomPath.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CUSTOMPATH_H__
#define HEADER_GUARD___CUSTOMPATH_H__

#include <core/IPath.h>

namespace core {

class CustomPath : public plugin::IPath
{
public:
	CustomPath(plugin::IPath * path, const std::string & folderName);

	//! \return Zwraca pelna sciezke do folderu "MyDocuments\EDR"
	virtual const Filesystem::Path& getUserDataPath() const;
	//! \return Zwraca pelna sciezke do folderu "ApplicationData\EDR"
	virtual const Filesystem::Path& getApplicationDataPath() const;
	//! \return Zwraca pelna sciezke do folderu z zasobami aplikacji
	virtual const Filesystem::Path& getResourcesPath() const;
	//! \return Zwraca pe³n¹ œcie¿kê do tymczasowego folderu, który jest czyszczony podczas ka¿dego zamykania aplikacji
	virtual const Filesystem::Path& getTmpPath() const;
	//! \return Zwraca pe³n¹ œcie¿kê do folderu pluginu
	virtual const Filesystem::Path& getPluginPath() const;

private:
	plugin::IPath * path;
	const Filesystem::Path pluginPath;
	const Filesystem::Path pluginTmpPath;
};

}

#endif	//	HEADER_GUARD___CUSTOMPATH_H__
