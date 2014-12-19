/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   23:26
    filename: IDataHierarchyManagerReader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATAHIERARCHYMANAGERREADER_H__
#define HEADER_GUARD___IDATAHIERARCHYMANAGERREADER_H__

#include <corelib/Variant.h>
#include <utils/TypeInfo.h>

namespace core {

	class IDataHierarchyManagerReader {

	public:
		//! Destruktor wirtualny
		virtual ~IDataHierarchyManagerReader() {}

		//! \param typeInfo Typ zarejestrowany w DM dla kt�ego chemy utworzy� OW
		//! \return Pusty OW dla zadanego typu lub wyj�tek/nullptr gdy typ niezarejestrowany
		virtual VariantPtr createWrapper(const utils::TypeInfo & typeInfo) const = 0;

		//! \return Zarejestrowane w aplikacji typy danych
		virtual void getRegisteredTypes(utils::TypeInfoSet & registeredTypes) const = 0;
		//! \return Zarejestrowane w aplikacji typy danych
		virtual const bool isRegistered(const utils::TypeInfo & type) const = 0;

		//! \return Hierarchia typ�w danych - jakie operacje moge realizowa�, po czym dziedzicze
		virtual void getTypeBaseTypes(const utils::TypeInfo & type, utils::TypeInfoSet & baseTypes) const = 0;

		//! \return Hierarchia typ�w danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i mo�e by� downcastowany na mnie
		virtual void getTypeDerrivedTypes(const utils::TypeInfo & type, utils::TypeInfoSet & derrivedTypes) const = 0;

		//! \param base Typ bazowy kt�rego kompatybilno�c sprawdzamy
		//! \param derrived Typ pochodny wzgl�dem k�trego sprawdzamy kompatybilno�c typu bazowego
		//! \return Prawda kiedy typ bazowy jest faktycznie wspierany przez typ pochodny
		virtual const bool isTypeCompatible(const utils::TypeInfo & base, const utils::TypeInfo & derrived) const = 0;
	};
}

#endif	//	HEADER_GUARD___IDATAHIERARCHYMANAGERREADER_H__