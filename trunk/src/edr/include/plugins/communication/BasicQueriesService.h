/**
@author Marek Daniluk
@brief Klasa MotionBasicQueriesService dziedzicząca po WsdlConnection wykonująca operacje dokumentu MotionBasicQueriesService.motionWsdl.
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
		Listuje próby pomiarowe sesji.
		@param sessionID id sesji której próby pomiarowe mają zostać wylistowane
		@return zbiór prób pomiarowych dla wybranej sesji
		*/
		std::vector<motionWsdl::Trial> listSessionTrials(int sessionID);
		/**
		Listuje sesje dla danego laboratorium.
		@param labID id labu którego sesje mają zostać wylistowane
		@return zbiór sesji dla wybranego labu
		*/
		std::vector<motionWsdl::Session> listLabSessionsWithAttributes(int labID);
		/**
		Listuje pliki dla encji typu subjectType i id ID.
		@param ID id encji dla której mają zostać wylistowane pliki
		@param subjectType typ encji dla której mają zostać wylistowane pliki
		@return zbiór plików dla wybranej encji
		*/
		std::vector<motionWsdl::File> listFiles(int ID, const std::string& subjectType);
		/**
		TODO: Listuje encje bazodanowe, sesje, podległe sesjom próby pomiarowe i
		informacje o plikach poszczególnych sesji i prób pomiarowych.
		@return lista prób pomiarowych, w przyszłości powinno znaleźć się drzewko zależności między encjami
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
