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
@brief Klasa EntityTrialItem dziedziczy po QListWidgetItem i reprezentuje pr�b� pomiarow� w widgecie QListWidget.
*/

struct EntityTrialItem : public QListWidgetItem
{
    int session;
    int id;
};

/**
@author Marek Daniluk
@brief Klasa LocalTrialItem dziedziczy po QListWidgetItem i reprezentuje lokaln� pr�b� pomiarow� w widgecie QListWidget.
*/

struct LocalTrialItem : public QListWidgetItem
{
    IDataManager::LocalTrial localTrial;
    std::string name;
public:
    LocalTrialItem()
    {
        setTextColor(QColor(0, 0, 255));
    };
};

/**
@author Marek Daniluk
@brief Klasa CommunicationWidget pe�ni funkcj� widoku dla serwisu Communication. Korzysta z biblioteki Qt.
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
    Ustala warto�� procentow� w pasku pobierania.
    @parem value warto�� do przedstawienia przez pasek pobierania
    */
    void setProgress(int value);
    /**
    Ustala informacj� na etykiecie.
    @info informacja do wy�wietlenia na etykiecie
    */
    void setInfoLabel(const std::string& info);;
    /**
    Wy�wietla okno z komunikatem b��du.
    @param error komunikat b��du
    */
    void showErrorMessage(const std::string& error);
    /**
    Pobiera zaznaczon� z listy pr�b� pomiarow�. Pobieranie rozpocznie si� tylko gdy jest zaznaczona
    pr�ba pomiarowa nie znajduj�ca si� na lokalnym dysku.
    */
    void download();
    /**
    Przerywa pobieranie plik�w.
    */
    void abort();
    /**
    �aduje wybran� pr�b� pomiarow� do edytora EDR.
    @param localTrial lokalna pr�ba pomiarowa do za�adowania
    */
    void loadTrial(const IDataManager::LocalTrial& localTrial);
    /**
    Aktualizuje informacje o pr�bach pomiarowych.
    */
    void updateTrials();
    /**
    Metoda aktualizuj�ca dane widoku.
    @param subject dane pozwalaj�ce zaktualizowa� widok
    */
    virtual void update(const communication::CommunicationManager* subject);

    private slots:
        /**
        Slot klikni�cia przycisku aktualizacji.
        */
        void updateButtonClicked();
        /**
        Slot klikni�cia przycisku pobierania/�adowania pr�b pomiarowych.
        */
        void downloadButtonClicked();
        /**
        Slot klikni�cia przycisku anulowania pobierania.
        */
        void abortButtonClicked();
        /**
        Slot dwukliku na li�cie pr�b pomiarowych.
        */
        void itemDoubleClicked(QListWidgetItem*);
        /**
        Slot klikni�cia na li�cie pr�b pomiarowych.
        */
        void itemClicked(QListWidgetItem*);
private:
    /**
    Wska�nik na kontroler serwisu
    */
    CommunicationService* communicationService;
    /**
    Kopia pr�b pomiarowych znajduj�cych si� na serwerze.
    */
    std::vector<communication::Trial> serverTrials;
    /**
    Kopia pr�b pomiarowych znajduj�cych si� lokalnie.
    */
    std::vector<IDataManager::LocalTrial> localTrials;
    /**
    Informacja wykorzystywana przy etykietach.
    */
    std::string infoText;
    /**
    W�a�ciwa metoda aktualizuj�ca widok. powinna wykonywa� si� w g��wnym w�tku.
    */
    void refreshUI();
    /**
    Przycisk aktualizacji informacji o pr�bach pomiarowych
    */
    QPushButton* updateButton;
    /**
    Przycisk pobierania pr�b pomiarowych
    */
    QPushButton* downloadButton;
    /**
    Przycisk anulowania pobierania pr�by pomiarowej
    */
    QPushButton* abortButton;
    /**
    Lista z pr�bami pomiarowymi
    */
    QListWidget* trials;
    /**
    Pasek post�pu
    */
    QProgressBar* progressBar;
    /**
    Etykieta informacyjna nad list�
    */
    QLabel* aviableTrialsLabel;
    /**
    Etykieta informacyjna odno�nie aktualizacji i stanu widoku
    */
    QLabel* infoLabel;
    /**
    Serwer online/offline?
    */
    bool isOnline;
    /**
    Etykieta informacyjna odno�nie stanu serwera
    */
    QLabel* serverStateLabel;
};
#endif
