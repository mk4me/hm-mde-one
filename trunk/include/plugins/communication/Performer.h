/**
@author Marek Daniluk
@brief Klasa Performer reprezentuje obiekt performera po stronie serwera. Klasa powstala na bazie
dokumentacji i kodu zrodlowego zespolu bazodanowego. Brak szczegolow w dokumentacji nie pozwala na
jednoznaczne ustalenie przydatnosci klasy, jednak jak wspomina zespol bazodanowy, funkcjonalnosc
komunikacji z baza danych wciaz jest w fazie testow.
*/

#ifndef _PERFORMER_H_
#define _PERFORMER_H_

#include "Entity.h"

namespace communication {

	class Performer : public Entity {
	protected:
		/**
		Pole klasy przechowujace imie performera
		*/
		std::string name;
		/**
		Pole klasy przechowujace nazwisko performera
		*/
		std::string surname;
	public:
		/**
		Konstruktor klasy Performer.
		*/
		Performer(int performerID = 0, const std::string& name = "", const std::string& surname = "");
		/**
		Wirtualny destruktor klasy Performer.
		*/
		virtual ~Performer();
		/**
		Setter name
		@param name imie performera
		*/
		void setName(const std::string& name);
		/**
		Setter surname
		@param surname nazwisko performera
		*/
		void setSurname(const std::string& surname);
		/**
		Getter name
		@return imie performera
		*/
		const std::string& getName() const;
		/**
		Getter surname
		@return nazwisko performera
		*/
		const std::string& getSurname() const;
		/**
		Metoda toString
		@return prezentuje obiekt w formie tekstowej
		**/
		virtual const std::string toString() const;
	};
}
#endif
