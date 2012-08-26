/**
@author Marek Daniluk
@brief Klasa MotionBasicQueriesService dziedzicząca po WsdlConnection wykonująca operacje dokumentu MotionBasicQueriesService.motionWsdl.
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

		//! \param connection Połączenie przez które będzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Połączenie przez które jest realizowany serwis
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

		//! \param connection Połączenie przez które będzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

		/**
		Listuje próby pomiarowe sesji.
		@param sessionID id sesji której próby pomiarowe mają zostać wylistowane
		@return zbiór prób pomiarowych dla wybranej sesji
		*/
		virtual const std::vector<motionWsdl::Trial> listSessionTrials(int sessionID);
       
		/**
		Listuje sesje dla danego laboratorium.
		@param labID id labu którego sesje mają zostać wylistowane
		@return zbiór sesji dla wybranego labu
		*/
		virtual const std::vector<motionWsdl::Session> listLabSessionsWithAttributes(int labID);
        
		/**
		Listuje pliki dla encji typu subjectType i id ID.
		@param ID id encji dla której mają zostać wylistowane pliki
		@param subjectType typ encji dla której mają zostać wylistowane pliki
		@return zbiór plików dla wybranej encji
		*/
		virtual const std::vector<motionWsdl::File> listFiles(int ID, const std::string& subjectType);
        
		/**
		TODO: Listuje encje bazodanowe, sesje, podległe sesjom próby pomiarowe i
		informacje o plikach poszczególnych sesji i prób pomiarowych.
		@return lista prób pomiarowych, w przyszłości powinno znaleźć się drzewko zależności między encjami
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

		//! \param connection Połączenie przez które będzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Połączenie przez które jest realizowany serwis
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
