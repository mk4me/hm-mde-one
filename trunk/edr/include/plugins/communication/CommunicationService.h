/**
@author Marek Daniluk
@brief Klasa CommunicationService dostarcza funkcjonalnoœci interfejsu ICommunication do edytora EDR. Dziedziczy równie¿
po interfejsie IService dziêki czemu jest Communication Service jest dodawany jako plugin. Pe³ni funkcjê kontrolera dla
ca³ego serwisu korzystaj¹cego z MVC. Dostarcza widoku do³¹czanego jako dokowany panel do edytora i model odpowiedzialny za
implementacjê funkcjonalnoœci wymaganych przez serwis komunikacji.
*/

#ifndef HEADER_GUARD_COMMUNICATION_COMMUNICATIONSERVICE_H__
#define HEADER_GUARD_COMMUNICATION_COMMUNICATIONSERVICE_H__

#include <core/IService.h>
#include <plugins/communication/ICommunication.h>
#include <plugins/communication/CommunicationManager.h>
#include <plugins/communication/TransportWSDL_FTPS.h>
#include <plugins/communication/QueryWSDL.h>
#include <plugins/communication/TextParser.h>
#include "CommunicationWidget.h"

class CommunicationWidget;

class CommunicationService : public IService, public ICommunication
{
    UNIQUE_ID('COMM','SRVC');
public:
    typedef core::shared_ptr<TextParser> TextParserPtr;
private:
    /**
    Widok serwisu
    */
    CommunicationWidget* widget;
    /**
    Model serwisu
    */
    communication::CommunicationManager* model;
    /**
    WskaŸnik na klasê TransportWSDL_FTPS odpowiedzialn¹ za transport danych
    */
    communication::TransportWSDL_FTPS* transport;
    /**
    WskaŸnik na klasê QueryWSDL odpowiedzialn¹ za odpytywanie bazy danych
    */
    communication::QueryWSDL* query;
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
    /**
    Testowy parser tekstu
    */
    TextParserPtr tp;
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
    Metoda z interfejsu IService. Inicjalizacja us³ugi.
    @param serviceManager Manager us³ug.
    @param root Korzeñ wspólnej sceny 3D.
    @param dataManager Manager zasobów.
    */
    virtual AsyncResult init(IServiceManager* serviceManager, IDataManager* dataManager, osg::Node* sceneRoot, osgViewer::CompositeViewer* viewer);
    /**
    Metoda z interfejsu IService. £aduje.
    @param serviceManager Manager us³ug.
    @param dataManager Manager zasobów.
    */
    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);
    /**
    Metoda z interfejsu IService. Aktualizacja logiki us³ugi. Ten sam w¹tek co UI.
    */
    virtual AsyncResult update(double time, double timeDelta);
    /**
    Metoda z interfejsu IService. Us³uga nie musi mieæ wizualnej reprezentacji.
    @return Widget tworzony przez us³ugê b¹dŸ NULL.
    */
    virtual IWidget* getWidget()
    { 
        // HACK: ca³y ten system jest shackowany!
        //return reinterpret_cast<IWidget*>(widget);
        return NULL;
    }
    /**
    Metoda z interfejsu IService. Widget dostarczaj¹cy opcji zwi¹zanych z us³ug¹/us³ugami zale¿nymi.
    @return Widget tworzony przez us³ugê b¹dŸ NULL.
    */
    virtual IWidget* getSettingsWidget()
    {
        return reinterpret_cast<IWidget*>(widget);
    }
    /**
    Metoda z interfejsu IService. Nazwa us³ugi.
    @return nazwa us³ugi
    */
    virtual const std::string& getName() const
    {
        return name;
    }
    //ICommunication
    /**
    Metoda z interfejsu ICommunication. Pobieranie informacji o próbach pomiarowych z serwera.
    */
    virtual void updateSessionContents();
    /**
    Metoda z interfejsu ICommunication. Pobieranie wszystkich plików próby pomiarowej.
    @param trialID id triala w bazie danych którego pliki maj¹ byæ pobrane
    */
    virtual void downloadTrial(unsigned int trialID);
    /**
    Metoda z interfejsu ICommunication. Pobieranie pojedynczego pliku.
    @param fileID id pliku w bazie danych który ma byæ pobrany
    */
    virtual void downloadFile(unsigned int fileID);
    /**
    Metoda z interfejsu ICommunication. £adowanie lokalnej próby pomiarowej do edytora.
    @param localTrial lokalna próba pomiarowa do za³adowania
    */
    virtual void loadTrial(const IDataManager::LocalTrial& localTrial);
    /**
    Metoda z interfejsu ICommunication. Przerwanie pobierania pliku lub triala.
    */
    virtual void cancelDownloading();
    /**
    Odczyt danych prób pomiarowych z dysku lokalnego.
    */
    void load();
    /**
    Zapis danych prób pomiarowych na dysk lokalny.
    */
    void save();
    /**
    Zwraca wskaŸnik na instancjê klasy TransportWSDL_FTPS.
    @return wskaŸnik na instancjê odpowiedzialn¹ za transport danych
    */
    const communication::TransportWSDL_FTPS* getTransportManager() const;
    /**
    Zwraca wskaŸnik na instancjê klasy QueryWSDL.
    @return wskaŸnik na instancjê odpowiedzialn¹ za odpytywanie bazy danych
    */
    const communication::QueryWSDL* getQueryManager() const;
    /**
    Ustala dane u¿ytkownika wykorzystywane do logowania takie jak host, nazwa i has³o u¿ytkownika.
    Dane s³u¿¹ do autentykacji przy odpytywaniu bazy danych
    @param user nazwa u¿ytkownika
    @param password has³o u¿ytkownika
    @param bqsUri adres URI dokumentu BasicQueriesService
    @param busUri adres URI dokumentu BasicUpdatesService
    */
    void setQueryCredentials(const std::string& user, const std::string& password, const std::string& bqsUri, const std::string& busUri);
    /**
    Ustala dane u¿ytkownika wykorzystywane do logowania takie jak host, nazwa i has³o u¿ytkownika.
    Dane s³u¿¹ do autentykacji ftp przy pobieraniu danych
    @param user nazwa u¿ytkownika
    @param password has³o u¿ytkownika
    @param uri adres URI serwera ftp
    */
    void setTransportFTPCredentials(const std::string& user, const std::string& password, const std::string& uri);
    /**
    Ustala dane u¿ytkownika wykorzystywane do logowania takie jak host, nazwa i has³o u¿ytkownika.
    Dane s³u¿¹ do autentykacji wsdlowej przy pobieraniu danych
    @param user nazwa u¿ytkownika
    @param password has³o u¿ytkownika
    @param uri adres URI dokumentu FileStoremanService
    */
    void setTransportWSCredentials(const std::string& user, const std::string& password, const std::string& uri);
};
#endif
