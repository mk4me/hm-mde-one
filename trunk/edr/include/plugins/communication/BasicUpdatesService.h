/**
@author Marek Daniluk
@brief Klasa BasicUpdatesService dziedzicz�ca po WSDL_Wsdlpull wykonuj�ca operacje dokumentu BasicUpdatesService.wsdl.
*/

#ifndef _BASICUPDATESSERVICE_H_
#define _BASICUPDATESSERVICE_H_

#include "WSDL_Wsdlpull.h"
#include "Entity.h"

namespace communication
{

	class BasicUpdatesService : public WSDL_Wsdlpull
	{
	protected:
	public:
		/**
		Konstruktor klasy BasicUpdatesService.
		*/
		BasicUpdatesService();
		/**
		Wirtualny destruktor klasy BasicUpdatesService.
		*/
		virtual ~BasicUpdatesService();
		/**
		Manualne utworzenie encji performera w oparciu o dane z aplikacji klienckiej
		@param name imi� performera
		@param surname nazwisko performera
		@return wska�nik na obiekt klasy Performer
		*/
		Performer* createPerformer(const std::string& name = "", const std::string& surname = "");
		/**
		Manualne utworzenie encji sesji przez u�ytkownika (oznaczone TODO)
		@param userID identyfikator u�ytkownika
		@param labID identyfikator laboratorium
		@param performerID identyfikator performera
		@param sessionDate data powstania sesji
		@param sessionDescription opis sesji
		@param motionKindName ???
		@param groupsIDs referencja do wektora przechowuj�cego identyfikatory grup do kt�rych ma nale�e� sesja
		@return wska�nik na obiekt klasy Session
		*/
		//Session* createSession(int labID, int performerID, DateTime& sessionDate, const std::string& sessionDescription, const std::string& motionKindName, std::vector<int>& groupsIDs);
		///**
		//TODO
		//*/
		//Trial* createTrial(int sessionID, const std::string& trialDescription, int trialDuration);
		///**
		//TODO
		//*/
		//Segment* defineTrialSegment(int trialID, const std::string& segmentName, int startTime, int endTime);
		///**
		//TODO
		//*/
		//int setPerformerAttribute(int performerID, const std::string& attributeName, const std::string& attributeVal, bool update);
		///**
		//TODO
		//*/
		//int setSessionAttribute(int sessionID, const std::string& attributeName, const std::string& attributeVal, bool update);
		///**
		//TODO
		//*/
		//int setTrialAttribute(int trialID, const std::string& attributeName, const std::string& attributeVal, bool update);
		///**
		//TODO
		//*/
		//int setFileAttribute(int fileID, const std::string& attributeName, const std::string& attributeVal, bool update);
		///**
		//TODO
		//*/
		//int setSegmentAttribute(int segmentID, const std::string& attributeName, const std::string& attributeVal, bool update);
		///**
		//TODO
		//*/
		//bool assignSessionToGroup(int sessionID, int groupID);
	};
}
#endif
