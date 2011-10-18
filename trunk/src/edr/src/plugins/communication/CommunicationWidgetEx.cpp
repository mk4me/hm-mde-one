#include "CommunicationPCH.h"
#include "CommunicationWidgetEx.h"
#include <plugins/communication/CommunicationService.h>
#include <core/StringTools.h>
#include <core/StringTools.h>
#include <QtGui/QPainter>
#include <boost/regex.hpp>

#include <regex>

//http://hmdb.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl
//http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl

using namespace communication;

CommunicationWidgetEx::PerformerTreeItem::PerformerTreeItem(const communication::ShallowCopy::Performer * performer)
    : performer(performer)
{
    menu = nullptr;
    headers << "" << "Element";
    setToolTip(0, "Performer");
    setIcon(0, QIcon(core::getResourceString("icons/subject.png")));

    setText(0, QString::number(performer->performerID));
    setText(1, QString::number(performer->performerConfs.size()).append(" session(s)"));
}

CommunicationWidgetEx::SessionTreeItem::SessionTreeItem(const communication::ShallowCopy::Session * session)
    : session(session)
{
    menu = nullptr;
    headers << "" << "Element" << "Date" << "Performer";
    setToolTip(0, "Session");
    setIcon(0, QIcon(core::getResourceString("icons/session.png")));

    setText(0, QString::fromUtf8(session->sessionName.c_str()));
    setText(1, QString::number(session->trials.size()).append(" trial(s)"));
    setText(2, QString::fromUtf8(session->sessionDate.c_str()).left(10));
}

CommunicationWidgetEx::TrialTreeItem::TrialTreeItem(const communication::ShallowCopy::Trial * trial)
    : trial(trial)
{
    menu = nullptr;
    headers << "" << "Element" << "Date" << "Performer" << "Session";
    setToolTip(0, "Trial");
    setIcon(0, QIcon(core::getResourceString("icons/trial.png")));
    setText(0, QString::fromUtf8(trial->trialName.c_str()).right(11));
    setText(1, QString::number(trial->files.size()).append(" file(s)"));
}

const QString CommunicationWidgetEx::TrialTreeItem::getName() const
{
    return core::toQString(trial->trialName);
}

CommunicationWidgetEx::CommunicationWidgetEx(CommunicationService* service)
    : communicationService(service)
{
    // inicjalizacja UI
    setupUi(this);

    // dodanie grupy akcji (designer na to nie pozwala :()
    // TIP: w przyk³adach Qt nie ma jawnego zwalniania akcji
    // wzialem z VideoWidget
    formatGroup = new QActionGroup(this);
    formatGroup->addAction(actionPerformer_View);
    formatGroup->addAction(actionSession_View);
    formatGroup->addAction(actionTrial_View);
    formatGroup->addAction(actionLocal_View);

    actionAbort_download->setVisible(false);

    currentView = performerView;

    //menu = new QMenu;
    //menu->addSeparator();
    
    view = new QMenu;
    view->setTitle("View");
    view->addSeparator();
    view->addAction(actionPerformer_View);
    view->addAction(actionSession_View);
    view->addAction(actionTrial_View);
    view->addAction(actionLocal_View);

    menuTl = new QMenu;
    menuTl->addSeparator();
    //menuTl->addActions(menu->actions());
    menuTl->addSeparator();
    //menuTl->addAction(actionLoad_trial);

    menuTs = new QMenu;
    menuTs->addSeparator();
    //menuTs->addActions(menu->actions());
    menuTs->addSeparator();
    menuTs->addAction(actionDownload);

    connect(performerView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(sessionView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(trialView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(localView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
}

CommunicationWidgetEx::~CommunicationWidgetEx()
{
    delete formatGroup;
    delete view;
    delete menuTl;
    delete menuTs;
}

void CommunicationWidgetEx::treeItemClicked(QTreeWidgetItem* item, int column)
{
    if(item) {
        //IEntityTreeItem* current = reinterpret_cast<IEntityTreeItem*>(item);
        IEntityTreeItem* current = dynamic_cast<IEntityTreeItem*>(item);

        int columns = current->columnCount();
        currentView->setColumnCount(columns);
        currentView->setHeaderLabels(current->getHeaders());
        for(int i = 0; i < columns; i++) {
            currentView->resizeColumnToContents(i);
        }
    }
}

void CommunicationWidgetEx::trialContextMenu(QPoint p)
{
    QMenu* menu = new QMenu;
    menu->addSeparator();
    menu->addAction(actionUpdate);
    menu->addAction(actionAbort_download);

    menu->addSeparator();
    menu->addMenu(view);
    menu->addSeparator();

    //currentItem() przy drugim kliknieciu automatycznie wybiera pierwszy element drzewa
    //sprawdzamy czy cokolwiek jest wybrane sprawdzajac liste zaznaczonych itemow
    if(currentView && currentView->currentItem() && currentView->selectedItems().size() > 0) {
        //IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentView->currentItem());
        IEntityTreeItem* item = dynamic_cast<IEntityTreeItem*>(currentView->currentItem());
        if(item->getMenu())
            menu->addActions(item->getMenu()->actions());
    }
    menu->exec(mapToGlobal(p));
    delete menu;
}

void CommunicationWidgetEx::contextMenu(QPoint p)
{
    QMenu* menu = new QMenu;
    menu->addSeparator();
    menu->addAction(actionUpdate);
    menu->addAction(actionAbort_download);
    menu->addSeparator();
    menu->addMenu(view);

    menu->exec(mapToGlobal(p));
    delete menu;
}

void CommunicationWidgetEx::performerViewPressed(bool tog)
{
    currentView = performerView;
    csStackedWidget->setCurrentWidget(performer_page);
}

void CommunicationWidgetEx::sessionViewPressed(bool tog)
{
    currentView = sessionView;
    csStackedWidget->setCurrentWidget(session_page);
}

void CommunicationWidgetEx::trialViewPressed(bool tog)
{
    currentView = trialView;
    csStackedWidget->setCurrentWidget(trial_page);
}

void CommunicationWidgetEx::localViewPressed(bool tog)
{
    currentView = localView;
    csStackedWidget->setCurrentWidget(local_page);
}

void CommunicationWidgetEx::updatePressed()
{
    actionDownload->setDisabled(true);
    actionUpdate->setDisabled(true);
    actionLoad_trial->setDisabled(true);
    actionAbort_download->setDisabled(true);
    actionAbort_download->setVisible(false);
    communicationService->copyDbData();
}

void CommunicationWidgetEx::downloadPressed()
{
    actionDownload->setDisabled(true);
    actionUpdate->setDisabled(true);
    actionLoad_trial->setDisabled(false);
    actionAbort_download->setDisabled(false);
    actionAbort_download->setVisible(true);
    downloadTrial(dynamic_cast<TrialTreeItem*>(currentView->currentItem())->getEntityID());
}

void CommunicationWidgetEx::loadPressed()
{
    BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
    {
        boost::cmatch matches;
        boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})(.*)");
        //sprawdzamy, czy zgadza sie nazwa folderu
        if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e)) {
            if(dynamic_cast<TrialTreeItem*>(currentView->currentItem())->getName() == core::toQString(matches[2])) {
                loadFiles(lTrial);
                return;
            }
        }
    }
}

void CommunicationWidgetEx::abortPressed()
{
    actionDownload->setDisabled(false);
    actionUpdate->setDisabled(false);
    actionLoad_trial->setDisabled(false);
    actionAbort_download->setDisabled(true);
    actionAbort_download->setVisible(false);
    communicationService->cancelDownloading();
}

void CommunicationWidgetEx::loadFiles(const std::vector<core::Filesystem::Path>& files)
{
    communicationService->loadFiles(files);
}

void CommunicationWidgetEx::downloadTrial(int trialID)
{
    communicationService->downloadTrial(trialID);
}

void CommunicationWidgetEx::update(const CommunicationManager* subject)
{
    //p³ytka kopia bazy danych
    constShallowCopy = subject->getShalloCopy();

    metaData = subject->getMetadata();
    localTrials = subject->getLocalTrials();
    infoLabel->setText("");
    
    if(constShallowCopy != nullptr){
        buildPerformerView(performerView);
        buildSessionView(sessionView);
        buildTrialView(trialView);
        buildLocalView(localView);
    }

    refreshUI();
}

void CommunicationWidgetEx::refreshUI()
{
    actionDownload->setDisabled(false);
    actionUpdate->setDisabled(false);
    actionLoad_trial->setDisabled(false);
    actionAbort_download->setDisabled(true);
    actionAbort_download->setVisible(false);

    progressBar->reset();
    if(actionPerformer_View->isChecked()) {
        actionPerformer_View->setChecked(false);
        actionPerformer_View->setChecked(true);
    } else if(actionSession_View->isChecked()) {
        actionSession_View->setChecked(false);
        actionSession_View->setChecked(true);
    } else if(actionTrial_View->isChecked()) {
        actionTrial_View->setChecked(false);
        actionTrial_View->setChecked(true);
    } else {
        actionLocal_View->setChecked(false);
        actionLocal_View->setChecked(true);
    }
}

void CommunicationWidgetEx::setInfoLabel(const std::string& info)
{
    infoLabel->setText(core::toQString(info));
}

void CommunicationWidgetEx::setProgress(int value)
{
    if(value > 99) {
        progressBar->reset();
    } else {
        progressBar->setValue(value);
    }
}

void CommunicationWidgetEx::buildPerformerView(QTreeWidget* tree)
{
    tree->clear();
    //listowanie performerow

    for(auto performerIT = constShallowCopy->performers.begin(); performerIT != constShallowCopy->performers.end(); performerIT++)
    {
        PerformerTreeItem* item = new PerformerTreeItem(performerIT->second);
        
        //item->setMenu(menu);
        
        tree->addTopLevelItem(item);

        QString perfID(QString::number(performerIT->second->performerID));
        
        for(auto perfConfIT = performerIT->second->performerConfs.begin(); perfConfIT != performerIT->second->performerConfs.end(); perfConfIT++)
        {
            SessionTreeItem* itemS = new SessionTreeItem(perfConfIT->second->session);
            //itemS->setMenu(menu);
            itemS->setText(3, perfID);
            item->addChild(itemS);

            QString sessionID(core::toQString(perfConfIT->second->session->sessionDate).left(10));
            QString sessionDate(QString::fromUtf8(perfConfIT->second->session->sessionName.c_str()));

            for(auto trialIT = perfConfIT->second->session->trials.begin(); trialIT != perfConfIT->second->session->trials.end(); trialIT++)
            {
                TrialTreeItem* itemT = createTrialItem(trialIT->second);
                itemT->setText(2, sessionID);
                itemT->setText(3, itemS->text(3));
                itemT->setText(4, sessionDate);
                itemS->addChild(itemT);
            }
        }
    }
    refreshHeader(tree);
}

void CommunicationWidgetEx::buildSessionView(QTreeWidget* tree)
{
    tree->clear();
    //listowanie sesji
    for(auto perfConfIT = constShallowCopy->performerConfs.begin(); perfConfIT != constShallowCopy->performerConfs.end(); perfConfIT++)
    {
        SessionTreeItem* item = new SessionTreeItem(perfConfIT->second->session);
        //item->setMenu(menu);
        tree->addTopLevelItem(item);
        item->setText(3, QString::number(perfConfIT->second->performer->performerID));
            
        QString sessionDate(core::toQString(perfConfIT->second->session->sessionDate).left(10));
        QString sessionName(QString::fromUtf8(perfConfIT->second->session->sessionName.c_str()));

        for(auto trialIT = perfConfIT->second->session->trials.begin(); trialIT != perfConfIT->second->session->trials.end(); trialIT++)
        {
            TrialTreeItem* itemT = createTrialItem(trialIT->second);
            itemT->setText(2, sessionDate);
            itemT->setText(3, item->text(3));
            itemT->setText(4, sessionName);
            item->addChild(itemT);
        }
    }
    refreshHeader(tree);
}

void CommunicationWidgetEx::buildTrialView(QTreeWidget* tree)
{
    tree->clear();
    //listowanie triali
    for(auto trialIT = constShallowCopy->trials.begin(); trialIT != constShallowCopy->trials.end(); trialIT++)
    {
        TrialTreeItem* item = createTrialItem(trialIT->second);

        item->setText(2, core::toQString(trialIT->second->session->sessionDate).left(10));
        item->setText(3, QString::number(trialIT->second->session->performerConf->performer->performerID));
        item->setText(4, QString::fromUtf8(trialIT->second->session->sessionName.c_str()));
        tree->addTopLevelItem(item);
    }

    refreshHeader(tree);
}

void CommunicationWidgetEx::buildLocalView(QTreeWidget* tree)
{
    tree->clear();
    //listowanie triali

    for(auto trialIT = constShallowCopy->trials.begin(); trialIT != constShallowCopy->trials.end(); trialIT++)
    {
        TrialTreeItem* item = createTrialItem(trialIT->second);

        item->setText(2, core::toQString(trialIT->second->session->sessionDate).left(10));
        item->setText(3, QString::number(trialIT->second->session->performerConf->performer->performerID));
        item->setText(4, QString::fromUtf8(trialIT->second->session->sessionName.c_str()));
        if(item->textColor(0) != QColor(128, 128, 128)) {
            tree->addTopLevelItem(item);
        }else{
            delete item;
        }
    }

    refreshHeader(tree);
}

CommunicationWidgetEx::TrialTreeItem* CommunicationWidgetEx::createTrialItem(const communication::ShallowCopy::Trial * trial)
{
    TrialTreeItem* item = new TrialTreeItem(trial);
    //lokalny czy serwerowy? jeœli nie ma w ogole lokalnych prob to zaznaczamy od razu jako serwerowy
    if(localTrials.size() != 0) {
        BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
        {
            boost::cmatch matches;
            boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})(.*)");
            //sprawdzamy, czy zgadza sie nazwa folderu
            if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trialName.compare(matches[2])) {
                item->setTextColor(0, QColor(0, 0, 0));
                item->setMenu(menuTl);
                break;
            } else {
                item->setTextColor(0, QColor(128, 128, 128));
                item->setMenu(menuTs);
            }
        }
    } else {
        item->setTextColor(0, QColor(128, 128, 128));
        item->setMenu(menuTs);
    }
    return item;
}

void CommunicationWidgetEx::refreshHeader(QTreeWidget* tree)
{
    //headery
    if(tree->topLevelItem(0)) {
        //IEntityTreeItem* current = reinterpret_cast<IEntityTreeItem*>(tree->topLevelItem(0));
        IEntityTreeItem* current = dynamic_cast<IEntityTreeItem*>(tree->topLevelItem(0));

        int columns = current->columnCount();
        tree->setColumnCount(columns);
        tree->setHeaderLabels(current->getHeaders());
        for(int i = 0; i < columns; i++) {
            tree->resizeColumnToContents(i);
        }
    }
}
