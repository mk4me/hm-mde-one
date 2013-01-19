/********************************************************************
    created:  2013/01/09
    created:  9:1:2013   8:17
    filename: DataHierarchyManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATAHIERARCHYMANAGER_H__
#define HEADER_GUARD___DATAHIERARCHYMANAGER_H__

#include <core/IDataHierarchyManagerReader.h>
#include <map>

namespace core {

class DataHierarchyManager : public IDataHierarchyManagerReader
{
private:
	//! Opis hierarchi typu
	struct TypeHierarchy {

		ObjectWrapperPtr prototype;	//! Prototyp ObjectWrappera
		TypeInfoSet baseTypes;		//! Lista typ�w bazowych
		TypeInfoSet derrivedTypes;	//! Lista typ�w pochodnych

	};

	//! S�ownik hierarchii typow -> mapuje typ do jego typow bazowych (hierarchia dziedziczenia) [first] i do typ�w po nim dziedziczacych [second]
	typedef std::map<TypeInfo, TypeHierarchy > TypesHierarchy;

public:
	//! \param typeInfo Typ zarejestrowany w DM dla kt�ego chemy utworzy� OW
	//! \return Pusty OW dla zadanego typu lub wyj�tek/nullptr gdy typ niezarejestrowany
	virtual ObjectWrapperPtr createWrapper(const TypeInfo & typeInfo) const;

	//! \return Zarejestrowane w aplikacji typy danych
	virtual const TypeInfoSet getRegisteredTypes() const;

	virtual const bool isRegistered(const core::TypeInfo &) const;

	//! \return Hierarchia typ�w danych - jakie operacje moge realizowa�, po czym dziedzicze
	virtual const TypeInfoSet getTypeBaseTypes(const TypeInfo & type) const;

	//! \return Hierarchia typ�w danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i mo�e by� downcastowany na mnie
	virtual const TypeInfoSet getTypeDerrivedTypes(const TypeInfo & type) const;

	//! \param base Typ bazowy kt�rego kompatybilno�c sprawdzamy
	//! \param derrived Typ pochodny wzgl�dem k�trego sprawdzamy kompatybilno�c typu bazowego
	//! \return Prawda kiedy typ bazowy jest faktycznie wspierany przez typ pochodny
	virtual const bool isTypeCompatible(const TypeInfo & base, const TypeInfo & derrived) const;

	//! \param owp Rejestrowany ObjectWrapper, kt�re niesie informacj� o typach danych
	void registerObjectWrapperPrototype(const ObjectWrapperPtr & owp);

private:
	//! Szuka typu lub rzuca wyj�tkiem
	//! \param typeInfo Poszukiwany typ
	//! \return Opis hierarchi typu
	const TypeHierarchy & findOrThrow(const TypeInfo & typeInfo) const;

private:
	//! Opis hierarchi wszystkich zarejestrowanych typ�w
	TypesHierarchy typesHierarchy;

};

}

#endif	//	HEADER_GUARD___DATAHIERARCHYMANAGER_H__
