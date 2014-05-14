/********************************************************************
	created:	2011/06/07
	created:	7:6:2011   12:57
	filename: 	IPath.h
	author:		Wojciech Knieć
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IDIRECTORIESINFO_H__
#define HEADER_GUARD_CORE__IDIRECTORIESINFO_H__

#include <corelib/Filesystem.h>

namespace core {

//! Interfejs dostarczający informacji o strukturze plików obsługiwanej przez aplikację.
class IPath
{
public:
	//! Destruktor wirtualny
	virtual ~IPath() {}

	//! \return Zwraca pelna sciezke do folderu "MyDocuments\APP"
	virtual const Filesystem::Path& getUserDataPath() const = 0;
	//! \return Zwraca pelna sciezke do folderu "ApplicationData\APP" dla wszystkich użytkowników
	virtual const Filesystem::Path& getApplicationDataPath() const = 0;
	//! \return Zwraca pelna sciezke do folderu "ApplicationData\APP" dla aktualnego użytkownika
	virtual const Filesystem::Path& getUserApplicationDataPath() const = 0;
	//! \return Zwraca pelna sciezke do folderu z zasobami aplikacji
	virtual const Filesystem::Path& getResourcesPath() const = 0;
    //! \return Zwraca pełną ścieżkę do tymczasowego folderu, który jest czyszczony podczas każdego zamykania aplikacji
    virtual const Filesystem::Path& getTmpPath() const = 0;
	//! \return Zwraca pełną ścieżkę do folderu pluginu
	virtual const Filesystem::Path& getPluginPath() const = 0;
};

}

#endif
