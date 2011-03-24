#ifndef HEADER_GUARD_COMMUNICATION_COMMUNICATIONWIDGETEX_H__
#define HEADER_GUARD_COMMUNICATION_COMMUNICATIONWIDGETEX_H__

#include <QtGui/QMenu>
#include <utils/ObserverPattern.h>
#include <plugins/communication/CommunicationManager.h>
#include <plugins/communication/CommunicationService.h>
#include <plugins/communication/Entity.h>
#include "ui_CommunicationWidgetEx.h"

class CommunicationService;
class CommunicationWidgetEx;

class IEntityTreeItem : public QTreeWidgetItem
{
public:
    virtual ~IEntityTreeItem() {};

    virtual int getEntityID() = 0;
};

class PerformerTreeItem : public IEntityTreeItem
{
    ShallowCopy::Performer performer;
public:
    PerformerTreeItem();

    virtual ~PerformerTreeItem() {};

    void setPerformer(const ShallowCopy::Performer& performer);

    virtual int getEntityID()
    {
        return performer.performerID;
    };
};

class SessionTreeItem : public IEntityTreeItem
{
    ShallowCopy::Session session;
public:
    SessionTreeItem();

    virtual ~SessionTreeItem() {};

    void setSession(const ShallowCopy::Session& session);

    virtual int getEntityID()
    {
        return session.sessionID;
    };
};

class TrialTreeItem : public IEntityTreeItem
{
    ShallowCopy::Trial trial;
public:
    TrialTreeItem();

    virtual ~TrialTreeItem() {};

    void setTrial(const ShallowCopy::Trial& trial, std::vector<core::IDataManager::LocalTrial>& localTrials);

    virtual int getEntityID()
    {
        return trial.trialID;
    };

    const QString getName() const;
};

/**
@author Marek Daniluk
@brief Klasa CommunicationWidgetEx pe³ni funkcjê widoku dla serwisu Communication. Korzysta z biblioteki Qt.
Jest to druga wersja widoku, bazuj¹ca na widgecie QTreeWidget. Widok dostarcza t¹ sam¹ funkcjonalnoœæ co poprzedni, czyli
listowanie triali, wyszczegó³nienie lokalnych i serwerowych prób pomiarowych, ³adowanie lokalnych prób pomiarowych, pobieranie
serwerowych triali. Dodatkow¹ funkcjonalnoœci¹ s¹ widoki wprowadzone wraz z nowym widokiem.
*/

class CommunicationWidgetEx : public QWidget, public Ui::CommunicationWidget, public utils::Observer<communication::CommunicationManager>
{
    Q_OBJECT
public:
    /**
    Konstruktor CommunicationWidgetEx
    */
    CommunicationWidgetEx(CommunicationService* service);
    /**
    Wirtualny destruktor CommunicationWidget
    */
    virtual ~CommunicationWidgetEx() {};
    /**
    Metoda aktualizuj¹ca dane widoku.
    @param subject dane pozwalaj¹ce zaktualizowaæ widok
    */
    virtual void update(const communication::CommunicationManager* subject);
    /**
    Ustala informacjê na etykiecie.
    @param info informacja do wyœwietlenia na etykiecie
    */
    void setInfoLabel(const std::string& info);
    /**
    Ustala wartoœæ procentow¹ w pasku pobierania.
    @param value wartoœæ do przedstawienia przez pasek pobierania
    */
    void setProgress(int value);
    /**
    @param online Czy aplikacja jest w trybie online?
    */
    void setOnline(bool isOnline)
    {
        this->isOnline = isOnline;
    }
    /**
    Ustawia widget w tryb pobierania, dodaje mo¿liwoœæ przerwania i blokuje pobieranie
    wiêcej ni¿ jednego triala na raz.
    @param isDownloading czy akcje pobierania powinny byæ zablokowane
    */
    void setDownloadingState(bool isDownloading)
    {
        if(isDownloading) {
            actionAbort_download->setVisible(true);
            actionDownload->setDisabled(true);
        } else {
            actionAbort_download->setVisible(false);
            actionDownload->setDisabled(false);
        }
    }

public slots:
    //widok performera
    void performerViewPressed(bool tog);
    //widok sesji
    void sessionViewPressed(bool tog);
    //widok proby pomiarowej
    void trialViewPressed(bool tog);
    //update triali
    void updatePressed();
    //ladowanie proby
    void loadPressed();
    //pobieranie proby
    void downloadPressed();
    //przerwanie pobierania proby
    void abortPressed();
    //menu dla widgetu
    void contextMenu(QPoint);
    //menu dla triala
    void trialContextMenu(QPoint);
    //wybrany trial
    void treeItemClicked(QTreeWidgetItem* item, int column);
private:
    /**
    WskaŸnik na kontroler serwisu
    */
    CommunicationService* communicationService;
    /**
    W³aœciwa metoda aktualizuj¹ca widok. powinna wykonywaæ siê w g³ównym w¹tku.
    */
    void refreshUI();
    /**
    Za³aduj trial do aplikacji.
    @param localTrial lista plików jednej próby pomiarowej do za³adowania.
    */
    void loadTrial(const core::IDataManager::LocalTrial& localTrial);
    /**
    Pobranie próby pomiarowej z serwera.
    @param trialID id encji trialowej do pobrania.
    */
    void downloadTrial(int trialID);
    /**
    Lokalna kopia danych zwi¹zana z problemem Qt i w¹tkami "z zewn¹trz".
    */
    ShallowCopy::ShallowCopy shallowCopy;
    /**
    Lokalna kopia danych zwi¹zana z problemem Qt i w¹tkami "z zewn¹trz".
    */
    MetaData::MetaData metaData;

    bool isOnline;
    /**
    Kopia prób pomiarowych znajduj¹cych siê lokalnie.
    */
    std::vector<core::IDataManager::LocalTrial> localTrials;

    bool isLocalTrialSelected;
    bool isServerTrialSelected;

    core::IDataManager::LocalTrial loadingTrial;
    int downloadingTrialID;
};
#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONWIDGETEX_H__
