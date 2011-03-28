#include "CommunicationPCH.h"
#include "CommunicationWidgetEx.h"
#include <core/StringTools.h>
#include <core/StringTools.h>
#include <QtGui/QPainter>

//http://hmdb.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl
//http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl

using namespace communication;

PerformerTreeItem::PerformerTreeItem(PerformerRelationPtr performer)
    : performer(performer)
{
    menu = nullptr;
    headers << "" << "Sessions" << "Description" << "Notes";
    setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/subject.png")));

    QString name = QString::fromUtf8(performer->performer.firstName.c_str());
    QString lastName = QString::fromUtf8(performer->performer.lastName.c_str());
    setText(0, QString(name + " " + lastName));
}

SessionTreeItem::SessionTreeItem(SessionRelationPtr session)
    : session(session)
{
    menu = nullptr;
    headers << "" << "Date" << "Trials" << "Description" << "Notes";
    setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/session.png")));

    setText(0, QString::fromUtf8(session->session.sessionName.c_str()));
    setText(1, QString::fromUtf8(session->session.sessionDate.c_str()).left(10));
    setText(3, QString::fromUtf8(session->session.sessionDescription.c_str()));
    setText(4, QString::fromUtf8(session->session.tags.c_str()));
}

TrialTreeItem::TrialTreeItem(TrialRelationPtr trial)
    : trial(trial)
{
    menu = nullptr;
    headers << "" << "Files" << "Description" << "Notes";
    setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/trial.png")));
    setText(0, QString::fromUtf8(trial->trial.trialName.c_str()).right(11));
    int iconSize = 24;
    QPixmap files(iconSize, iconSize);
    QPainter painter(&files);
    bool video = false, c3d = false, amc = false, asf = false;
    BOOST_FOREACH(ShallowCopy::File file, trial->trial.files)
    {
        if(!boost::filesystem::path(file.fileName).extension().compare(".avi")) {
            video = true;
        } else if(!boost::filesystem::path(file.fileName).extension().compare(".c3d")) {
            c3d = true;
        } else if(!boost::filesystem::path(file.fileName).extension().compare(".amc")) {
            amc = true;
        } else if(!boost::filesystem::path(file.fileName).extension().compare(".asf")) {
            asf = true;
        }
    }
    if(video) {
        painter.fillRect(0, 0, iconSize/3, iconSize, QColor(255, 255, 0));//drawImage(QPoint(0, 0), QImage("data/resources/icons/subject.png"));
    }
    if(c3d) {
        painter.fillRect(iconSize/3, 0, iconSize/3, iconSize, QColor(0, 255, 0));
    }
    if(amc && asf) {
        painter.fillRect(iconSize/3*2, 0, iconSize/3, iconSize, QColor(255, 0, 0));
    }
    painter.end(); 
    setIcon(1, QIcon(files/*QString::fromUtf8("data/resources/icons/subject.png")*/));
    setText(1, QString::number(trial->trial.files.size()));
    setText(2, QString::fromUtf8(trial->trial.trialDescription.c_str()));
}

const QString TrialTreeItem::getName() const
{
    return core::toQString(trial->trial.trialName);
}

CommunicationWidgetEx::CommunicationWidgetEx(CommunicationService* service)
    : communicationService(service)
{
    // inicjalizacja UI
    setupUi(this);

    // dodanie grupy akcji (designer na to nie pozwala :()
    // TIP: w przyk³adach Qt nie ma jawnego zwalniania akcji
    // wzialem z VideoWidget
    QActionGroup* formatGroup = new QActionGroup(this);
    formatGroup->addAction(actionPerformer_View);
    formatGroup->addAction(actionSession_View);
    formatGroup->addAction(actionTrial_View);
    
    actionAbort_download->setVisible(false);
    
    menuTl = new QMenu;
    menuTl->setTitle("Local Trial");
    menuTl->addAction(actionLoad_trial);
    menuTl->addSeparator();
    menuTl->addAction(actionAdd);
    menuTl->addAction(actionRemove);
    menuTl->addAction(actionAdd_c3d);
    menuTl->addAction(actionAdd_videos);
    menuTl->addAction(actionAdd_amc);
    menuTl->addAction(actionRemove_c3d);
    menuTl->addAction(actionRemove_videos);
    menuTl->addAction(actionRemove_amc);

    menuTs = new QMenu;
    menuTs->setTitle("Trial");
    menuTs->addAction(actionDownload);
    menuTs->addSeparator();
    menuTs->addAction(actionAdd);
    menuTs->addAction(actionRemove);
    menuTs->addAction(actionAdd_c3d);
    menuTs->addAction(actionAdd_videos);
    menuTs->addAction(actionAdd_amc);
    menuTs->addAction(actionRemove_c3d);
    menuTs->addAction(actionRemove_videos);
    menuTs->addAction(actionRemove_amc);

    menuS = new QMenu;
    menuS->setTitle("Session");
    menuS->addAction(actionAdd);
    menuS->addAction(actionRemove);
    menuS->addAction(actionAdd_c3d);
    menuS->addAction(actionAdd_videos);
    menuS->addAction(actionAdd_amc);
    menuS->addAction(actionRemove_c3d);
    menuS->addAction(actionRemove_videos);
    menuS->addAction(actionRemove_amc);

    menuP = new QMenu;
    menuP->setTitle("Subject");
    menuP->addAction(actionAdd);
    menuP->addAction(actionRemove);
    menuP->addAction(actionAdd_c3d);
    menuP->addAction(actionAdd_videos);
    menuP->addAction(actionAdd_amc);
    menuP->addAction(actionRemove_c3d);
    menuP->addAction(actionRemove_videos);
    menuP->addAction(actionRemove_amc);

    connect(dataWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(workspace, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(workspaceItemClicked(QTreeWidgetItem*, int)));
}

void CommunicationWidgetEx::treeItemClicked(QTreeWidgetItem* item, int column)
{
    if(item) {
        IEntityTreeItem* current = reinterpret_cast<IEntityTreeItem*>(item);
        int columns = current->columnCount();
        dataWidget->setColumnCount(columns);
        dataWidget->setHeaderLabels(current->getHeaders());
        for(int i = 0; i < columns; i++) {
            dataWidget->resizeColumnToContents(i);
        }
        if(current->isMarkedAll()) {
            actionRemove->setDisabled(false);
            actionRemove->setVisible(true);
            actionAdd->setDisabled(true);
            actionAdd->setVisible(false);
        } else {
            actionRemove->setDisabled(true);
            actionRemove->setVisible(false);
            actionAdd->setDisabled(false);
            actionAdd->setVisible(true);
        }
        if(current->isMarkedAmcs()) {
            actionRemove_amc->setDisabled(false);
            actionRemove_amc->setVisible(true);
            actionAdd_amc->setDisabled(true);
            actionAdd_amc->setVisible(false);
        } else {
            actionRemove_amc->setDisabled(true);
            actionRemove_amc->setVisible(false);
            actionAdd_amc->setDisabled(false);
            actionAdd_amc->setVisible(true);
        }
        if(current->isMarkedC3ds()) {
            actionRemove_c3d->setDisabled(false);
            actionRemove_c3d->setVisible(true);
            actionAdd_c3d->setDisabled(true);
            actionAdd_c3d->setVisible(false);
        } else {
            actionRemove_c3d->setDisabled(true);
            actionRemove_c3d->setVisible(false);
            actionAdd_c3d->setDisabled(false);
            actionAdd_c3d->setVisible(true);
        }
        if(current->isMarkedVideos()) {
            actionRemove_videos->setDisabled(false);
            actionRemove_videos->setVisible(true);
            actionAdd_videos->setDisabled(true);
            actionAdd_videos->setVisible(false);
        } else {
            actionRemove_videos->setDisabled(true);
            actionRemove_videos->setVisible(false);
            actionAdd_videos->setDisabled(false);
            actionAdd_videos->setVisible(true);
        }
    }
}

void CommunicationWidgetEx::workspaceItemClicked(QTreeWidgetItem* item, int column)
{
    if(item) {
        IEntityTreeItem* current = reinterpret_cast<IEntityTreeItem*>(item);
        int columns = current->columnCount();
        workspace->setColumnCount(columns);
        workspace->setHeaderLabels(current->getHeaders());
        for(int i = 0; i < columns; i++) {
            workspace->resizeColumnToContents(i);
        }
    }
}

void CommunicationWidgetEx::trialContextMenu(QPoint p)
{
    QMenu* view = new QMenu;
    view->setTitle("View");
    view->addSeparator();
    view->addAction(actionPerformer_View);
    view->addAction(actionSession_View);
    view->addAction(actionTrial_View);

    QMenu* menu = new QMenu;
    menu->addSeparator();
    menu->addAction(actionUpdate);
    menu->addAction(actionAbort_download);

    menu->addSeparator();
    menu->addMenu(view);
    menu->addSeparator();

    if(dataWidget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(dataWidget->currentItem());
        if(item->getMenu())
            menu->addMenu(item->getMenu());
    }

    menu->exec(mapToGlobal(p));
}

void CommunicationWidgetEx::contextMenu(QPoint p)
{
    QMenu* view = new QMenu;
    view->setTitle("View");
    view->addSeparator();
    view->addAction(actionPerformer_View);
    view->addAction(actionSession_View);
    view->addAction(actionTrial_View);

    QMenu* menu = new QMenu;
    menu->addSeparator();
    menu->addAction(actionUpdate);
    menu->addAction(actionAbort_download);
    menu->addSeparator();
    menu->addMenu(view);

    menu->exec(mapToGlobal(p));
}

void CommunicationWidgetEx::performerViewPressed(bool tog)
{
    buildPerformerView(dataWidget, false);
    buildPerformerView(workspace, true);
}

void CommunicationWidgetEx::sessionViewPressed(bool tog)
{
    buildSessionView(dataWidget, false);
    buildSessionView(workspace, true);
}

void CommunicationWidgetEx::trialViewPressed(bool tog)
{
    buildTrialView(dataWidget, false);
    buildTrialView(workspace, true);
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
    downloadTrial(reinterpret_cast<TrialTreeItem*>(dataWidget->currentItem())->getEntityID());
}

void CommunicationWidgetEx::loadPressed()
{
    actionDownload->setDisabled(true);
    actionUpdate->setDisabled(true);
    actionLoad_trial->setDisabled(false);
    actionAbort_download->setDisabled(true);
    actionAbort_download->setVisible(false);
    BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
    {
        boost::cmatch matches;
        boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
        //sprawdzamy, czy zgadza sie nazwa folderu
        if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e)) {
            if(reinterpret_cast<TrialTreeItem*>(dataWidget->currentItem())->getName() == core::toQString(matches[2])) {
                loadTrial(lTrial);
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

void CommunicationWidgetEx::loadTrial(const core::IDataManager::LocalTrial& localTrial)
{
    communicationService->loadTrial(localTrial);
}

void CommunicationWidgetEx::downloadTrial(int trialID)
{
    communicationService->downloadTrial(trialID);
}

void CommunicationWidgetEx::update(const CommunicationManager* subject)
{
    //budujemy relacje miedzy encjami bazodanowymi
    ShallowCopy::ShallowCopy shallowCopy = subject->getShalloCopy();
    performersWithRelations.clear();

    BOOST_FOREACH(ShallowCopy::Performer performer, shallowCopy.performers)
    {
        PerformerRelationPtr p = PerformerRelationPtr(new PerformerRelation);
        p->performer = performer;
        performersWithRelations.push_back(p);
        //sesje polaczone z performerami
        BOOST_FOREACH(ShallowCopy::PerformerConf performerC, shallowCopy.performerConfs)
        {
            BOOST_FOREACH(ShallowCopy::Session session, shallowCopy.sessions)
            {
                if(performerC.performerID == p->performer.performerID && performerC.sessionID == session.sessionID) {
                    SessionRelationPtr s = SessionRelationPtr(new SessionRelation);
                    s->session = session;
                    p->sessions.push_back(s);
                    //kojarzenie sesji z trialami
                    BOOST_FOREACH(ShallowCopy::Trial trial, shallowCopy.trials)
                    {
                        if(trial.sessionID == s->session.sessionID) {
                            TrialRelationPtr t = TrialRelationPtr(new TrialRelation);
                            t->trial = trial;
                            s->trials.push_back(t);
                        }
                    }
                }
            }
        }
    }

    metaData = subject->getMetadata();
    localTrials = subject->getLocalTrials();
    infoLabel->setText("");

    refreshUI();
}

void CommunicationWidgetEx::refreshUI()
{
    actionDownload->setDisabled(false);
    actionUpdate->setDisabled(false);
    actionLoad_trial->setDisabled(false);
    actionAbort_download->setDisabled(true);
    actionAbort_download->setVisible(false);

    dataWidget->clear();
    dataWidget->setSortingEnabled(true);
    dataWidget->setDisabled(false);
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
        actionPerformer_View->setChecked(false);
        actionPerformer_View->setChecked(true);
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

void CommunicationWidgetEx::addToWorkspace()
{
    if(dataWidget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(dataWidget->currentItem());
        markRecursive(true, item);//item->setMarkedAll(true);
    }
    if(actionPerformer_View->isChecked()) {   
        actionPerformer_View->setChecked(false);
        actionPerformer_View->setChecked(true);
    }
    if(actionSession_View->isChecked()) {   
        actionSession_View->setChecked(false);
        actionSession_View->setChecked(true);
    }
    if(actionTrial_View->isChecked()) {   
        actionTrial_View->setChecked(false);
        actionTrial_View->setChecked(true);
    }
}

void CommunicationWidgetEx::removeFromWorkspace()
{
    if(dataWidget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(dataWidget->currentItem());
        markRecursive(false, item);//->setMarkedAll(false);
    }
    if(actionPerformer_View->isChecked()) {   
        actionPerformer_View->setChecked(false);
        actionPerformer_View->setChecked(true);
    }
    if(actionSession_View->isChecked()) {   
        actionSession_View->setChecked(false);
        actionSession_View->setChecked(true);
    }
    if(actionTrial_View->isChecked()) {   
        actionTrial_View->setChecked(false);
        actionTrial_View->setChecked(true);
    }
}

void CommunicationWidgetEx::markRecursive(bool mark, IEntityTreeItem* item)
{
    for(int i = 0; i < item->childCount(); i++) {
        markRecursive(mark, reinterpret_cast<IEntityTreeItem*>(item->child(i)));
    }
    item->setMarkedAll(mark);
}

void CommunicationWidgetEx::buildPerformerView(QTreeWidget* tree, bool hideUnmarked)
{
    tree->clear();
    //listowanie performerow
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        PerformerTreeItem* item = new PerformerTreeItem(performer);
        
        //item->setPerformer(performer);
        item->setMenu(menuP);
        
        tree->addTopLevelItem(item);
        //element ma byc niewidoczny?
        if(hideUnmarked && !performer->markAll) {
            item->setHidden(true);
        }
        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            SessionTreeItem* itemS = new SessionTreeItem(session);
            itemS->setMenu(menuS);
            item->addChild(itemS);
            //element ma byc niewidoczny?
            if(hideUnmarked && !session->markAll) {
                itemS->setHidden(true);
            }
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                TrialTreeItem* itemT = createTrialItem(trial);
                itemS->addChild(itemT);
                //element ma byc niewidoczny?
                if(hideUnmarked && !trial->markAll) {
                    itemT->setHidden(true);
                }
            }
            itemS->setText(2, QString::number(item->childCount()));
        }
        item->setText(1, QString::number(item->childCount()));
    }
}

void CommunicationWidgetEx::buildSessionView(QTreeWidget* tree, bool hideUnmarked)
{
    tree->clear();
    //listowanie sesji
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            SessionTreeItem* item = new SessionTreeItem(session);
            item->setMenu(menuS);
            tree->addTopLevelItem(item);
            //element ma byc niewidoczny?
            if(hideUnmarked && !session->markAll) {
                item->setHidden(true);
            }
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                TrialTreeItem* itemT = createTrialItem(trial);
                item->addChild(itemT);
                //element ma byc niewidoczny?
                if(hideUnmarked && !trial->markAll) {
                    itemT->setHidden(true);
                }
            }
            item->setText(2, QString::number(item->childCount()));
        }
    }
}

void CommunicationWidgetEx::buildTrialView(QTreeWidget* tree, bool hideUnmarked)
{
    tree->clear();
    //listowanie triali
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                TrialTreeItem* item = createTrialItem(trial);
                tree->addTopLevelItem(item);
                //element ma byc niewidoczny?
                if(hideUnmarked && !trial->markAll) {
                    item->setHidden(true);
                }
            }
        }
    }
}

TrialTreeItem* CommunicationWidgetEx::createTrialItem(TrialRelationPtr trial)
{
    TrialTreeItem* item = new TrialTreeItem(trial);
    //lokalny czy serwerowy?
    BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
    {
        boost::cmatch matches;
        boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
        //sprawdzamy, czy zgadza sie nazwa folderu
        if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trial.trialName.compare(matches[2])) {
            item->setTextColor(0, QColor(0, 0, 255));
            item->setMenu(menuTl);
            break;
        } else {
            item->setMenu(menuTs);
        }
    }
    return item;
}
