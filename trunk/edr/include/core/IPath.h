/********************************************************************
	created:	2011/06/07
	created:	7:6:2011   12:57
	filename: 	IDirectoriesInfo.h
	author:		Wojciech Knieć
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IDIRECTORIESINFO_H__
#define HEADER_GUARD_CORE__IDIRECTORIESINFO_H__

#include <boost/filesystem.hpp>

namespace core {

//! Interfejs dostarczający informacji o strukturze plików obsługiwanej przez aplikację.
class IPath
{
public:
    //! Typ reprezentacji ścieżek
	typedef boost::filesystem::path Path;
public:
	virtual ~IPath() {}

public:
	//! \return Zwraca pelna sciezke do folderu "MyDocuments\EDR"
	virtual const Path& getUserDataPath() const = 0;
	//! \return Zwraca pelna sciezke do folderu "ApplicationData\EDR"
	virtual const Path& getApplicationDataPath() const = 0;
	//! \return Zwraca pelna sciezke do folderu z zasobami aplikacji
	virtual const Path& getResourcesPath() const = 0;
};

}

#endif
