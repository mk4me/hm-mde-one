/**
@author Marek Daniluk
@brief Klasa Session reprezentuje obiekt sesji po stronie serwera, zawierajac wyliczenie atrybutow
statycznych sesji. Klasa powstala na bazie dokumentacji i kodu zrodlowego zespolu bazodanowego. Brak szczegolow
w dokumentacji nie pozwala na jednoznaczne ustalenie przydatnosci klasy, jednak jak wspomina zespol bazodanowy,
funkcjonalnosc komunikacji z baza danych wciaz jest w fazie testow.
*/

#ifndef _SESSION_H_
#define _SESSION_H_

#include <string>
#include <vector>
#include "DateTime.h"
#include "Entity.h"
#include "Trial.h"
#include "EDRException.h"

namespace EDRCM {

	class Session : public Entity {
	protected:
		/**
		brak danych
		*/
		int motionKindID;
		/**
		brak danych
		*/
		int performerID;
		/**
		???
		*/
		DateTime sessionDate;
		/**
		Pole klasy przechowujace opis sesji
		*/
		std::string sessionDescription;
		/**
		Lista triali przydzielona sesji
		*/
		std::vector<Trial> trials;
	public:
		/**
		Konstruktor klasy Session.
		*/
		Session();
		Session(int sessionID, int motionKindID, int performerID, DateTime& sessionDate, const std::string& sessionDescription);
		/**
		Wirtualny destruktor klasy Session.
		*/
		virtual ~Session();
		/**
		Setter motionKindID
		@param motionKindID brak danych
		*/
		void setmotionKindID(int motionKindID);
		/**
		Setter performerID
		@param performerID ID performera
		*/
		void setPerformerID(int performerID);
		/**
		Setter sessionDate
		@param sessionDate brak danych
		*/
		void setSessionDate(DateTime sessionDate);
		/**
		Setter sessionDescription
		@param sessionDescription opis sesji
		*/
		void setSessionDescription(const std::string& sessionDescription);
		/**
		Getter motionKindID
		@return brak danych
		*/
		const int getmotionKindID() const;
		/**
		Getter performerID
		@return ID performera
		*/
		const int getPerformerID() const;
		/**
		Getter sessionDate
		@return brak danych
		*/
		const DateTime& getSessionDate() const;
		/**
		Getter sessionDescription
		@return opis sesji
		*/
		const std::string& getSessionDescription() const;

		std::vector<Trial>& getTrials();
		void setTrials(std::vector<Trial>& trials);

		/**
		Metoda toString
		@return prezentuje obiekt w formie tekstowej
		**/
		virtual const std::string toString() const;
	};
}
#endif
