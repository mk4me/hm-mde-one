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

	//! Manager zarejestrowanych rozszerzeñ plików w aplikacji, które aplikacja potrafi obs³u¿yæ przez parsery
	class IExtensionManagerReader
	{
	public:
		//! Zbiór rozszerzeñ
		typedef std::set<std::string> Extensions;

		//! Opis rozszerzeñ
		struct ExtensionDescription
		{
			//! Zarejestrowane opisy
			std::vector<std::string> descriptions;
			//! Zarejestrowane typy
			core::TypeInfoList types;
		};

	public:
		//! \return Zbior obs³ugiwanych rozszerzen plików wraz z ich opisem
		virtual const Extensions getSupportedFilesExtensions() const = 0;

		//! \return true jeœli rozszerznie jest wspierane przez DataManager, w przeciwnym wypadku false
		virtual const bool isExtensionSupported(const std::string & extension) const = 0;

		//! \param extension Rozszerzenie dla którego pytamy o opis
		//! \return Opis rozszerzenia
		virtual const ExtensionDescription & getExtensionDescription(const std::string & extension) const = 0;
	};

}

#endif	//	HEADER_GUARD___IEXTENSIONMANAGERREADER_H__
