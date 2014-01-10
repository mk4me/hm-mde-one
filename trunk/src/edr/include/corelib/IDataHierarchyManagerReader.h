/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   23:26
    filename: IDataHierarchyManagerReader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATAHIERARCHYMANAGERREADER_H__
#define HEADER_GUARD___IDATAHIERARCHYMANAGERREADER_H__

#include <corelib/BaseDataTypes.h>

namespace core {

	class IDataHierarchyManagerReader {

	public:
		//! Destruktor wirtualny
		virtual ~IDataHierarchyManagerReader() {}

		//! \param typeInfo Typ zarejestrowany w DM dla któego chemy utworzyæ OW
		//! \return Pusty OW dla zadanego typu lub wyj¹tek/nullptr gdy typ niezarejestrowany
		virtual ObjectWrapperPtr createWrapper(const TypeInfo & typeInfo) const = 0;

		//! \return Zarejestrowane w aplikacji typy danych
		virtual void getRegisteredTypes(TypeInfoSet & registeredTypes) const = 0;
		//! \return Zarejestrowane w aplikacji typy danych
		virtual const bool isRegistered(const TypeInfo & type) const = 0;

		//! \return Hierarchia typów danych - jakie operacje moge realizowaæ, po czym dziedzicze
		virtual void getTypeBaseTypes(const TypeInfo & type, TypeInfoSet & baseTypes) const = 0;

		//! \return Hierarchia typów danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i mo¿e byæ downcastowany na mnie
		virtual void getTypeDerrivedTypes(const TypeInfo & type, TypeInfoSet & derrivedTypes) const = 0;

		//! \param base Typ bazowy którego kompatybilnoœc sprawdzamy
		//! \param derrived Typ pochodny wzglêdem kótrego sprawdzamy kompatybilnoœc typu bazowego
		//! \return Prawda kiedy typ bazowy jest faktycznie wspierany przez typ pochodny
		virtual const bool isTypeCompatible(const TypeInfo & base, const TypeInfo & derrived) const = 0;
	};
}

#endif	//	HEADER_GUARD___IDATAHIERARCHYMANAGERREADER_H__
