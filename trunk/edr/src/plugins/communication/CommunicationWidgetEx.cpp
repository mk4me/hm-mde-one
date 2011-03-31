#include "CommunicationPCH.h"
#include "CommunicationWidgetEx.h"
#include <core/StringTools.h>
#include <core/StringTools.h>
#include <QtGui/QPainter>

//http://hmdb.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl
//http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl

using namespace communication;

CommunicationWidgetEx::PerformerTreeItem::PerformerTreeItem(PerformerRelationPtr performer)
    : performer(performer)
{
    menu = nullptr;
    headers << "" << "Element";
    setToolTip(0, "Performer");
    setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/subject.png")));

    QString name = QString::fromUtf8(performer->performer.firstName.c_str());
    QString lastName = QString::fromUtf8(performer->performer.lastName.c_str());
    setText(0, QString(name + " " + lastName));
    setText(1, QString::number(performer->sessions.size()).append(" session(s)"));
}

CommunicationWidgetEx::SessionTreeItem::SessionTreeItem(SessionRelationPtr session)
    : session(session)
{
    menu = nullptr;
    headers << "" << "Element" << "Date" << "Performer";
    setToolTip(0, "Session");
    setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/session.png")));

    setText(0, QString::fromUtf8(session->session.sessionName.c_str()));
    setText(1, QString::number(session->trials.size()).append(" trial(s)"));
    setText(2, QString::fromUtf8(session->session.sessionDate.c_str()).left(10));
}

CommunicationWidgetEx::TrialTreeItem::TrialTreeItem(TrialRelationPtr trial)
    : trial(trial)
{
    menu = nullptr;
    headers << "" << "Element" << "Date" << "Performer" << "Session";
    setToolTip(0, "Trial");
    setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/trial.png")));
    setText(0, QString::fromUtf8(trial->trial.trialName.c_str()).right(11));
    //int iconSize = 24;
    //QPixmap files(iconSize, iconSize);
    //QPainter painter(&files);
    QString files;
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
        //painter.fillRect(0, 0, iconSize/3, iconSize, QColor(255, 255, 0));//drawImage(QPoint(0, 0), QImage("data/resources/icons/subject.png"));
        files.append("VID");
    }
    if(c3d) {
        files.append(" EMG GRF");
        //painter.fillRect(iconSize/3, 0, iconSize/3, iconSize, QColor(0, 255, 0));
    }
    if(amc && asf && c3d) {
        files.append(" MOC");
        //painter.fillRect(iconSize/3*2, 0, iconSize/3, iconSize, QColor(255, 0, 0));
    }
    //painter.end();
    //setIcon(1, QIcon(files/*QString::fromUtf8("data/resources/icons/subject.png")*/));
    setText(1, QString::number(trial->trial.files.size()).append(" file(s) [").append(files).append("]"));
}

const QString CommunicationWidgetEx::TrialTreeItem::getName() const
{
    return core::toQString(trial->trial.trialName);
}

CommunicationWidgetEx::WorkspaceTreeItem::WorkspaceTreeItem(TrialRelationPtr trial)
    : TrialTreeItem(trial)
{
    menu = nullptr;
    headers << "" << "Date" << "Performer" << "Session";
    setToolTip(0, "Trial");
    setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/trial.png")));
    setText(0, QString::fromUtf8(trial->trial.trialName.c_str()).right(11));

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
    formatGroup->addAction(actionLocal_View);
    
    actionAbort_download->setVisible(false);
    
    performerView->setVisible(true);
    sessionView->setVisible(false);
    trialView->setVisible(false);
    localView->setVisible(false);

    currentView = performerView;

    menu = new QMenu;
    menu->addSeparator();
    menu->addAction(actionAdd);
    menu->addAction(actionRemove);
    menu->addAction(actionAdd_GRF);
    menu->addAction(actionAdd_EMG);
    menu->addAction(actionAdd_MOCAP);
    menu->addAction(actionAdd_videos);
    menu->addAction(actionRemove_GRF);
    menu->addAction(actionRemove_EMG);
    menu->addAction(actionRemove_MOCAP);
    menu->addAction(actionRemove_videos);

    menuTl = new QMenu;
    menuTl->addSeparator();
    menuTl->addAction(actionLoad_trial);
    menuTl->addSeparator();
    menuTl->addActions(menu->actions());

    menuTs = new QMenu;
    menuTs->addSeparator();
    menuTs->addAction(actionDownload);
    menuTs->addSeparator();
    menuTs->addActions(menu->actions());
    
    connect(performerView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(sessionView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(trialView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(localView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));

    connect(workspace, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(workspaceItemClicked(QTreeWidgetItem*, int)));
}

void CommunicationWidgetEx::treeItemClicked(QTreeWidgetItem* item, int column)
{
    if(item) {
        IEntityTreeItem* current = reinterpret_cast<IEntityTreeItem*>(item);

        int columns = current->columnCount();
        currentView->setColumnCount(columns);
        currentView->setHeaderLabels(current->getHeaders());
        for(int i = 0; i < columns; i++) {
            currentView->resizeColumnToContents(i);
        }
        if(current->isMarkedAll()) {
            actionRemove->setVisible(true);
            actionAdd->setVisible(false);
        } else {
            actionRemove->setVisible(false);
            actionAdd->setVisible(true);
        }
        if(current->isMarkedEmg()) {
            actionRemove_EMG->setVisible(true);
            actionAdd_EMG->setVisible(false);
        } else {
            actionRemove_EMG->setVisible(false);
            actionAdd_EMG->setVisible(true);
        }
        if(current->isMarkedGrf()) {
            actionRemove_GRF->setVisible(true);
            actionAdd_GRF->setVisible(false);
        } else {
            actionRemove_GRF->setVisible(false);
            actionAdd_GRF->setVisible(true);
        }
        if(current->isMarkedMocap()) {
            actionRemove_MOCAP->setVisible(true);
            actionAdd_MOCAP->setVisible(false);
        } else {
            actionRemove_MOCAP->setVisible(false);
            actionAdd_MOCAP->setVisible(true);
        }
        if(current->isMarkedVideos()) {
            actionRemove_videos->setVisible(true);
            actionAdd_videos->setVisible(false);
        } else {
            actionRemove_videos->setVisible(false);
            actionAdd_videos->setVisible(true);
        }
    }
}

void CommunicationWidgetEx::workspaceItemClicked(QTreeWidgetItem* item, int column)
{
    for(int i = 0; i < workspace->columnCount(); i++) {
        workspace->resizeColumnToContents(i);
    }
    //if(item) {
    //    IEntityTreeItem* current = reinterpret_cast<IEntityTreeItem*>(item);
    //    int columns = current->columnCount();
    //    workspace->setColumnCount(columns);
    //    workspace->setHeaderLabels(current->getHeaders());
    //    for(int i = 0; i < columns; i++) {
    //        workspace->resizeColumnToContents(i);
    //    }
    //}
}

void CommunicationWidgetEx::trialContextMenu(QPoint p)
{
    QMenu* view = new QMenu;
    view->setTitle("View");
    view->addSeparator();
    view->addAction(actionPerformer_View);
    view->addAction(actionSession_View);
    view->addAction(actionTrial_View);
    view->addAction(actionLocal_View);

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
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentView->currentItem());
        if(item->getMenu())
            menu->addActions(item->getMenu()->actions());
        menu->setToolTip(item->text(0));
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
    view->addAction(actionLocal_View);

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
    currentView = performerView;

    performerView->setVisible(true);
    sessionView->setVisible(false);
    trialView->setVisible(false);
    localView->setVisible(false);
}

void CommunicationWidgetEx::sessionViewPressed(bool tog)
{
    currentView = sessionView;

    performerView->setVisible(false);
    sessionView->setVisible(true);
    trialView->setVisible(false);
    localView->setVisible(false);
}

void CommunicationWidgetEx::trialViewPressed(bool tog)
{
    currentView = trialView;

    performerView->setVisible(false);
    sessionView->setVisible(false);
    trialView->setVisible(true);
    localView->setVisible(false);
}

void CommunicationWidgetEx::localViewPressed(bool tog)
{
    currentView = localView;

    performerView->setVisible(false);
    sessionView->setVisible(false);
    trialView->setVisible(false);
    localView->setVisible(true);
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
    downloadTrial(reinterpret_cast<TrialTreeItem*>(currentView->currentItem())->getEntityID());
}

void CommunicationWidgetEx::loadPressed()
{
    BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
    {
        boost::cmatch matches;
        boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
        //sprawdzamy, czy zgadza sie nazwa folderu
        if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e)) {
            if(reinterpret_cast<TrialTreeItem*>(currentView->currentItem())->getName() == core::toQString(matches[2])) {
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
    
    //buduj widokki tylko raz po zaladowaniu danych
    buildPerformerView(performerView);
    buildSessionView(sessionView);
    buildTrialView(trialView);
    buildLocalView(localView);
    buildWorkspace(workspace);

    //communicationView->setSortingEnabled(true);
    //communicationView->setDisabled(false);
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
    } else if(actionLocal_View->isChecked()) {
        actionLocal_View->setChecked(false);
        actionLocal_View->setChecked(true);
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
    if(currentView->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentView->currentItem());
        markRecursive(true, item);
    }
    buildWorkspace(workspace);
}

void CommunicationWidgetEx::removeFromWorkspace()
{
    if(currentView->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentView->currentItem());
        markRecursive(false, item);
    }
    buildWorkspace(workspace);
}

void CommunicationWidgetEx::addEMG()
{
    if(currentView->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentView->currentItem());
        markRecursiveEMG(true, item);
    }
    buildWorkspace(workspace);
}

void CommunicationWidgetEx::addGRF()
{
    if(currentView->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentView->currentItem());
        markRecursiveGRF(true, item);
    }
    buildWorkspace(workspace);
}

void CommunicationWidgetEx::addMocap()
{
    if(currentView->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentView->currentItem());
        markRecursiveMocap(true, item);
    }
    buildWorkspace(workspace);
}

void CommunicationWidgetEx::addVideo()
{
    if(currentView->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentView->currentItem());
        markRecursiveVideo(true, item);
    }
    buildWorkspace(workspace);
}

void CommunicationWidgetEx::removeEMG()
{
    if(currentView->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentView->currentItem());
        markRecursiveEMG(false, item);
    }
    buildWorkspace(workspace);
}

void CommunicationWidgetEx::removeGRF()
{
    if(currentView->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentView->currentItem());
        markRecursiveGRF(false, item);
    }
    buildWorkspace(workspace);
}

void CommunicationWidgetEx::removeMocap()
{
    if(currentView->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentView->currentItem());
        markRecursiveMocap(false, item);
    }
    buildWorkspace(workspace);
}

void CommunicationWidgetEx::removeVideo()
{
    if(currentView->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentView->currentItem());
        markRecursiveVideo(false, item);
    }
    buildWorkspace(workspace);
}

void CommunicationWidgetEx::markRecursive(bool mark, IEntityTreeItem* item)
{
    for(int i = 0; i < item->childCount(); i++) {
        markRecursive(mark, reinterpret_cast<IEntityTreeItem*>(item->child(i)));
    }
    item->setMarkedAll(mark);
}

void CommunicationWidgetEx::markRecursiveEMG(bool mark, IEntityTreeItem* item)
{
    for(int i = 0; i < item->childCount(); i++) {
        markRecursiveEMG(mark, reinterpret_cast<IEntityTreeItem*>(item->child(i)));
    }
    item->setMarkedEmg(mark);
}

void CommunicationWidgetEx::markRecursiveGRF(bool mark, IEntityTreeItem* item)
{
    for(int i = 0; i < item->childCount(); i++) {
        markRecursiveGRF(mark, reinterpret_cast<IEntityTreeItem*>(item->child(i)));
    }
    item->setMarkedGrf(mark);
}

void CommunicationWidgetEx::markRecursiveMocap(bool mark, IEntityTreeItem* item)
{
    for(int i = 0; i < item->childCount(); i++) {
        markRecursiveMocap(mark, reinterpret_cast<IEntityTreeItem*>(item->child(i)));
    }
    item->setMarkedMocap(mark);
}

void CommunicationWidgetEx::markRecursiveVideo(bool mark, IEntityTreeItem* item)
{
    for(int i = 0; i < item->childCount(); i++) {
        markRecursiveVideo(mark, reinterpret_cast<IEntityTreeItem*>(item->child(i)));
    }
    item->setMarkedVideos(mark);
}

void CommunicationWidgetEx::buildPerformerView(QTreeWidget* tree)
{
    tree->clear();
    //listowanie performerow
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        PerformerTreeItem* item = new PerformerTreeItem(performer);
        QString name = QString::fromUtf8(performer->performer.firstName.c_str());
        QString lastName = QString::fromUtf8(performer->performer.lastName.c_str());
        
        //item->setPerformer(performer);
        item->setMenu(menu);
        
        tree->addTopLevelItem(item);
        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            SessionTreeItem* itemS = new SessionTreeItem(session);
            itemS->setMenu(menu);
            itemS->setText(3, QString(name + " " + lastName));
            item->addChild(itemS);
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                TrialTreeItem* itemT = createTrialItem(trial);
                itemT->setText(2, core::toQString(session->session.sessionDate).left(10));
                itemT->setText(3, QString(name + " " + lastName));
                itemT->setText(4, QString::fromUtf8(session->session.sessionName.c_str()));
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
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        QString name = QString::fromUtf8(performer->performer.firstName.c_str());
        QString lastName = QString::fromUtf8(performer->performer.lastName.c_str());
        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            SessionTreeItem* item = new SessionTreeItem(session);
            item->setMenu(menu);
            tree->addTopLevelItem(item);
            item->setText(3, QString(name + " " + lastName));
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                TrialTreeItem* itemT = createTrialItem(trial);
                itemT->setText(2, core::toQString(session->session.sessionDate).left(10));
                itemT->setText(3, QString(name + " " + lastName));
                itemT->setText(4, QString::fromUtf8(session->session.sessionName.c_str()));
                item->addChild(itemT);
            }
        }
    }
    refreshHeader(tree);
}

void CommunicationWidgetEx::buildTrialView(QTreeWidget* tree)
{
    tree->clear();
    //listowanie triali
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        QString name = QString::fromUtf8(performer->performer.firstName.c_str());
        QString lastName = QString::fromUtf8(performer->performer.lastName.c_str());
        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                TrialTreeItem* item = createTrialItem(trial);
                item->setText(2, core::toQString(session->session.sessionDate).left(10));
                item->setText(3, QString(name + " " + lastName));
                item->setText(4, QString::fromUtf8(session->session.sessionName.c_str()));
                tree->addTopLevelItem(item);
            }
        }
    }
    refreshHeader(tree);
}

void CommunicationWidgetEx::buildLocalView(QTreeWidget* tree)
{
    tree->clear();
    //listowanie triali
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        QString name = QString::fromUtf8(performer->performer.firstName.c_str());
        QString lastName = QString::fromUtf8(performer->performer.lastName.c_str());
        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                TrialTreeItem* item = createTrialItem(trial);
                item->setText(2, core::toQString(session->session.sessionDate).left(10));
                item->setText(3, QString(name + " " + lastName));
                item->setText(4, QString::fromUtf8(session->session.sessionName.c_str()));
                if(item->textColor(0) != QColor(128, 128, 128)) {
                    tree->addTopLevelItem(item);
                }
            }
        }
    }
    refreshHeader(tree);
}

void CommunicationWidgetEx::buildWorkspace(QTreeWidget* tree)
{
    tree->clear();
    QStringList headers;
    headers << "" << "Date" << "Performer" << "Session";
    tree->setHeaderLabels(headers);
    QTreeWidgetItem* workspace = new QTreeWidgetItem(tree);
    workspace->setText(0, "workspace");
    
    QTreeWidgetItem* mocap = new QTreeWidgetItem;
    mocap->setText(0, "MOCAP");
    mocap->setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/mocap.png")));
    QTreeWidgetItem* emg = new QTreeWidgetItem;
    emg->setText(0, "EMG");
    emg->setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/emg.png")));
    QTreeWidgetItem* grf = new QTreeWidgetItem;
    grf->setText(0, "GRF");
    grf->setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/grf.png")));
    QTreeWidgetItem* video = new QTreeWidgetItem;
    video->setText(0, "VIDEO");
    video->setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/video.png")));
    workspace->addChild(mocap);
    workspace->addChild(emg);
    workspace->addChild(grf);
    workspace->addChild(video);

    //listowanie triali
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        QString name = QString::fromUtf8(performer->performer.firstName.c_str());
        QString lastName = QString::fromUtf8(performer->performer.lastName.c_str());

        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                bool vid = false, c3d = false, amc = false, asf = false;
                BOOST_FOREACH(ShallowCopy::File file, trial->trial.files)
                {
                    if(!boost::filesystem::path(file.fileName).extension().compare(".avi")) {
                        vid = true;
                    } else if(!boost::filesystem::path(file.fileName).extension().compare(".c3d")) {
                        c3d = true;
                    } else if(!boost::filesystem::path(file.fileName).extension().compare(".amc")) {
                        amc = true;
                    } else if(!boost::filesystem::path(file.fileName).extension().compare(".asf")) {
                        asf = true;
                    }
                }
                if(trial->markVideos && vid) {
                    WorkspaceTreeItem* item = createWorkspaceTrialItem(trial);
                    item->setText(1, core::toQString(session->session.sessionDate).left(10));
                    item->setText(2, QString(name + " " + lastName));
                    item->setText(3, QString::fromUtf8(session->session.sessionName.c_str()));
                    video->addChild(item);
                }
                if(trial->markMocap && c3d && amc && asf) {
                    WorkspaceTreeItem* item = createWorkspaceTrialItem(trial);
                    item->setText(1, core::toQString(session->session.sessionDate).left(10));
                    item->setText(2, QString(name + " " + lastName));
                    item->setText(3, QString::fromUtf8(session->session.sessionName.c_str()));
                    mocap->addChild(item);
                }
                if(trial->markEmg && c3d) {
                    WorkspaceTreeItem* item = createWorkspaceTrialItem(trial);
                    item->setText(1, core::toQString(session->session.sessionDate).left(10));
                    item->setText(2, QString(name + " " + lastName));
                    item->setText(3, QString::fromUtf8(session->session.sessionName.c_str()));
                    emg->addChild(item);
                }
                if(trial->markGrf && c3d) {
                    WorkspaceTreeItem* item = createWorkspaceTrialItem(trial);
                    item->setText(1, core::toQString(session->session.sessionDate).left(10));
                    item->setText(2, QString(name + " " + lastName));
                    item->setText(3, QString::fromUtf8(session->session.sessionName.c_str()));
                    grf->addChild(item);
                }
            }
        }
    }
    for(int i = 0; i < tree->columnCount(); i++) {
        tree->resizeColumnToContents(i);
    }
}

CommunicationWidgetEx::TrialTreeItem* CommunicationWidgetEx::createTrialItem(TrialRelationPtr trial)
{
    TrialTreeItem* item = new TrialTreeItem(trial);
    //lokalny czy serwerowy?
    BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
    {
        boost::cmatch matches;
        boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
        //sprawdzamy, czy zgadza sie nazwa folderu
        if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trial.trialName.compare(matches[2])) {
            item->setTextColor(0, QColor(0, 0, 0));
            item->setMenu(menuTl);
            break;
        } else {
            item->setTextColor(0, QColor(128, 128, 128));
            item->setMenu(menuTs);
        }
    }
    return item;
}

CommunicationWidgetEx::WorkspaceTreeItem* CommunicationWidgetEx::createWorkspaceTrialItem(TrialRelationPtr trial)
{
    WorkspaceTreeItem* item = new WorkspaceTreeItem(trial);
    //lokalny czy serwerowy?
    BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
    {
        boost::cmatch matches;
        boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
        //sprawdzamy, czy zgadza sie nazwa folderu
        if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trial.trialName.compare(matches[2])) {
            item->setTextColor(0, QColor(0, 0, 0));
            item->setMenu(menuTl);
            break;
        } else {
            item->setTextColor(0, QColor(128, 128, 128));
            item->setMenu(menuTs);
        }
    }
    return item;
}

void CommunicationWidgetEx::refreshHeader(QTreeWidget* tree)
{
    //headery
    if(tree->topLevelItem(0)) {
        IEntityTreeItem* current = reinterpret_cast<IEntityTreeItem*>(tree->topLevelItem(0));

        int columns = current->columnCount();
        tree->setColumnCount(columns);
        tree->setHeaderLabels(current->getHeaders());
        for(int i = 0; i < columns; i++) {
            tree->resizeColumnToContents(i);
        }
    }
}