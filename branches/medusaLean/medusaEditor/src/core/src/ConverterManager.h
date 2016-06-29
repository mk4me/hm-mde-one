/********************************************************************
    created:  2013/03/25
    created:  25:3:2013   14:43
    filename: ConverterManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CONVERTERMANAGER_H__
#define HEADER_GUARD___CONVERTERMANAGER_H__

#include <corelib/IConverterManager.h>

namespace core {

	//! Implementacja managera konwerter�w w aplikacji
	class ConverterManager : public plugin::IConverterManager
	{
	private:
		//! Typ mapuj�cy typy danych do indeks�w na kt�rych te typy wyst�puj�
		typedef std::map<utils::TypeInfo, std::set<int>> DataTypeToIndex;

		//! Struktura opisuj�ca konwerter
		struct ConverterData
		{
			//! Sumaryczny rozmiar wej�cia (ile danych faktycznie potrzeba by konwertowa�)
			unsigned int inputSize;
			//! Mapowanie typ�w danych wyjsciowych do ich indeks�w
			DataTypeToIndex outputTypesIndexes;
			//! Mapowanie typ�w danych wejsciowych do ich indeks�w
			DataTypeToIndex inputTypesIndexes;
			//! Zbi�r typ�w wej�ciowych
			utils::TypeInfoSet inputTypes;
			//! Zbi�r typ�w wyj�ciowych
			utils::TypeInfoSet outputTypes;
		};

		//! Mapa konwerter�w i ich opisu
		typedef std::map<plugin::IConverterPtr, ConverterData> ConvertersDescription;

		//! Mapa typ�w do zwi�zanych z nimi konwerter�w
		typedef std::map<utils::TypeInfo, Converters> ConvertersByTypes;

		//! Mapowanie danych wg typow
		typedef std::map<utils::TypeInfo, utils::ConstObjectsList> DataByTypes;

	private:
		//! Kolekcja zarejestrowanych parser�w
		ConvertersDescription converters_;
		//! Mapa konwerter�w wg przyjmowanych wej��
		ConvertersByTypes convertersByInputs_;
		//! Mapa konwerter�w wg oferownaych wyj��
		ConvertersByTypes convertersByOutputs_;

	private:

		//! \param converter Konwerter dla kt�rego przygotowujemy opis
		//! \param data [out] Opis konwertera kt�ry wype�niamy
		static void fillConverterData(const plugin::IConverterPtr & converter, ConverterData & data);

		//! \param first Para typ -> zbi�r wej�� z typem
		//! \param first Para typ -> zbi�r wej�� z typem
		//! \return Prawda je�li typy i ich ilo�c si� zgadzaj�
		static const bool compareInputs(const DataTypeToIndex::value_type & first, const DataTypeToIndex::value_type & second);

		//! \param converter Konwerter kt�rego chcemy u�y�
		//! \param data Dane do dopasowania
		//! \param conversionData [out] Dane gotowe do zasilenia konwertera
		static void generateConverterInputs(const ConverterData & converterDescription, const utils::ConstObjectsList & data, std::list<utils::ConstObjectsVector> & conversionData);

		//! \param input Dane pogrupowane wg typow obslugiwanych przez wejscia
		//! \param conversionData [out] dane do konwersji
		static void generateConverterInputs(const DataByTypes & input, std::list<utils::ConstObjectsVector> & conversionData);

	public:

		ConverterManager();

		//! Wirtualny destruktor
		virtual ~ConverterManager();

		//! \param input Dane wej�ciowe do konwersji
		//! \param output [out] Wyniki konwersji	
		//! \param recursive Czy wyniki maj� by� generowane rekurencyjnie r�wnie� na bazie wynik�w z poprzednich iteracji
		virtual void convert(const utils::ConstObjectsList & input, utils::ConstObjectsList & output, bool recursive = false) const;

		//! \param input Dane wej�ciowe do konwersji
		//! \param output [out] Wyniki konwersji	
		//! \param requestedTypes Oczekiwane typy wyj�ciowe
		//! \param recursive Czy wyniki maj� by� generowane rekurencyjnie r�wnie� na bazie wynik�w z poprzednich iteracji
		virtual void convert(const utils::ConstObjectsList & input, utils::ConstObjectsList & output, const utils::TypeInfoSet & requestedTypes, bool recursive = false) const;

		//! \param converters [out] Wszystkie zarejestrowane konwertery danych domenowych
		virtual void converters(Converters & converters) const;

		//! \param inTypes Typy danych wej�ciowych kt�re chcemy konwertowa�
		//! \param converters [out] Zarejestrowane konwertery danych domenowych kt�re potrafi� operowa� na danym zbiorze i jego podzbiorach
		virtual void convertersForInputs(const utils::TypeInfoList & inTypes, plugin::IConverterManager::Converters & converters) const;
		//! \param inTypes Typy danych wej�ciowych kt�re chcemy uzyskac po konwersji
		//! \param converters [out] Zarejestrowane konwertery danych domenowych kt�re potrafi� generowa� dane zadanego typu
		virtual void convertersWithOutputs(const utils::TypeInfoSet & outTypes, plugin::IConverterManager::Converters & converters) const;

		//! \param converter Konwerter do zarejestrowania
		void registerConverter(const plugin::IConverterPtr & converter);
	};

}

#endif	//	HEADER_GUARD___CONVERTERMANAGER_H__
