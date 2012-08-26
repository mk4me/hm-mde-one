#ifndef HEADER_GUARD_COMMUNICATION_COMMUNICATIONWIDGETEX_H__
#define HEADER_GUARD_COMMUNICATION_COMMUNICATIONWIDGETEX_H__

#include <QtGui/QMenu>
#include <utils/ObserverPattern.h>
#include <plugins/communication/CommunicationManager.h>
#include <plugins/communication/Entity.h>
#include "ui_CommunicationWidgetEx.h"

class CommunicationService;

/**
@author Marek Daniluk
@brief Klasa CommunicationWidgetEx pełni funkcję widoku dla serwisu Communication. Korzysta z biblioteki Qt.
Jest to druga wersja widoku, bazująca na widgecie QTreeWidget. Widok dostarcza tą samą funkcjonalność co poprzedni, czyli
listowanie triali, wyszczegółnienie lokalnych i serwerowych prób pomiarowych, ładowanie lokalnych prób pomiarowych, pobieranie
serwerowych triali. Dodatkową funkcjonalnością są widoki wprowadzone wraz z nowym widokiem.
*/
class CommunicationWidgetEx : public QWidget, public Ui::CommunicationWidget, public utils::Observer<communication::CommunicationManager>
{
    Q_OBJECT

struct TrialRelation
{
    TrialRelation()
    {};
    communication::ShallowCopy::Trial trial;
};

typedef core::shared_ptr<TrialRelation> TrialRelationPtr;
//typedef std::vector<TrialRelationPtr> TrialsRelations;
typedef std::map<int, TrialRelationPtr> TrialsRelations;

struct SessionRelation
{
    SessionRelation()
    {};
    communication::ShallowCopy::Session session;
    TrialsRelations trials;
};

typedef core::shared_ptr<SessionRelation> SessionRelationPtr;
//typedef std::vector<SessionRelationPtr> SessionsRelations;
typedef std::map<int, SessionRelationPtr> SessionsRelations;

struct PerformerRelation
{
    PerformerRelation()
    {};
    communication::ShallowCopy::Performer performer;
    SessionsRelations sessions;
};

typedef core::shared_ptr<PerformerRelation> PerformerRelationPtr;
//typedef std::vector<PerformerRelationPtr> PerformersRelations;
typedef std::map<int,PerformerRelationPtr> PerformersRelations;

/**
@author Marek Daniluk
@brief Klasa IEntityTreeItem dziedziczy po QTreeWidgetItem i rozszerza możliwości elementów drzewa o menu
kontekstowe i dynamiczną zmianę liczby i nazw kolumn nagłówka QTreeWidget. Ponieważ reprezentuje dane
powiązane z płytką kopią bazą danych opierającą się na wielu typach, klasa jest abstrakcyjna.
*/
class IEntityTreeItem : public QTreeWidgetItem
{
protected:
    QMenu* menu;
    QStringList headers;
public:
    virtual ~IEntityTreeItem() {};
    
    virtual int getEntityID() = 0;

    void setMenu(QMenu* menu)
    {
        this->menu = menu;
    };

    QMenu* getMenu()
    {
        return menu;
    };

    const QStringList& getHeaders() const
    {
        return headers;
    };
};

class PerformerTreeItem : public IEntityTreeItem
{
    const communication::ShallowCopy::Performer * performer;
public:
    PerformerTreeItem(const communication::ShallowCopy::Performer * performer);

    virtual ~PerformerTreeItem() {};

    virtual int getEntityID()
    {
        return performer->performerID;
    };
};

class SessionTreeItem : public IEntityTreeItem
{
    const communication::ShallowCopy::Session * session;
public:
    SessionTreeItem(const communication::ShallowCopy::Session * session);

    virtual ~SessionTreeItem() {};

    virtual int getEntityID()
    {
        return session->sessionID;
    };
};

class TrialTreeItem : public IEntityTreeItem
{
protected:
    const communication::ShallowCopy::Trial * trial;
public:
    TrialTreeItem(const communication::ShallowCopy::Trial * trial);

    virtual ~TrialTreeItem() {};

    virtual int getEntityID()
    {
        return trial->trialID;
    };

    const communication::ShallowCopy::Trial * getTrial()
    {
        return trial;
    };

    const QString getName() const;
};

public:
    /**
    Konstruktor CommunicationWidgetEx
    */
    CommunicationWidgetEx(CommunicationService* service);
    /**
    Wirtualny destruktor CommunicationWidget
    */
    virtual ~CommunicationWidgetEx();
    /**
    Metoda aktualizująca dane widoku.
    @param subject dane pozwalające zaktualizować widok
    */
    virtual void update(const communication::CommunicationManager* subject);
    /**
    Ustala informację na etykiecie.
    @param info informacja do wyświetlenia na etykiecie
    */
    void setInfoLabel(const std::string& info);
    /**
    Ustala wartość procentową w pasku pobierania.
    @param value wartość do przedstawienia przez pasek pobierania
    */
    void setProgress(int value);
    /**
    @param online Czy aplikacja jest w trybie online?
    */
    void setOnline(bool isOnline)
    {
        if(!isOnline) {
            actionDownload->setVisible(false);
            actionUpdate->setVisible(false);
        } else {
            actionDownload->setVisible(true);
            actionUpdate->setVisible(true);
        }
    }
    /**
    Właściwa metoda aktualizująca widok. powinna wykonywać się w głównym wątku.
    */
    void refreshUI();

public slots:
    //widok performera
    void performerViewPressed(bool tog);
    //widok sesji
    void sessionViewPressed(bool tog);
    //widok proby pomiarowej
    void trialViewPressed(bool tog);
    //widok tylko lokalnych zasobów
    void localViewPressed(bool tog);
    //update triali
    void updatePressed();
    //ladowanie proby
    void loadPressed();
    //pobieranie proby
    void downloadPressed();
    //przerwanie pobierania proby
    void abortPressed();
    //menu ogólne dla widgetów
    void contextMenu(QPoint);
    //menu dla triala
    void trialContextMenu(QPoint);
    //
    void treeItemClicked(QTreeWidgetItem* item, int column);
private:
    /**
    Wskaźnik na kontroler serwisu
    */
    CommunicationService* communicationService;

    /**
    Załaduj pliki do aplikacji.
    @param files lista plików do załadowania po filtracji.
    */
    void loadFiles(const std::vector<core::Filesystem::Path>& files);
    /**
    Pobranie próby pomiarowej z serwera.
    @param trialID id encji trialowej do pobrania.
    */
    void downloadTrial(int trialID);
    /**
    Buduj widok performera
    */
    void buildPerformerView(QTreeWidget* tree);
    /**
    Buduj widok sesji
    */
    void buildSessionView(QTreeWidget* tree);
    /**
    Buduj widok próby pomiarowej
    */
    void buildTrialView(QTreeWidget* tree);
    /**
    Buduj widok lokalnych zasobów
    */
    void buildLocalView(QTreeWidget* tree);
    /**
    Tworzy widok triala
    */
    TrialTreeItem* createTrialItem(const communication::ShallowCopy::Trial * trial);
    /**
    Odśwież headery drzewa danych. Działa tylko gdy drzewo posiada same elementy IEntityTreeItem
    */
    void refreshHeader(QTreeWidget* tree);
    /**
    Dane z płytkiej kopii DB powiązane relacjami.
    */
    PerformersRelations performersWithRelations;
    /**
    Lokalna kopia danych związana z problemem Qt i wątkami "z zewnątrz".
    */
    communication::MetaData::MetaData metaData;
    /**
    Kopia prób pomiarowych znajdujących się lokalnie.
    */
    //std::vector<core::IDataManager::LocalTrial> localTrials;
    /**
    menu kontekstowe dla poszczególnych elementów ui.
    */
    QMenu* menuTl;
    QMenu* menuTs;
    //QMenu* menu;
    QMenu* view;
    QActionGroup* formatGroup;
    /**
    Obecnie widoczny view CS.
    */
    QTreeWidget* currentView;
    /**
    */

    ShallowCopyConstPtr constShallowCopy;
};
#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONWIDGETEX_H__
