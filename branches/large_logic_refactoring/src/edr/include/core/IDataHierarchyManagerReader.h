/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   23:26
    filename: IDataHierarchyManagerReader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATAHIERARCHYMANAGERREADER_H__
#define HEADER_GUARD___IDATAHIERARCHYMANAGERREADER_H__

#include <core/ObjectWrapper.h>

namespace core {

	class IDataHierarchyManagerReader {

	public:
		//! \param typeInfo Typ zarejestrowany w DM dla kt�ego chemy utworzy� OW
		//! \return Pusty OW dla zadanego typu lub wyj�tek/nullptr gdy typ niezarejestrowany
		virtual ObjectWrapperPtr createWrapper(const TypeInfo & typeInfo) const = 0;

		//! \return Zarejestrowane w aplikacji typy danych
		virtual const TypeInfoSet getRegisteredTypes() const = 0;
		//! \return Zarejestrowane w aplikacji typy danych
		virtual const bool isRegistered(const TypeInfo & type) const = 0;

		//! \return Hierarchia typ�w danych - jakie operacje moge realizowa�, po czym dziedzicze
		virtual const TypeInfoSet getTypeBaseTypes(const TypeInfo & type) const = 0;

		//! \return Hierarchia typ�w danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i mo�e by� downcastowany na mnie
		virtual const TypeInfoSet getTypeDerrivedTypes(const TypeInfo & type) const = 0;

		//! \param base Typ bazowy kt�rego kompatybilno�c sprawdzamy
		//! \param derrived Typ pochodny wzgl�dem k�trego sprawdzamy kompatybilno�c typu bazowego
		//! \return Prawda kiedy typ bazowy jest faktycznie wspierany przez typ pochodny
		virtual const bool isTypeCompatible(const TypeInfo & base, const TypeInfo & derrived) const = 0;
	};
}

#endif	//	HEADER_GUARD___IDATAHIERARCHYMANAGERREADER_H__
