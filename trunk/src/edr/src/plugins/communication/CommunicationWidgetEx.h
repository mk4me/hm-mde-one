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
@brief Klasa CommunicationWidgetEx pe³ni funkcjê widoku dla serwisu Communication. Korzysta z biblioteki Qt.
Jest to druga wersja widoku, bazuj¹ca na widgecie QTreeWidget. Widok dostarcza t¹ sam¹ funkcjonalnoœæ co poprzedni, czyli
listowanie triali, wyszczegó³nienie lokalnych i serwerowych prób pomiarowych, ³adowanie lokalnych prób pomiarowych, pobieranie
serwerowych triali. Dodatkow¹ funkcjonalnoœci¹ s¹ widoki wprowadzone wraz z nowym widokiem.
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
@brief Klasa IEntityTreeItem dziedziczy po QTreeWidgetItem i rozszerza mo¿liwoœci elementów drzewa o menu
kontekstowe i dynamiczn¹ zmianê liczby i nazw kolumn nag³ówka QTreeWidget. Poniewa¿ reprezentuje dane
powi¹zane z p³ytk¹ kopi¹ baz¹ danych opieraj¹c¹ siê na wielu typach, klasa jest abstrakcyjna.
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
        if(!isOnline) {
            actionDownload->setVisible(false);
            actionUpdate->setVisible(false);
        } else {
            actionDownload->setVisible(true);
            actionUpdate->setVisible(true);
        }
    }
    /**
    W³aœciwa metoda aktualizuj¹ca widok. powinna wykonywaæ siê w g³ównym w¹tku.
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
    WskaŸnik na kontroler serwisu
    */
    CommunicationService* communicationService;

    /**
    Za³aduj pliki do aplikacji.
    @param files lista plików do za³adowania po filtracji.
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
    Odœwie¿ headery drzewa danych. Dzia³a tylko gdy drzewo posiada same elementy IEntityTreeItem
    */
    void refreshHeader(QTreeWidget* tree);
    /**
    Dane z p³ytkiej kopii DB powi¹zane relacjami.
    */
    PerformersRelations performersWithRelations;
    /**
    Lokalna kopia danych zwi¹zana z problemem Qt i w¹tkami "z zewn¹trz".
    */
    communication::MetaData::MetaData metaData;
    /**
    Kopia prób pomiarowych znajduj¹cych siê lokalnie.
    */
    std::vector<core::IDataManager::LocalTrial> localTrials;
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
