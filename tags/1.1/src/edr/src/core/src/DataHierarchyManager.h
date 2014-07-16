/********************************************************************
    created:  2013/01/09
    created:  9:1:2013   8:17
    filename: DataHierarchyManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATAHIERARCHYMANAGER_H__
#define HEADER_GUARD___DATAHIERARCHYMANAGER_H__

#include <corelib/IDataHierarchyManagerReader.h>
#include <map>

namespace core {

class DataHierarchyManager : public IDataHierarchyManagerReader
{
private:
	//! Opis hierarchi typu
	struct TypeHierarchy {

		utils::ObjectWrapperPtr prototype;	//! Prototyp ObjectWrappera
		utils::TypeInfoSet baseTypes;		//! Lista typów bazowych
		utils::TypeInfoSet derrivedTypes;	//! Lista typów pochodnych

	};

	//! S³ownik hierarchii typow -> mapuje typ do jego typow bazowych (hierarchia dziedziczenia) [first] i do typów po nim dziedziczacych [second]
	typedef std::map<utils::TypeInfo, TypeHierarchy > TypesHierarchy;

public:
	//! \param typeInfo Typ zarejestrowany w DM dla któego chemy utworzyæ OW
	//! \return Pusty OW dla zadanego typu lub wyj¹tek/nullptr gdy typ niezarejestrowany
	virtual VariantPtr createWrapper(const utils::TypeInfo & typeInfo) const;

	//! \return Zarejestrowane w aplikacji typy danych
	virtual void getRegisteredTypes(utils::TypeInfoSet & registeredTypes) const;

	virtual const bool isRegistered(const utils::TypeInfo &) const;

	//! \return Hierarchia typów danych - jakie operacje moge realizowaæ, po czym dziedzicze
	virtual void getTypeBaseTypes(const utils::TypeInfo & type, utils::TypeInfoSet & baseTypes) const;

	//! \return Hierarchia typów danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i mo¿e byæ downcastowany na mnie
	virtual void getTypeDerrivedTypes(const utils::TypeInfo & type, utils::TypeInfoSet & derrivedTypes) const;

	//! \param base Typ bazowy którego kompatybilnoœc sprawdzamy
	//! \param derrived Typ pochodny wzglêdem kótrego sprawdzamy kompatybilnoœc typu bazowego
	//! \return Prawda kiedy typ bazowy jest faktycznie wspierany przez typ pochodny
	virtual const bool isTypeCompatible(const utils::TypeInfo & base, const utils::TypeInfo & derrived) const;

	//! \param owp Rejestrowany ObjectWrapper, które niesie informacjê o typach danych
	void registerObjectWrapperPrototype(const utils::ObjectWrapperPtr & owp);

private:
	//! Opis hierarchi wszystkich zarejestrowanych typów
	TypesHierarchy typesHierarchy;

};

}

#endif	//	HEADER_GUARD___DATAHIERARCHYMANAGER_H__
