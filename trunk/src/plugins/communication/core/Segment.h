/**
@author Marek Daniluk
@brief Klasa Segment reprezentuje obiekt Segment po stronie serwera. Klasa powstala na bazie
dokumentacji i kodu zrodlowego zespolu bazodanowego. Brak szczegolow w dokumentacji nie pozwala na
jednoznaczne ustalenie przydatnosci klasy, jednak jak wspomina zespol bazodanowy, funkcjonalnosc
komunikacji z baza danych wciaz jest w fazie testow.
*/

#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include <string>
#include "Entity.h"

namespace EDRCM {

	class Segment : public Entity {
	protected:
		int trialID;
		std::string segmentName;
		int startTime;
		int endTime;
	public:
		/**
		Konstruktor klasy Segment.
		*/
		Segment(int segmentID = 0, int trialID = 0, const std::string& segmentName = "", int startTime = 0, int endTime = 0);
		/**
		Wirtualny destruktor klasy Segment.
		*/
		virtual ~Segment();
		/**
		Setter trialID
		@param trialID
		*/
		void setTrialID(int trialID);
		/**
		Setter segmentName
		@param segmentName
		*/
		void setSegmentName(const std::string& segmentName);
		/**
		Setter startTime
		@param startTime
		*/
		void setStartTime(int startTime);
		/**
		Setter endTime
		@param endTime
		*/
		void setEndTime(int endTime);
		/**
		Getter trialID
		@return trialID
		*/
		const int getTrialID() const;
		/**
		Getter segmentName
		@return segmentName
		*/
		const std::string& getSegmentName() const;
		/**
		Getter startTime
		@return startTime
		*/
		const int getStartTime() const;
		/**
		Getter endTime
		@return endTime
		*/
		const int getEndTime() const;
		/**
		Metoda toString
		@return prezentuje obiekt w formie tekstowej
		**/
		virtual const std::string toString() const;
	};
}
#endif
