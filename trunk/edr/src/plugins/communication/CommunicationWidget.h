#ifndef COMMUNICATION_WIDGET_H
#define COMMUNICATION_WIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QtGui/QProgressBar>
#include <QtGui/QListWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QFormLayout>
#include <QtGui/QDialogButtonBox>
#include <utils/ObserverPattern.h>
#include <plugins/communication/CommunicationManager.h>
#include <plugins/communication/CommunicationService.h>
#include <plugins/communication/Entity.h>

class CommunicationService;

/**
@author Marek Daniluk
@brief Klasa EntityTrialItem dziedziczy po QListWidgetItem i reprezentuje próbê pomiarow¹ w widgecie QListWidget.
*/

class EntityTrialItem : public QListWidgetItem
{
private:
    int session;
    int id;
public:
    EntityTrialItem() {session = 0;};
    void setSession(int session) {this->session = session;};
    int getSession() {return session;};
    void setID(int ID) {this->id = ID;};
    int getID() {return id;};
};

/**
@author Marek Daniluk
@brief Klasa LocalTrialItem dziedziczy po QListWidgetItem i reprezentuje lokaln¹ próbê pomiarow¹ w widgecie QListWidget.
*/

class LocalTrialItem : public QListWidgetItem
{
private:
    std::string path;
    std::string name;
public:
    LocalTrialItem() {setTextColor(QColor(0, 0, 255));};
    void setPath(const std::string& path) {this->path = path;};
    const std::string& getPath() const {return path;};
    void setName(const std::string& name) {this->name = name;};
    const std::string& getName() const {return name;};
};

/**
@author Marek Daniluk
@brief Klasa CommunicationWidget pe³ni funkcjê widoku dla serwisu Communication. Korzysta z biblioteki Qt.
*/

class CommunicationWidget : public QWidget, public utils::Observer<communication::CommunicationManager>
{
    Q_OBJECT
public:
    /**
    Konstruktor CommunicationWidget
    */
    CommunicationWidget(CommunicationService* service);
    /**
    Wirtualny destruktor CommunicationWidget
    */
    virtual ~CommunicationWidget();
    /**
    Ustala wartoœæ procentow¹ w pasku pobierania.
    @parem value wartoœæ do przedstawienia przez pasek pobierania
    */
    void setProgress(int value);
    /**
    Ustala informacjê na etykiecie.
    @info informacja do wyœwietlenia na etykiecie
    */
    void setInfoLabel(const std::string& info);;
    /**
    Wyœwietla okno z komunikatem b³êdu.
    @param error komunikat b³êdu
    */
    void showErrorMessage(const std::string& error);
    /**
    Pobiera zaznaczon¹ z listy próbê pomiarow¹. Pobieranie rozpocznie siê tylko gdy jest zaznaczona
    próba pomiarowa nie znajduj¹ca siê na lokalnym dysku.
    */
    void download();
    /**
    Przerywa pobieranie plików.
    */
    void abort();
    /**
    £aduje wybran¹ próbê pomiarow¹ do edytora EDR.
    @param name nazwa lokalnej próby pomiarowej do za³adowania
    */
    void loadTrial(const std::string& name);
    /**
    Aktualizuje informacje o próbach pomiarowych.
    */
    void updateTrials();
    /**
    Metoda aktualizuj¹ca dane widoku.
    @param subject dane pozwalaj¹ce zaktualizowaæ widok
    */
    virtual void update(const communication::CommunicationManager* subject);

    private slots:
        /**
        Slot klikniêcia przycisku aktualizacji.
        */
        void updateButtonClicked();
        /**
        Slot klikniêcia przycisku pobierania/³adowania prób pomiarowych.
        */
        void downloadButtonClicked();
        /**
        Slot klikniêcia przycisku anulowania pobierania.
        */
        void abortButtonClicked();
        /**
        Slot dwukliku na liœcie prób pomiarowych.
        */
        void itemDoubleClicked(QListWidgetItem*);
        /**
        Slot klikniêcia na liœcie prób pomiarowych.
        */
        void itemClicked(QListWidgetItem*);
private:
    /**
    WskaŸnik na kontroler serwisu
    */
    CommunicationService* communicationService;
    /**
    Kopia prób pomiarowych znajduj¹cych siê na serwerze.
    */
    std::vector<communication::Trial> serverTrials;
    /**
    Kopia prób pomiarowych znajduj¹cych siê lokalnie.
    */
    std::vector<LocalTrial> localTrials;
    /**
    Informacja wykorzystywana przy etykietach.
    */
    std::string infoText;
    /**
    W³aœciwa metoda aktualizuj¹ca widok. powinna wykonywaæ siê w g³ównym w¹tku.
    */
    void refreshUI();
    /**
    Przycisk aktualizacji informacji o próbach pomiarowych
    */
    QPushButton* updateButton;
    /**
    Przycisk pobierania prób pomiarowych
    */
    QPushButton* downloadButton;
    /**
    Przycisk anulowania pobierania próby pomiarowej
    */
    QPushButton* abortButton;
    /**
    Lista z próbami pomiarowymi
    */
    QListWidget* trials;
    /**
    Pasek postêpu
    */
    QProgressBar* progressBar;
    /**
    Etykieta informacyjna nad list¹
    */
    QLabel* aviableTrialsLabel;
    /**
    Etykieta informacyjna odnoœnie aktualizacji i stanu widoku
    */
    QLabel* infoLabel;
    /**
    Serwer online/offline?
    */
    bool isOnline;
    /**
    Etykieta informacyjna odnoœnie stanu serwera
    */
    QLabel* serverStateLabel;
};
#endif
