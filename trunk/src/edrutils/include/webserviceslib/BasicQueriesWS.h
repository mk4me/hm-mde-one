/**
@author Marek Daniluk
@brief Klasa MotionBasicQueriesService dziedzicz�ca po WsdlConnection wykonuj�ca operacje dokumentu MotionBasicQueriesService.motionWsdl.
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

		//! \param connection Po��czenie przez kt�re b�dzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po��czenie przez kt�re jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po��czenie przez kt�re jest realizowany serwis
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

		//! \param connection Po��czenie przez kt�re b�dzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po��czenie przez kt�re jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po��czenie przez kt�re jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

		/**
		Listuje pr�by pomiarowe sesji.
		@param sessionID id sesji kt�rej pr�by pomiarowe maj� zosta� wylistowane
		@return zbi�r pr�b pomiarowych dla wybranej sesji
		*/
		virtual const std::vector<motionWsdl::Trial> listSessionTrials(int sessionID);
       
		/**
		Listuje sesje dla danego laboratorium.
		@param labID id labu kt�rego sesje maj� zosta� wylistowane
		@return zbi�r sesji dla wybranego labu
		*/
		virtual const std::vector<motionWsdl::Session> listLabSessionsWithAttributes(int labID);
        
		/**
		Listuje pliki dla encji typu subjectType i id ID.
		@param ID id encji dla kt�rej maj� zosta� wylistowane pliki
		@param subjectType typ encji dla kt�rej maj� zosta� wylistowane pliki
		@return zbi�r plik�w dla wybranej encji
		*/
		virtual const std::vector<motionWsdl::File> listFiles(int ID, const std::string& subjectType);
        
		/**
		TODO: Listuje encje bazodanowe, sesje, podleg�e sesjom pr�by pomiarowe i
		informacje o plikach poszczeg�lnych sesji i pr�b pomiarowych.
		@return lista pr�b pomiarowych, w przysz�o�ci powinno znale�� si� drzewko zale�no�ci mi�dzy encjami
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

		//! \param connection Po��czenie przez kt�re b�dzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po��czenie przez kt�re jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po��czenie przez kt�re jest realizowany serwis
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
