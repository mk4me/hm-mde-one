/**
@author Marek Daniluk
@brief Klasa MotionBasicQueriesService dziedzicz�ca po WsdlConnection wykonuj�ca operacje dokumentu MotionBasicQueriesService.motionWsdl.
*/

#ifndef HEADER_GUARD_COMMUNICATION_BASICQUERIESSERVICE_H__
#define HEADER_GUARD_COMMUNICATION_BASICQUERIESSERVICE_H__

#include <plugins/communication/WsdlConnection.h>
#include <plugins/communication/Entity.h>

namespace communication
{
    class GeneralBasicQueriesService : public WsdlConnection
    {
    public:
        /**
		Konstruktor klasy MotionBasicQueriesService.
		*/
		GeneralBasicQueriesService();
		/**
		Wirtualny destruktor klasy MotionBasicQueriesService.
		*/
		virtual ~GeneralBasicQueriesService();

        DateTime getLastDBModificationTime();
        DateTime getLastMetadataModificationTime();

        static DateTime dbTimeFormatExtractor(const std::string & time);
    };

	class MotionBasicQueriesService : public GeneralBasicQueriesService
	{
    public:
		/**
		Konstruktor klasy MotionBasicQueriesService.
		*/
		MotionBasicQueriesService();
		/**
		Wirtualny destruktor klasy MotionBasicQueriesService.
		*/
		virtual ~MotionBasicQueriesService();

		/**
		Listuje pr�by pomiarowe sesji.
		@param sessionID id sesji kt�rej pr�by pomiarowe maj� zosta� wylistowane
		@return zbi�r pr�b pomiarowych dla wybranej sesji
		*/
		std::vector<motionWsdl::Trial> listSessionTrials(int sessionID);
		/**
		Listuje sesje dla danego laboratorium.
		@param labID id labu kt�rego sesje maj� zosta� wylistowane
		@return zbi�r sesji dla wybranego labu
		*/
		std::vector<motionWsdl::Session> listLabSessionsWithAttributes(int labID);
		/**
		Listuje pliki dla encji typu subjectType i id ID.
		@param ID id encji dla kt�rej maj� zosta� wylistowane pliki
		@param subjectType typ encji dla kt�rej maj� zosta� wylistowane pliki
		@return zbi�r plik�w dla wybranej encji
		*/
		std::vector<motionWsdl::File> listFiles(int ID, const std::string& subjectType);
		/**
		TODO: Listuje encje bazodanowe, sesje, podleg�e sesjom pr�by pomiarowe i
		informacje o plikach poszczeg�lnych sesji i pr�b pomiarowych.
		@return lista pr�b pomiarowych, w przysz�o�ci powinno znale�� si� drzewko zale�no�ci mi�dzy encjami
		*/
		std::vector<motionWsdl::Trial> listSessionContents();
	};

    class MedicalBasicQueriesService : public GeneralBasicQueriesService
	{
    public:
		/**
		Konstruktor klasy MotionBasicQueriesService.
		*/
		MedicalBasicQueriesService();
		/**
		Wirtualny destruktor klasy MotionBasicQueriesService.
		*/
		virtual ~MedicalBasicQueriesService();

    };
}
#endif
