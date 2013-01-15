/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   23:18
    filename: IExtensionManagerReader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IEXTENSIONMANAGERREADER_H__
#define HEADER_GUARD___IEXTENSIONMANAGERREADER_H__

#include <vector>
#include <set>
#include <string>
#include <core/TypeInfo.h>

namespace plugin {

	//! Manager zarejestrowanych rozszerze� plik�w w aplikacji, kt�re aplikacja potrafi obs�u�y� przez parsery
	class IExtensionManagerReader
	{
	public:
		//! Zbi�r rozszerze�
		typedef std::set<std::string> Extensions;

		//! Opis rozszerze�
		struct ExtensionDescription
		{
			//! Zarejestrowane opisy
			std::vector<std::string> descriptions;
			//! Zarejestrowane typy
			core::TypeInfoList types;
		};

	public:
		//! \return Zbior obs�ugiwanych rozszerzen plik�w wraz z ich opisem
		virtual const Extensions getSupportedFilesExtensions() const = 0;

		//! \return true je�li rozszerznie jest wspierane przez DataManager, w przeciwnym wypadku false
		virtual const bool isExtensionSupported(const std::string & extension) const = 0;

		//! \param extension Rozszerzenie dla kt�rego pytamy o opis
		//! \return Opis rozszerzenia
		virtual const ExtensionDescription & getExtensionDescription(const std::string & extension) const = 0;
	};

}

#endif	//	HEADER_GUARD___IEXTENSIONMANAGERREADER_H__
