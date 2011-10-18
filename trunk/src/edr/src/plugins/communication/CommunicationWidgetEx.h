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
private:
    /**
    Wska�nik na kontroler serwisu
    */
    CommunicationService* communicationService;

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
    TrialTreeItem* createTrialItem(const communication::ShallowCopy::Trial * trial);
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
