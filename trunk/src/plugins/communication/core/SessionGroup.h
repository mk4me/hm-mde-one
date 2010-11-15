/**
@author Marek Daniluk
@brief Klasa SessionGroup reprezentuje obiekt SessionGroup po stronie serwera. Klasa powstala na bazie
dokumentacji i kodu zrodlowego zespolu bazodanowego. Brak szczegolow w dokumentacji nie pozwala na
jednoznaczne ustalenie przydatnosci klasy, jednak jak wspomina zespol bazodanowy, funkcjonalnosc
komunikacji z baza danych wciaz jest w fazie testow.
*/

#ifndef _SESSIONGROUP_H_
#define _SESSIONGROUP_H_

#include <string>
#include "Entity.h"

namespace EDRCM {

	class SessionGroup : public Entity {
	protected:
		std::string sessionGroupName;
	public:
		/**
		Konstruktor klasy SessionGroup.
		*/
		SessionGroup(int sessionGroupID = 0, const std::string& sessionGroupName = "");
		/**
		Wirtualny destruktor klasy SessionGroup.
		*/
		virtual ~SessionGroup();
		/**
		Setter sessionGroupName
		@param sessionGroupName
		*/
		void setSessionGroupName(const std::string& sessionGroupName);
		/**
		Getter sessionGroupName
		@return sessionGroupName
		*/
		const std::string& getSessionGroupName() const;
		/**
		Metoda toString
		@return prezentuje obiekt w formie tekstowej
		**/
		virtual const std::string toString() const;
	};
}
#endif
