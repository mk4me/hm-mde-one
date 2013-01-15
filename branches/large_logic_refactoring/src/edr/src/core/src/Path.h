/********************************************************************
    created:  2013/01/15
    created:  15:1:2013   8:54
    filename: Path.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___PATH_H__
#define HEADER_GUARD___PATH_H__

#include <core/IPath.h>

namespace core {

	class Path : public plugin::IPath
	{
	public:
		Path();
		virtual ~Path();

	public:
		//! \return Zwraca pelna sciezke do folderu "MyDocuments\EDR"
		virtual const core::Filesystem::Path& getUserDataPath() const;
		//! \return Zwraca pelna sciezke do folderu "ApplicationData\EDR"
		virtual const core::Filesystem::Path& getApplicationDataPath() const;
		//! \return Zwraca pelna sciezke do folderu z zasobami aplikacji
		virtual const core::Filesystem::Path& getResourcesPath() const;
		//! \return Zwraca pe�n� �cie�k� do tymczasowego folderu, kt�ry jest czyszczony podczas ka�dego zamykania aplikacji
		virtual const core::Filesystem::Path& getTmpPath() const;
		//! \return Zwraca pe�n� �cie�k� do folderu pluginu
		virtual const core::Filesystem::Path& getPluginPath() const;
	};
}

#endif	//	HEADER_GUARD___PATH_H__
