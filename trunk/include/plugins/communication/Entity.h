/**
@author Marek Daniluk
@brief Klasa Entity jest klasa bazowa dla obiektow reprezentujacych byty po stronie serwera.
*/

#ifndef _ENTITY_H_
#define _ENTITY_H_

namespace communication {

	class Entity {
	protected:
		/**
		Pole klasy przechowujace informacje o identyfikatorze
		*/
		int id;
	public:
		/**
		Konstruktor klasy Entity.
		@param ID identyfikator obiektu
		*/
		Entity(int id = 0);
		/**
		Wirtualny destruktor klasy Entity.
		*/
		virtual ~Entity();
		/**
		Setter ID
		@param ID identyfikator obiektu
		*/
		void setID(int id);
		/**
		Getter ID
		@return ID identyfikator obiektu
		*/
		const int getID() const;
		/**
		Czysto wirtualna metoda toString
		@return prezentuje obiekt w formie tekstowej
		**/
		virtual const std::string toString() const = 0;
		/**
		Operator wyjscia dla Entity pod cout
		*/
		friend std::ostream& operator <<(std::ostream& out, const Entity& e) {
			return out << e.toString();
		}
	};
}
#endif
