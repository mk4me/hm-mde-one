/**
@author Marek Daniluk
@brief Klasa MotionBasicQueriesService dziedzicz¹ca po WsdlConnection wykonuj¹ca operacje dokumentu MotionBasicQueriesService.motionWsdl.
*/

#ifndef HEADER_GUARD_COMMUNICATION_BASICQUERIESSERVICE_H__
#define HEADER_GUARD_COMMUNICATION_BASICQUERIESSERVICE_H__

#include <webserviceslib/IBasicQueriesWS.h>
#include <webserviceslib/IWebService.h>
#include <webserviceslib/Entity.h>                                                                        

namespace webservices
{
    class GeneralBasicQueriesWS : public IGeneralBasicQueriesWS
    {
    public:
        /**
		Konstruktor klasy GeneralBasicQueriesWS.
		*/
		GeneralBasicQueriesWS(const WSConnectionPtr & connection = WSConnectionPtr());
		/**
		Wirtualny destruktor klasy GeneralBasicQueriesWS.
		*/
		virtual ~GeneralBasicQueriesWS();

		//! \param connection Po³¹czenie przez które bêdzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

        virtual const DateTime dataModificationTime();

        virtual const DateTime metadataModificationTime();

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
        
    };

	class MotionBasicQueriesWS : public IMotionBasicQueriesWS
	{
    public:
		/**
		Konstruktor klasy MotionBasicQueriesService.
		*/
		MotionBasicQueriesWS(const WSConnectionPtr & connection = WSConnectionPtr());
		/**
		Wirtualny destruktor klasy MotionBasicQueriesService.
		*/
		virtual ~MotionBasicQueriesWS();

		//! \param connection Po³¹czenie przez które bêdzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

		/**
		Listuje próby pomiarowe sesji.
		@param sessionID id sesji której próby pomiarowe maj¹ zostaæ wylistowane
		@return zbiór prób pomiarowych dla wybranej sesji
		*/
		virtual const std::vector<motionWsdl::Trial> listSessionTrials(int sessionID);
       
		/**
		Listuje sesje dla danego laboratorium.
		@param labID id labu którego sesje maj¹ zostaæ wylistowane
		@return zbiór sesji dla wybranego labu
		*/
		virtual const std::vector<motionWsdl::Session> listLabSessionsWithAttributes(int labID);
        
		/**
		Listuje pliki dla encji typu subjectType i id ID.
		@param ID id encji dla której maj¹ zostaæ wylistowane pliki
		@param subjectType typ encji dla której maj¹ zostaæ wylistowane pliki
		@return zbiór plików dla wybranej encji
		*/
		virtual const std::vector<motionWsdl::File> listFiles(int ID, const std::string& subjectType);
        
		/**
		TODO: Listuje encje bazodanowe, sesje, podleg³e sesjom próby pomiarowe i
		informacje o plikach poszczególnych sesji i prób pomiarowych.
		@return lista prób pomiarowych, w przysz³oœci powinno znaleŸæ siê drzewko zale¿noœci miêdzy encjami
		*/
		virtual const std::vector<motionWsdl::Trial> listSessionContents();
        

		virtual const DateTime dataModificationTime();

		virtual const DateTime metadataModificationTime();

	private:

		mutable OpenThreads::ReentrantMutex * mutex;

		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;

		GeneralBasicQueriesWS genBasicQueries;
	};

    class MedicalBasicQueriesWS : public IMedicalBasicQueriesWS
	{
    public:
		/**
		Konstruktor klasy MotionBasicQueriesService.
		*/
		MedicalBasicQueriesWS(const WSConnectionPtr & connection = WSConnectionPtr());
		/**
		Wirtualny destruktor klasy MotionBasicQueriesService.
		*/
		virtual ~MedicalBasicQueriesWS();

		//! \param connection Po³¹czenie przez które bêdzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

        virtual const DateTime dataModificationTime();

        virtual const DateTime metadataModificationTime();

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;

		GeneralBasicQueriesWS genBasicQueries;
    };
}
#endif
