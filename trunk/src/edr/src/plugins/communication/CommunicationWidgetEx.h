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
@brief Klasa CommunicationWidgetEx pe�ni funkcj� widoku dla serwisu Communication. Korzysta z biblioteki Qt.
Jest to druga wersja widoku, bazuj�ca na widgecie QTreeWidget. Widok dostarcza t� sam� funkcjonalno�� co poprzedni, czyli
listowanie triali, wyszczeg�nienie lokalnych i serwerowych pr�b pomiarowych, �adowanie lokalnych pr�b pomiarowych, pobieranie
serwerowych triali. Dodatkow� funkcjonalno�ci� s� widoki wprowadzone wraz z nowym widokiem.
*/
class CommunicationWidgetEx : public QWidget, public Ui::CommunicationWidget, public utils::Observer<communication::CommunicationManager>
{
    Q_OBJECT

struct EntityRelations
{
    EntityRelations(bool markMocap, bool markVideos, bool markEmg, bool markGrf)
        : markMocap(markMocap), markVideos(markVideos), markEmg(markEmg), markGrf(markGrf)
    {};
    bool markMocap;
    bool markVideos;
    bool markEmg;
    bool markGrf;
};

struct TrialRelation : public EntityRelations
{
    TrialRelation()
        : EntityRelations(false, false, false, false)
    {};
    communication::ShallowCopy::Trial trial;
};

typedef core::shared_ptr<TrialRelation> TrialRelationPtr;
typedef std::vector<TrialRelationPtr> TrialsRelations;

struct SessionRelation : public EntityRelations
{
    SessionRelation()
        : EntityRelations(false, false, false, false)
    {};
    communication::ShallowCopy::Session session;
    TrialsRelations trials;
};

typedef core::shared_ptr<SessionRelation> SessionRelationPtr;
typedef std::vector<SessionRelationPtr> SessionsRelations;

struct PerformerRelation : public EntityRelations
{
    PerformerRelation()
        : EntityRelations(false, false, false, false)
    {};
    communication::ShallowCopy::Performer performer;
    SessionsRelations sessions;
};

typedef core::shared_ptr<PerformerRelation> PerformerRelationPtr;
typedef std::vector<PerformerRelationPtr> PerformersRelations;

/**
@author Marek Daniluk
@brief Klasa IEntityTreeItem dziedziczy po QTreeWidgetItem i rozszerza mo�liwo�ci element�w drzewa o menu
kontekstowe i dynamiczn� zmian� liczby i nazw kolumn nag��wka QTreeWidget. Poniewa� reprezentuje dane
powi�zane z p�ytk� kopi� baz� danych opieraj�c� si� na wielu typach, klasa jest abstrakcyjna.
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

    virtual void setMarkedMocap(bool mark) = 0;

    virtual bool isMarkedMocap() const = 0;

    virtual void setMarkedVideos(bool mark) = 0;

    virtual bool isMarkedVideos() const = 0;

    virtual void setMarkedEmg(bool mark) = 0;

    virtual bool isMarkedEmg() const = 0;

    virtual void setMarkedGrf(bool mark) = 0;

    virtual bool isMarkedGrf() const = 0;

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
        performer->markMocap = mark;
        performer->markVideos = mark;
        performer->markEmg = mark;
        performer->markGrf = mark;
    };

    virtual bool isMarkedAll() const
    {
        if(performer->markMocap && performer->markVideos && performer->markEmg && performer->markGrf)
            return true;
        return false;
    };

    virtual void setMarkedMocap(bool mark)
    {
        performer->markMocap = mark;
    };

    virtual bool isMarkedMocap() const
    {
        return performer->markMocap;
    };

    virtual void setMarkedVideos(bool mark)
    {
        performer->markVideos = mark;
    };

    virtual bool isMarkedVideos() const
    {
        return performer->markVideos;
    };

    virtual void setMarkedEmg(bool mark)
    {
        performer->markEmg = mark;
    };

    virtual bool isMarkedEmg() const
    {
        return performer->markEmg;
    };

    virtual void setMarkedGrf(bool mark)
    {
        performer->markGrf = mark;
    };

    virtual bool isMarkedGrf() const
    {
        return performer->markGrf;
    };
};

class SessionTreeItem : public IEntityTreeItem
{
    SessionRelationPtr session;
public:
    SessionTreeItem(SessionRelationPtr session);

    virtual ~SessionTreeItem() {};

    virtual int getEntityID()
    {
        return session->session.sessionID;
    };

    virtual void setMarkedAll(bool mark)
    {
        session->markMocap = mark;
        session->markVideos = mark;
        session->markEmg = mark;
        session->markGrf = mark;
    };

    virtual bool isMarkedAll() const
    {
        if(session->markMocap && session->markVideos && session->markEmg && session->markGrf)
            return true;
        return false;
    };

    virtual void setMarkedMocap(bool mark)
    {
        session->markMocap = mark;
    };

    virtual bool isMarkedMocap() const
    {
        return session->markMocap;
    };

    virtual void setMarkedVideos(bool mark)
    {
        session->markVideos = mark;
    };

    virtual bool isMarkedVideos() const
    {
        return session->markVideos;
    };

    virtual void setMarkedEmg(bool mark)
    {
        session->markEmg = mark;
    };

    virtual bool isMarkedEmg() const
    {
        return session->markEmg;
    };

    virtual void setMarkedGrf(bool mark)
    {
        session->markGrf = mark;
    };

    virtual bool isMarkedGrf() const
    {
        return session->markGrf;
    };
};

class TrialTreeItem : public IEntityTreeItem
{
protected:
    TrialRelationPtr trial;
public:
    TrialTreeItem(TrialRelationPtr trial);

    virtual ~TrialTreeItem() {};

    virtual int getEntityID()
    {
        return trial->trial.trialID;
    };

    TrialRelationPtr getTrial()
    {
        return trial;
    };

    virtual void setMarkedAll(bool mark)
    {
        trial->markMocap = mark;
        trial->markVideos = mark;
        trial->markEmg = mark;
        trial->markGrf = mark;
    };

    virtual bool isMarkedAll() const
    {
        if(trial->markMocap && trial->markVideos && trial->markEmg && trial->markGrf)
            return true;
        return false;
    };

    virtual void setMarkedMocap(bool mark)
    {
        trial->markMocap = mark;
    };

    virtual bool isMarkedMocap() const
    {
        return trial->markMocap;
    };

    virtual void setMarkedVideos(bool mark)
    {
        trial->markVideos = mark;
    };

    virtual bool isMarkedVideos() const
    {
        return trial->markVideos;
    };

    virtual void setMarkedEmg(bool mark)
    {
        trial->markEmg = mark;
    };

    virtual bool isMarkedEmg() const
    {
        return trial->markEmg;
    };

    virtual void setMarkedGrf(bool mark)
    {
        trial->markGrf = mark;
    };

    virtual bool isMarkedGrf() const
    {
        return trial->markGrf;
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
    virtual ~CommunicationWidgetEx() {};
    /**
    Metoda aktualizuj�ca dane widoku.
    @param subject dane pozwalaj�ce zaktualizowa� widok
    */
    virtual void update(const communication::CommunicationManager* subject);
    /**
    Ustala informacj� na etykiecie.
    @param info informacja do wy�wietlenia na etykiecie
    */
    void setInfoLabel(const std::string& info);
    /**
    Ustala warto�� procentow� w pasku pobierania.
    @param value warto�� do przedstawienia przez pasek pobierania
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
    W�a�ciwa metoda aktualizuj�ca widok. powinna wykonywa� si� w g��wnym w�tku.
    */
    void refreshUI();
public slots:
    //widok performera
    void performerViewPressed(bool tog);
    //widok sesji
    void sessionViewPressed(bool tog);
    //widok proby pomiarowej
    void trialViewPressed(bool tog);
    //widok tylko lokalnych zasob�w
    void localViewPressed(bool tog);
    //update triali
    void updatePressed();
    //ladowanie proby
    void loadPressed();
    //pobieranie proby
    void downloadPressed();
    //przerwanie pobierania proby
    void abortPressed();
    //menu og�lne dla widget�w
    void contextMenu(QPoint);
    //menu dla triala
    void trialContextMenu(QPoint);
    //
    void treeItemClicked(QTreeWidgetItem* item, int column);

    void useVideos();
    void useMocap();
    void useGrf();
    void useEmg();
    //
    void useTrialVideos();
    void useTrialMocap();
    void useTrialGrf();
    void useTrialEmg();
private:
    /**
    Wska�nik na kontroler serwisu
    */
    CommunicationService* communicationService;
    ///**
    //Za�aduj trial do aplikacji.
    //@param localTrial lista plik�w jednej pr�by pomiarowej do za�adowania.
    //*/
    //void loadTrial(const core::IDataManager::LocalTrial& localTrial);
    /**
    Za�aduj pliki do aplikacji.
    @param files lista plik�w do za�adowania po filtracji.
    */
    void loadFiles(const std::vector<core::Filesystem::Path>& files);
    /**
    Pobranie pr�by pomiarowej z serwera.
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
    Buduj widok pr�by pomiarowej
    */
    void buildTrialView(QTreeWidget* tree);
    /**
    Buduj widok lokalnych zasob�w
    */
    void buildLocalView(QTreeWidget* tree);
    /**
    Tworzy widok triala
    */
    TrialTreeItem* createTrialItem(TrialRelationPtr trial);
    /**
    Od�wie� headery drzewa danych. Dzia�a tylko gdy drzewo posiada same elementy IEntityTreeItem
    */
    void refreshHeader(QTreeWidget* tree);
    /**
    Dane z p�ytkiej kopii DB powi�zane relacjami.
    */
    PerformersRelations performersWithRelations;
    /**
    Lokalna kopia danych zwi�zana z problemem Qt i w�tkami "z zewn�trz".
    */
    communication::MetaData::MetaData metaData;
    /**
    Kopia pr�b pomiarowych znajduj�cych si� lokalnie.
    */
    std::vector<core::IDataManager::LocalTrial> localTrials;
    /**
    menu kontekstowe dla poszczeg�lnych element�w ui.
    */
    QMenu* menuTl;
    QMenu* menuTs;
    QMenu* menu;
    QMenu* view;
    /**
    Obecnie widoczny view CS.
    */
    QTreeWidget* currentView;
    /**
    */
    std::vector<core::Filesystem::Path> listTrialFiles(bool mocap, bool emg, bool grf, bool videos);
};
#endif //HEADER_GUARD_COMMUNICATION_COMMUNICATIONWIDGETEX_H__
