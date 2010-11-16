/**
@author Marek Daniluk
@brief Klasa Trial reprezentuje obiekt Trial po stronie serwera. Klasa powstala na bazie
dokumentacji i kodu zrodlowego zespolu bazodanowego. Brak szczegolow w dokumentacji nie pozwala na
jednoznaczne ustalenie przydatnosci klasy, jednak jak wspomina zespol bazodanowy, funkcjonalnosc
komunikacji z baza danych wciaz jest w fazie testow.
*/

#ifndef _TRIAL_H_
#define _TRIAL_H_

#include "Entity.h"
#include "File.h"
#include <plugins/communication/EDRException.h>

namespace communication {

	class Trial : public Entity {
	protected:
		std::string trialDescription;
		int duration;
		std::vector<File> files;
	public:
		/**
		Konstruktor klasy Trial.
		*/
		Trial(int trialID = 0, const std::string& trialDescription = "", int duration = 0);
		/**
		Wirtualny destruktor klasy Trial.
		*/
		virtual ~Trial();
		/**
		Setter trialDescription
		@param trialDescription
		*/
		void setTrialDescription(const std::string& trialDescription);
		/**
		Setter duration
		@param duration
		*/
		void setDuration(int duration);
		/**
		Getter trialDescription
		@return trialDescription
		*/
		const std::string& getTrialDescription() const;
		/**
		Getter duration
		@return duration
		*/
		const int getDuration() const;

		std::vector<File>& getFiles();
		void setFiles(std::vector<File>& files);

		/**
		Metoda toString
		@return prezentuje obiekt w formie tekstowej
		**/
		virtual const std::string toString() const;
	};
}
#endif
