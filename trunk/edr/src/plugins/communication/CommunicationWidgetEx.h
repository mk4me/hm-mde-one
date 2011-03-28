#ifndef HEADER_GUARD_COMMUNICATION_COMMUNICATIONWIDGETEX_H__
#define HEADER_GUARD_COMMUNICATION_COMMUNICATIONWIDGETEX_H__

#include <QtGui/QMenu>
#include <utils/ObserverPattern.h>
#include <plugins/communication/CommunicationManager.h>
#include <plugins/communication/CommunicationService.h>
#include <plugins/communication/Entity.h>
#include "ui_CommunicationWidgetEx.h"

class CommunicationService;

typedef std::pair<bool, ShallowCopy::Performer> PerformerMark;
typedef std::vector<PerformerMark> PerformersMarks;

typedef std::pair<bool, ShallowCopy::Session> SessionMark;
typedef std::vector<SessionMark> SessionsMarks;

typedef std::pair<bool, ShallowCopy::Trial> TrialMark;
typedef std::vector<TrialMark> TrialsMarks;

struct EntityRelations
{
    EntityRelations(bool markAll, bool markC3ds, bool markVideos, bool markAmcs)
        : markAll(markAll), markC3ds(markC3ds), markVideos(markVideos), markAmcs(markAmcs) { };
    bool markAll;
    bool markC3ds;
    bool markVideos;
    bool markAmcs;
};

struct TrialRelation : public EntityRelations
{
    TrialRelation()
        : EntityRelations(false, false, false, false) {};
    ShallowCopy::Trial trial;
};

typedef core::shared_ptr<TrialRelation> TrialRelationPtr;
typedef std::vector<TrialRelationPtr> TrialsRelations;

struct SessionRelation : public EntityRelations
{
    SessionRelation()
        : EntityRelations(false, false, false, false) {};
    ShallowCopy::Session session;
    TrialsRelations trials;
};

typedef core::shared_ptr<SessionRelation> SessionRelationPtr;
typedef std::vector<SessionRelationPtr> SessionsRelations;

struct PerformerRelation : public EntityRelations
{
    PerformerRelation()
        : EntityRelations(false, false, false, false) {};
    ShallowCopy::Performer performer;
    SessionsRelations sessions;
};

typedef core::shared_ptr<PerformerRelation> PerformerRelationPtr;
typedef std::vector<PerformerRelationPtr> PerformersRelations;

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

    virtual void setMarkedAll(bool mark) = 0;

    virtual bool isMarkedAll() const = 0;

    virtual void setMarkedC3ds(bool mark) = 0;

    virtual bool isMarkedC3ds() const = 0;

    virtual void setMarkedVideos(bool mark) = 0;

    virtual bool isMarkedVideos() const = 0;

    virtual void setMarkedAmcs(bool mark) = 0;

    virtual bool isMarkedAmcs() const = 0;

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
    PerformerRelationPtr performer;
public:
    PerformerTreeItem(PerformerRelationPtr performer);

    virtual ~PerformerTreeItem() {};

    //void setPerformer(const PerformerMark& performer);

    virtual int getEntityID()
    {
        return performer->performer.performerID;
    };

    virtual void setMarkedAll(bool mark)
    {
        performer->markAll = mark;
        performer->markC3ds = mark;
        performer->markVideos = mark;
        performer->markAmcs = mark;
    };

    virtual bool isMarkedAll() const
    {
        return performer->markAll;
    };

    virtual void setMarkedC3ds(bool mark)
    {
        performer->markC3ds = mark;
    };

    virtual bool isMarkedC3ds() const
    {
        return performer->markC3ds;
    };

    virtual void setMarkedVideos(bool mark)
    {
        performer->markVideos = mark;
    };

    virtual bool isMarkedVideos() const
    {
        return performer->markVideos;
    };

    virtual void setMarkedAmcs(bool mark)
    {
        performer->markAmcs = mark;
    };

    virtual bool isMarkedAmcs() const
    {
        return performer->markAmcs;
    };
};

class SessionTreeItem : public IEntityTreeItem
{
    SessionRelationPtr session;
public:
    SessionTreeItem(SessionRelationPtr session);

    virtual ~SessionTreeItem() {};

    //void setSession(const SessionMark& session);

    virtual int getEntityID()
    {
        return session->session.sessionID;
    };

    virtual void setMarkedAll(bool mark)
    {
        session->markAll = mark;
        session->markC3ds = mark;
        session->markVideos = mark;
        session->markAmcs = mark;
    };

    virtual bool isMarkedAll() const
    {
        return session->markAll;
    };

    virtual void setMarkedC3ds(bool mark)
    {
        session->markC3ds = mark;
    };

    virtual bool isMarkedC3ds() const
    {
        return session->markC3ds;
    };

    virtual void setMarkedVideos(bool mark)
    {
        session->markVideos = mark;
    };

    virtual bool isMarkedVideos() const
    {
        return session->markVideos;
    };

    virtual void setMarkedAmcs(bool mark)
    {
        session->markAmcs = mark;
    };

    virtual bool isMarkedAmcs() const
    {
        return session->markAmcs;
    };
};

class TrialTreeItem : public IEntityTreeItem
{
protected:
    TrialRelationPtr trial;
public:
    TrialTreeItem(TrialRelationPtr trial);

    virtual ~TrialTreeItem() {};

    //void setTrial(const TrialMark& trial);//, std::vector<core::IDataManager::LocalTrial>& localTrials);

    virtual int getEntityID()
    {
        return trial->trial.trialID;
    };

    virtual void setMarkedAll(bool mark)
    {
        trial->markAll = mark;
        trial->markC3ds = mark;
        trial->markVideos = mark;
        trial->markAmcs = mark;
    };

    virtual bool isMarkedAll() const
    {
        return trial->markAll;
    };

    virtual void setMarkedC3ds(bool mark)
    {
        trial->markC3ds = mark;
    };

    virtual bool isMarkedC3ds() const
    {
        return trial->markC3ds;
    };

    virtual void setMarkedVideos(bool mark)
    {
        trial->markVideos = mark;
    };

    virtual bool isMarkedVideos() const
    {
        return trial->markVideos;
    };

    virtual void setMarkedAmcs(bool mark)
    {
        trial->markAmcs = mark;
    };

    virtual bool isMarkedAmcs() const
    {
        return trial->markAmcs;
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
        if(!isOnline) {
            actionDownload->setVisible(false);
            actionUpdate->setVisible(false);
        } else {
            actionDownload->setVisible(true);
            actionUpdate->setVisible(true);
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
    //dodawanie galezi do workspace
    void addToWorkspace();
    //usuwanie galezi z workspace
    void removeFromWorkspace();
    //
    void treeItemClicked(QTreeWidgetItem* item, int column);
    //
    void workspaceItemClicked(QTreeWidgetItem* item, int column);
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
    Buduj widok performera
    */
    void buildPerformerView(QTreeWidget* tree, bool hideUnmarked);
    /**
    Buduj widok sesji
    */
    void buildSessionView(QTreeWidget* tree, bool hideUnmarked);
    /**
    Buduj widok próby pomiarowej
    */
    void buildTrialView(QTreeWidget* tree, bool hideUnmarked);
    /**
    Tworzy widok dla triala
    */
    TrialTreeItem* createTrialItem(TrialRelationPtr trial);
    //
    void markRecursive(bool mark, IEntityTreeItem* item);
    /**
    Dane z p³ytkiej kopii DB powi¹zane relacjami.
    */
    PerformersRelations performersWithRelations;
    /**
    Lokalna kopia danych zwi¹zana z problemem Qt i w¹tkami "z zewn¹trz".
    */
    MetaData::MetaData metaData;
    /**
    Kopia prób pomiarowych znajduj¹cych siê lokalnie.
    */
    std::vector<core::IDataManager::LocalTrial> localTrials;
    /**
    Menu kontekstowe dla triala lokalnego.
    */
    QMenu* menuTl;
    /**
    Menu kontekstowe dla triala serwerowego.
    */
    QMenu* menuTs;
    /**
    Menu kontekstowe dla sesji.
    */
    QMenu* menuS;
    /**
    Menu kontekstowe dla performera.
    */
    QMenu* menuP;
};
#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONWIDGETEX_H__
