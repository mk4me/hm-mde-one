/**
@author Marek Daniluk
@brief Klasa MotionBasicUpdatesService dziedzicz�ca po WsdlConnection wykonuj�ca operacje dokumentu MotionBasicUpdatesService.wsdl.
*/

#ifndef HEADER_GUARD_COMMUNICATION_BASICUPDATESSERVICE_H__
#define HEADER_GUARD_COMMUNICATION_BASICUPDATESSERVICE_H__

#include <webserviceslib/IBasicUpdatesWS.h>
#include <webserviceslib/IWebService.h>
#include <webserviceslib/Entity.h>

namespace webservices
{

	class MotionBasicUpdatesWS : public IMotionBasicUpdatesWS
	{
	public:
		/**
		Konstruktor klasy MotionBasicUpdatesService.
		*/
		MotionBasicUpdatesWS(const WSConnectionPtr & connection = WSConnectionPtr());
		/**
		Wirtualny destruktor klasy MotionBasicUpdatesService.
		*/
		virtual ~MotionBasicUpdatesWS();

		//! \param connection Po��czenie przez kt�re b�dzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po��czenie przez kt�re jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po��czenie przez kt�re jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;
		/**
		Manualne utworzenie encji performera w oparciu o dane z aplikacji klienckiej
		@param name imi� performera
		@param surname nazwisko performera
		@return utworzony Performer
		*/
		//wsdl::Performer createPerformer(const std::string& name = "", const std::string& surname = "");
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

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
	};

    class MedicalBasicUpdatesWS : public IMedicalBasicUpdatesWS
    {
    public:
        MedicalBasicUpdatesWS(const WSConnectionPtr & connection = WSConnectionPtr());
        virtual ~MedicalBasicUpdatesWS();

		//! \param connection Po��czenie przez kt�re b�dzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po��czenie przez kt�re jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po��czenie przez kt�re jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
    };
}
#endif
