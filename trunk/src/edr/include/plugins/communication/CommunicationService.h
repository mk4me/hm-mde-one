/**
@author Marek Daniluk
@brief Klasa CommunicationService dostarcza funkcjonalno�ci interfejsu ICommunication do edytora EDR. Dziedziczy r�wnie�
po interfejsie IService dzi�ki czemu jest Communication Service jest dodawany jako plugin. Pe�ni funkcj� kontrolera dla
ca�ego serwisu korzystaj�cego z MVC. Dostarcza widoku do��czanego jako dokowany panel do edytora i model odpowiedzialny za
implementacj� funkcjonalno�ci wymaganych przez serwis komunikacji.
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
    /* Czy zako�czy� w�asny w�tek                                                                     */
    /************************************************************************/
    bool finish;
    ///************************************************************************/
    ///* Callbacki do CommunicationManagera                                                                     */
    ///************************************************************************/
    //communication::CommunicationManager::RequestCallbacks callbacks;
    /************************************************************************/
    /* Czas spania w�asnego w�tku aktualizuj�cego ping                                                                     */
    /************************************************************************/
    unsigned int sleepTime;
    ///************************************************************************/
    ///* Czy aktualizowa� stan �ci�gania plik�w                                                                     */
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
    Metoda z interfejsu IService. Inicjalizacja us�ugi.
    @param serviceManager Manager us�ug.
    @param root Korze� wsp�lnej sceny 3D.
    @param dataManager Manager zasob�w.
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
    Metoda z interfejsu IService. Us�uga nie musi mie� wizualnej reprezentacji.
    @return Widget tworzony przez us�ug� b�d� NULL.
    */
    virtual QWidget* getWidget(core::IActionsGroupManager * actions)
    {
        return nullptr;
    }
    /**
    Metoda z interfejsu IService. Widget dostarczaj�cy opcji zwi�zanych z us�ug�/us�ugami zale�nymi.
    @return Widget tworzony przez us�ug� b�d� NULL.
    */
    virtual QWidget* getSettingsWidget(core::IActionsGroupManager * actions)
    {
        //return widget;
        return nullptr;
    }
    /**
    Metoda z interfejsu IService. Nazwa us�ugi.
    @return nazwa us�ugi
    */
    virtual const std::string& getName() const
    {
        return name;
    }
 //   //ICommunication
	///**
	//Metoda z interfejsu ICommunication. P�ytka kopia bazy danych.
	//*/
	//virtual void copyMotionDbData();

 //   void copyMedicalDbData();
 //   /**
 //   Metoda z interfejsu ICommunication. Pobieranie wszystkich plik�w pr�by pomiarowej.
 //   @param trialID id triala w bazie danych kt�rego pliki maj� by� pobrane
 //   */
 //   virtual void downloadTrial(unsigned int trialID);

 //   virtual void downloadSession(unsigned int sessionID);

 //   virtual void downloadPerformer(unsigned int performerID);

 //   /**
 //   Metoda z interfejsu ICommunication. Pobieranie pojedynczego pliku.
 //   @param fileID id pliku w bazie danych kt�ry ma by� pobrany
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
