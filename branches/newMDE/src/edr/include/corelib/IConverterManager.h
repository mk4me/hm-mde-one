/********************************************************************
    created:  2013/03/22
    created:  22:3:2013   10:26
    filename: IConverterManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ICONVERTERMANAGER_H__
#define HEADER_GUARD___ICONVERTERMANAGER_H__

#include <corelib/IConverter.h>

namespace plugin {

//! Interfejs klasy zapewniaj¹cej dostêp do zarejestrowanych konwerterów
class IConverterManager
{
public:
	//! Typ kolekcji konwerterów
	typedef std::list<IConverterConstPtr> Converters;

public:
	//! Wirtualny destruktor
	virtual ~IConverterManager() {}

	//! \param input Dane wejœciowe do konwersji
	//! \param output [out] Wyniki konwersji	
	//! \param recursive Czy wyniki maj¹ byæ generowane rekurencyjnie równie¿ na bazie wyników z poprzednich iteracji
	virtual void convert(const utils::ConstObjectsList & input, utils::ConstObjectsList & output, bool recursive = false) const = 0;

	//! \param input Dane wejœciowe do konwersji
	//! \param output [out] Wyniki konwersji	
	//! \param requestedTypes Oczekiwane typy wyjœciowe
	//! \param recursive Czy wyniki maj¹ byæ generowane rekurencyjnie równie¿ na bazie wyników z poprzednich iteracji
	virtual void convert(const utils::ConstObjectsList & input, utils::ConstObjectsList & output, const utils::TypeInfoSet & requestedTypes, bool recursive = false) const = 0;

	//! \param converters [out] Wszystkie zarejestrowane konwertery danych domenowych
	virtual void converters(Converters & converters) const = 0;

	//! \param inTypes Typy danych wejœciowych które chcemy konwertowaæ
	//! \param converters [out] Zarejestrowane konwertery danych domenowych które potrafi¹ operowaæ na danym zbiorze i jego podzbiorach
	virtual void convertersForInputs(const utils::TypeInfoList & inTypes, Converters & converters) const = 0;
	//! \param inTypes Typy danych wejœciowych które chcemy uzyskac po konwersji
	//! \param converters [out] Zarejestrowane konwertery danych domenowych które potrafi¹ generowaæ dane zadanego typu
	virtual void convertersWithOutputs(const utils::TypeInfoSet & outTypes, Converters & converters) const = 0;
};

}

#endif	//	HEADER_GUARD___ICONVERTERMANAGER_H__
