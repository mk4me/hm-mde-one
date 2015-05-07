/********************************************************************
	created:  2015/05/04	12:38:57
	filename: Types.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATIC__TYPES_H__
#define __HEADER_GUARD_KINEMATIC__TYPES_H__

namespace kinematic
{
	//! Typ danych
	enum DataType
	{
		Local = 0,	//! Lokalne
		Global = 1	//! Globalne
	};

	//! \tparam Base Typ bazowy, wrapowany
	//! \tparam Type Wartoœæ reprezentuj¹ca typ
	template<typename Base, int Type>
	//! Klasa wrapuj¹ca dane ruchu (stan, zmianê) dodaj¹ca informacje o charakterze tych danych - lokalne|globalne
	class Data : public Base
	{
		static_assert(Type == Local || Type == Global, "Type must be Local or Global");

	public:
		//! Konstruktor domyslny
		Data() {}
		//! Konstruktor kopiuj¹cy
		//! \param data Kopiowane dane
		Data(const Data & data) : Base(data) {}
		//! Konstruktor przenosz¹cy
		//! \param data Przenoszone dane
		Data(Data && data) : Base(static_cast<Base&&>(std::move(data))) {}
		//! Konstruktor
		//! \param data Kopiowane dane
		explicit Data(const Base & data) : Base(data) {}
		//! Konstruktor
		//! \param data Przenoszone dane
		explicit Data(Base && data) : Base(std::move(data)) {}
		//! Destruktor wirtualny
		virtual ~Data() {}

	public:

		enum {
			type = Type	//! Typ danych
		};		
	};

	//! \tparam Base Typ bazowy
	template<typename Base>
	//! Wrapper dla reprezentacji danych lokalnych
	using LocalData = Data < Base, Local > ;

	//! \tparam Base Typ bazowy
	template<typename Base>
	//! Wrapper dla reprezentacji danych globalnych
	using GlobalData = Data < Base, Global > ;
}

#endif	// __HEADER_GUARD_KINEMATIC__TYPES_H__