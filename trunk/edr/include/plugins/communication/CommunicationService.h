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
#include <plugins/communication/CommunicationWidgetEx.h>
#include <plugins/communication/ICommunication.h>
#include <plugins/communication/QueryWSDL.h>
#include <plugins/communication/TransportWSDL_FTPS.h>

class CommunicationWidgetEx;

class CommunicationService : public core::IService, public ICommunication
{
    UNIQUE_ID("{441D9894-1019-4382-97EE-F18B511A49CB}","Communication Service");
private:
    /**
    Widok serwisu
    */
    CommunicationWidgetEx* widget;
    /**
    Model serwisu
    */
    communication::CommunicationManager* model;
    /**
    Nazwa serwisu
    */
    std::string name;
    /**
    ping serwera
    */
    void ping()
    {
        model->ping();
    };
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
    virtual void init(core::IServiceManager* serviceManager, core::IDataManager* dataManager);
    /**
    Metoda z interfejsu IService. �aduje.
    @param serviceManager Manager us�ug.
    @param dataManager Manager zasob�w.
    */
    virtual void loadData(core::IServiceManager* serviceManager, core::IDataManager* dataManager);
    /**
    Metoda z interfejsu IService. Aktualizacja logiki us�ugi. Ten sam w�tek co UI.
    */
    virtual void update(double time, double timeDelta);
    /**
    Metoda z interfejsu IService. Us�uga nie musi mie� wizualnej reprezentacji.
    @return Widget tworzony przez us�ug� b�d� NULL.
    */
    virtual QWidget* getWidget()
    { 
        // HACK: ca�y ten system jest shackowany!
        //return reinterpret_cast<IWidget*>(widget);
        return nullptr;
    }
    /**
    Metoda z interfejsu IService. Widget dostarczaj�cy opcji zwi�zanych z us�ug�/us�ugami zale�nymi.
    @return Widget tworzony przez us�ug� b�d� NULL.
    */
    virtual QWidget* getSettingsWidget()
    {
        return (QWidget*)widget;
    }
    /**
    Metoda z interfejsu IService. Nazwa us�ugi.
    @return nazwa us�ugi
    */
    virtual const std::string& getName() const
    {
        return name;
    }
    //ICommunication
	/**
	Metoda z interfejsu ICommunication. P�ytka kopia bazy danych.
	*/
	virtual void copyDbData();
    /**
    Metoda z interfejsu ICommunication. Pobieranie wszystkich plik�w pr�by pomiarowej.
    @param trialID id triala w bazie danych kt�rego pliki maj� by� pobrane
    */
    virtual void downloadTrial(unsigned int trialID);
    /**
    Metoda z interfejsu ICommunication. Pobieranie pojedynczego pliku.
    @param fileID id pliku w bazie danych kt�ry ma by� pobrany
    */
    virtual void downloadFile(unsigned int fileID);
    /**
    Metoda z interfejsu ICommunication. �adowanie lokalnej pr�by pomiarowej do edytora.
    @param localTrial lokalna pr�ba pomiarowa do za�adowania
    */
    virtual void loadTrial(const core::IDataManager::LocalTrial& localTrial);
    /**
    Metoda z interfejsu ICommunication. �adowanie listy plik�w do edytora.
    @param files lista plik�w do za�adowania
    */
    virtual void loadFiles(const std::vector<core::IDataManager::Path> files);
    /**
    Metoda z interfejsu ICommunication. Usuwanie listy plik�w z edytora.
    @param files lista plik�w do usuni�cia
    */
    virtual void removeFiles(const std::vector<core::IDataManager::Path> files);
    /**
    Metoda z interfejsu ICommunication. Przerwanie pobierania pliku lub triala.
    */
    virtual void cancelDownloading();
};
#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONSERVICE_H__
