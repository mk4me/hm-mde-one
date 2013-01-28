/********************************************************************
    created:  2013/01/15
    created:  15:1:2013   8:54
    filename: Path.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___PATH_H__
#define HEADER_GUARD___PATH_H__

#include <corelib/IPath.h>

namespace core {

	class Path : public IPath
	{
	public:
		Path(const Filesystem::Path & userDataPath,
		const Filesystem::Path & applicationDataPath,
		const Filesystem::Path & resourcesPath,
		const Filesystem::Path & tmpPath,
		const Filesystem::Path & pluginPath);
		virtual ~Path();

	public:
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
		Filesystem::Path userDataPath_;
		Filesystem::Path applicationDataPath_;
		Filesystem::Path resourcesPath_;
		Filesystem::Path tmpPath_;
		Filesystem::Path pluginPath_;
	};
}

#endif	//	HEADER_GUARD___PATH_H__
