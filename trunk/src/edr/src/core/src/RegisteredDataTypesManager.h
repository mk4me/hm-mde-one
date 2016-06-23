/********************************************************************
    created:  2013/01/09
    created:  9:1:2013   8:17
    filename: DataHierarchyManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATAHIERARCHYMANAGER_H__
#define HEADER_GUARD___DATAHIERARCHYMANAGER_H__

#include <corelib/IRegisteredDataTypesManagerReader.h>
#include <map>

namespace core {

	class RegisteredDataTypesManager : public IRegisteredDataTypesManagerReader
{
private:
	//! Opis hierarchi typu
	struct TypeHierarchy {

		utils::ObjectWrapperPtr prototype;	//! Prototyp ObjectWrappera
		utils::TypeInfoSet baseTypes;		//! Lista typ�w bazowych
		utils::TypeInfoSet derrivedTypes;	//! Lista typ�w pochodnych

	};

	//! S�ownik hierarchii typow -> mapuje typ do jego typow bazowych (hierarchia dziedziczenia) [first] i do typ�w po nim dziedziczacych [second]
	typedef std::map<utils::TypeInfo, TypeHierarchy > TypesHierarchy;

public:
	//! \param typeInfo Typ zarejestrowany w DM dla kt�ego chemy utworzy� OW
	//! \return Pusty OW dla zadanego typu lub wyj�tek/nullptr gdy typ niezarejestrowany
	virtual VariantPtr createWrapper(const utils::TypeInfo & typeInfo) const;

	//! \return Zarejestrowane w aplikacji typy danych
	virtual void registeredTypes(utils::TypeInfoSet & registeredTypes) const;

	virtual const bool isRegistered(const utils::TypeInfo &) const;

	//! \return Hierarchia typ�w danych - jakie operacje moge realizowa�, po czym dziedzicze
	virtual utils::TypeInfoSet baseTypes(const utils::TypeInfo & type) const;

	//! \return Hierarchia typ�w danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i mo�e by� downcastowany na mnie
	virtual utils::TypeInfoSet derrivedTypes(const utils::TypeInfo & type) const;

	//! \param base Typ bazowy kt�rego kompatybilno�c sprawdzamy
	//! \param derrived Typ pochodny wzgl�dem k�trego sprawdzamy kompatybilno�c typu bazowego
	//! \return Prawda kiedy typ bazowy jest faktycznie wspierany przez typ pochodny
	virtual const bool isDerrived(const utils::TypeInfo & type, const utils::TypeInfo & derrived) const;

	//! \param base Typ bazowy kt�rego kompatybilno�c sprawdzamy
	//! \param derrived Typ pochodny wzgl�dem k�trego sprawdzamy kompatybilno�c typu bazowego
	//! \return Prawda kiedy typ bazowy jest faktycznie wspierany przez typ pochodny
	virtual const bool isBase(const utils::TypeInfo & type, const utils::TypeInfo & base) const;

	//! \param owp Rejestrowany ObjectWrapper, kt�re niesie informacj� o typach danych
	void registerObjectWrapperPrototype(const utils::ObjectWrapperPtr & owp);

private:
	//! Opis hierarchi wszystkich zarejestrowanych typ�w
	TypesHierarchy typesHierarchy;

};

}

#endif	//	HEADER_GUARD___DATAHIERARCHYMANAGER_H__
