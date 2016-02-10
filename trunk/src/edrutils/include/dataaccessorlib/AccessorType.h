/********************************************************************
	created:  2016/01/14
	filename: AccessorType.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__ACCESSORTYPE_H__
#define __HEADER_GUARD_DATAACCESSOR__ACCESSORTYPE_H__

#include <utility>
#include <utils/TypeInfo.h>

namespace dataaccessor
{
	//! Klasa opisujaca typ akcesora (przechowywanych danych)
	class AccessorType
	{
	public:
		//! Domy�lny konstruktor
		AccessorType() : AccessorType(typeid(void), typeid(void)) {}
		//! \param valueType Typ warto�ci
		//! \param argumentType Typ argument�w
		AccessorType(const utils::TypeInfo & valueType, const utils::TypeInfo & argumentType)
			: type(valueType, argumentType) {}

		//! \param id Kopiowany identyfikator
		AccessorType(const AccessorType & atype) : type(atype.type) {}
		//! Destruktor
		~AccessorType() {}

		//! \return Typ warto�ci
		inline const utils::TypeInfo & valueType() const { return type.first; }
		//! \return Typ argument�w
		inline const utils::TypeInfo & argumentType() const { return type.second; }

		//! \param valueType Typ warto�ci
		void setValueType(utils::TypeInfo & valueType) { type.first = valueType; }
		//! \param argumentType Typ argument�w
		void setArgumentType(utils::TypeInfo & argumentType) { type.second = argumentType; }

		//! \return Czy identyfikator jest poprawny
		inline bool valid() const { return (type.first != typeid(void)) || (type.second != typeid(void)); }
		//! \return Czy identyfikator jest kompletny
		inline bool complete() const { return (type.first != typeid(void)) && (type.second != typeid(void)); }

		//! \param filter Identyfikator do weryfikacji
		//! \return Prawda je�li nasz identyfikator pasuje do filtra
		inline bool match(const AccessorType & atype) const
		{
			if (atype.type.first != typeid(void) && atype.type.first != type.first) {
				return false;
			}

			if (atype.type.second != typeid(void) && atype.type.second != type.second) {
				return false;
			}

			return true;
		}

	private:
		//! Opis typu
		std::pair<utils::TypeInfo, utils::TypeInfo> type;
	};
}

#endif  // __HEADER_GUARD_DATAACCESSOR__ACCESSORTYPE_H__