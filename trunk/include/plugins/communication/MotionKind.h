/**
@author Marek Daniluk
@brief Klasa MotionKind reprezentuje obiekt MotionKind po stronie serwera. Klasa powstala na bazie
dokumentacji i kodu zrodlowego zespolu bazodanowego. Brak szczegolow w dokumentacji nie pozwala na
jednoznaczne ustalenie przydatnosci klasy, jednak jak wspomina zespol bazodanowy, funkcjonalnosc
komunikacji z baza danych wciaz jest w fazie testow.
*/

#ifndef _MOTIONKIND_H_
#define _MOTIONKIND_H_

#include "Entity.h"

namespace communication {

	class MotionKind : public Entity {
	protected:
		std::string motionKindName;
	public:
		/**
		Konstruktor klasy MotionKind.
		*/
		MotionKind(int motionKindID, const std::string& motionKindName);
		/**
		Wirtualny destruktor klasy MotionKind.
		*/
		virtual ~MotionKind();
		/**
		Setter motionKindName
		@param motionKindName
		*/
		void setMotionKindName(const std::string& motionKindName);
		/**
		Getter motionKindName
		@return motionKindName
		*/
		const std::string& getMotionKindName() const;
		/**
		Metoda toString
		@return prezentuje obiekt w formie tekstowej
		**/
		virtual const std::string toString() const;
	};
}
#endif
