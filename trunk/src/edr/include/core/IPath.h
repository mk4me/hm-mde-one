/********************************************************************
	created:	2011/06/07
	created:	7:6:2011   12:57
	filename: 	IDirectoriesInfo.h
	author:		Wojciech Knieć
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IDIRECTORIESINFO_H__
#define HEADER_GUARD_CORE__IDIRECTORIESINFO_H__

#include <core/Filesystem.h>

namespace core {

//! Interfejs dostarczający informacji o strukturze plików obsługiwanej przez aplikację.
class IPath
{
public:
	virtual ~IPath() {}

public:
	//! \return Zwraca pelna sciezke do folderu "MyDocuments\EDR"
	virtual const Filesystem::Path& getUserDataPath() const = 0;
	//! \return Zwraca pelna sciezke do folderu "ApplicationData\EDR"
	virtual const Filesystem::Path& getApplicationDataPath() const = 0;
	//! \return Zwraca pelna sciezke do folderu z zasobami aplikacji
	virtual const Filesystem::Path& getResourcesPath() const = 0;
    //! \return Zwraca pełną ścieżkę do tymczasowego folderu, który jest czyszczony podczas każdego zamykania aplikacji
    virtual const Filesystem::Path& getTempPath() const = 0;
};

}

#endif
