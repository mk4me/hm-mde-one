/**
@author Marek Daniluk
@brief Klasa CommunicationService dostarcza funkcjonalności interfejsu ICommunication do edytora EDR. Dziedziczy również
po interfejsie IService dzięki czemu jest Communication Service jest dodawany jako plugin. Pełni funkcję kontrolera dla
całego serwisu korzystającego z MVC. Dostarcza widoku dołączanego jako dokowany panel do edytora i model odpowiedzialny za
implementację funkcjonalności wymaganych przez serwis komunikacji.
*/

#ifndef HEADER_GUARD_COMMUNICATION_COMMUNICATIONSERVICE_H__
#define HEADER_GUARD_COMMUNICATION_COMMUNICATIONSERVICE_H__

#include <core/IService.h>
#include <plugins/communication/CommunicationManager.h>
//#include <plugins/communication/CommunicationWidgetEx.h>
#include <plugins/communication/ICommunication.h>
#include <plugins/communication/QueryWSDL.h>
#include <plugins/communication/TransportWSDL_FTPS.h>
//#include <QtCore/QObject>

//Q_DECLARE_METATYPE ( communication::CommunicationManager::BasicRequest )
//Q_DECLARE_METATYPE ( std::string )

class CommunicationService : /*public QObject,*/ public core::IService, /*public ICommunication,*/ private OpenThreads::Thread
{
    //Q_OBJECT;

    UNIQUE_ID("{441D9894-1019-4382-97EE-F18B511A49CB}","Communication Service");
private:
    /**
    Widok serwisu
    */
    //CommunicationWidgetEx* widget;
    /**
    Model serwisu
    */
    communication::CommunicationManager* model;
    /**
    Nazwa serwisu
    */
    std::string name;
    ///************************************************************************/
    ///* Aktualny stan modelu                                                                     */
    ///************************************************************************/
    //communication::CommunicationManager::State currentState;
    //communication::CommunicationManager::State previousState;

    /************************************************************************/
    /* Czy zakończyć własny wątek                                                                     */
    /************************************************************************/
    bool finish;
    ///************************************************************************/
    ///* Callbacki do CommunicationManagera                                                                     */
    ///************************************************************************/
    //communication::CommunicationManager::RequestCallbacks callbacks;
    /************************************************************************/
    /* Czas spania własnego wątku aktualizującego ping                                                                     */
    /************************************************************************/
    unsigned int sleepTime;
    ///************************************************************************/
    ///* Czy aktualizować stan ściągania plików                                                                     */
    ///************************************************************************/
    //bool refreshProgress;
    ///************************************************************************/
    ///* Aktualne info dla widgeta                                                                     */
    ///************************************************************************/
    //std::string currentLabel;

    //bool refreshDBView;

    //bool refreshWidgetState;

    //bool refreshDBConnectionState;

    //OpenThreads::Mutex currentWidgetStateMutex;

    //int currentRequestID;
    //int updatingRequestID;

    core::IManagersAccessor * managersAccessor;

private:

    //void onBeginRequest(const communication::CommunicationManager::BasicRequest & request);
    //void onEndRequest(const communication::CommunicationManager::BasicRequest & request);
    //void onCancelRequest(const communication::CommunicationManager::BasicRequest & request);
    //void onRequestError(const communication::CommunicationManager::BasicRequest & request, const std::string & error);

    // /**
    //ping serwera
    //*/
    //void ping()
    //{
    //    //model->ping(callbacks);
    //};

private:
    
    virtual void run(){        
        while(finish == false){
            if(model->requestsQueueEmpty() == true) {
                //pinguj co pol minuty
                //model->requestPing();
                communication::CommunicationManager::CompleteRequest req;
                req.request = model->createRequestPing();
                model->pushRequest(req);
            }
            microSleep(sleepTime);
        }
    }

    //void resetWidgetState();

public:
    /**
    Konstruktor
    */
    CommunicationService();
    /**
    Wirtualny destruktor
    */
    virtual ~CommunicationService();

    //IService
    /**
    Metoda z interfejsu IService. Inicjalizacja usługi.
    @param serviceManager Manager usług.
    @param root Korzeń wspólnej sceny 3D.
    @param dataManager Manager zasobów.
    */
    virtual void init(core::IManagersAccessor * managersAccessor);

    virtual void lateInit();

    virtual void finalize()
    {
        if(isRunning() == true){
            finish = true;
            join();
        }
    }

    virtual void update(double deltaTime);
    
    /**
    Metoda z interfejsu IService. Usługa nie musi mieć wizualnej reprezentacji.
    @return Widget tworzony przez usługę bądź NULL.
    */
    virtual QWidget* getWidget(core::IActionsGroupManager * actions)
    {
        return nullptr;
    }
    /**
    Metoda z interfejsu IService. Widget dostarczający opcji związanych z usługą/usługami zależnymi.
    @return Widget tworzony przez usługę bądź NULL.
    */
    virtual QWidget* getSettingsWidget(core::IActionsGroupManager * actions)
    {
        //return widget;
        return nullptr;
    }
    /**
    Metoda z interfejsu IService. Nazwa usługi.
    @return nazwa usługi
    */
    virtual const std::string& getName() const
    {
        return name;
    }
 //   //ICommunication
	///**
	//Metoda z interfejsu ICommunication. Płytka kopia bazy danych.
	//*/
	//virtual void copyMotionDbData();

 //   void copyMedicalDbData();
 //   /**
 //   Metoda z interfejsu ICommunication. Pobieranie wszystkich plików próby pomiarowej.
 //   @param trialID id triala w bazie danych którego pliki mają być pobrane
 //   */
 //   virtual void downloadTrial(unsigned int trialID);

 //   virtual void downloadSession(unsigned int sessionID);

 //   virtual void downloadPerformer(unsigned int performerID);

 //   /**
 //   Metoda z interfejsu ICommunication. Pobieranie pojedynczego pliku.
 //   @param fileID id pliku w bazie danych który ma być pobrany
 //   */
 //   virtual void downloadFile(unsigned int fileID);
 //   
 //   /**
 //   Metoda z interfejsu ICommunication. Przerwanie pobierania pliku lub triala.
 //   */
 //   virtual void cancelDownloading();
};

typedef boost::shared_ptr<CommunicationService> CommunicationServicePtr;
typedef boost::shared_ptr<const CommunicationService> CommunicationServiceConstPtr;

#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONSERVICE_H__
