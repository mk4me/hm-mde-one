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

CommunicationWidgetEx::WorkspaceTopTreeItem::WorkspaceTopTreeItem()
{
    menu = nullptr;
    headers << "" << "Date" << "Performer" << "Session";
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

    
    QActionGroup* formatGroupWorkspace = new QActionGroup(this);
    formatGroupWorkspace->addAction(actionWorkspace_View);
    formatGroupWorkspace->addAction(actionWorkspaceTrial_View);
    
    actionAbort_download->setVisible(false);
    
    performerView->setVisible(true);
    sessionView->setVisible(false);
    trialView->setVisible(false);
    localView->setVisible(false);
    workspace->setVisible(true);
    workspaceTrialView->setVisible(false);

    currentView = performerView;
    currentWorkspaceView = workspace;

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
    menuTs->addActions(menu->actions());
    menuTs->addSeparator();
    menuTs->addAction(actionDownload);

    menuTw = new QMenu;
    menuTw->addSeparator();
    menuTw->addActions(menu->actions());
    menuTw->addSeparator();
    menuTw->addAction(actionLoad_trial);
    menuTw->addAction(actionLoad_Trial_GRF);
    menuTw->addAction(actionLoad_Trial_EMG);
    menuTw->addAction(actionLoad_Trial_MOCAP);
    menuTw->addAction(actionLoad_Trial_videos);

    menuWorkspace = new QMenu;
    menuWorkspace->addSeparator();
    menuWorkspace->addAction(actionRemove);
    menuWorkspace->addAction(actionRemove_GRF);
    menuWorkspace->addAction(actionRemove_EMG);
    menuWorkspace->addAction(actionRemove_MOCAP);
    menuWorkspace->addAction(actionRemove_videos);
    menuWorkspace->addSeparator();
    menuWorkspace->addAction(actionLoad_All);
    menuWorkspace->addAction(actionLoad_GRF);
    menuWorkspace->addAction(actionLoad_EMG);
    menuWorkspace->addAction(actionLoad_MOCAP);
    menuWorkspace->addAction(actionLoad_Videos);

    menuWorkspaceMocap = new QMenu;
    menuWorkspaceMocap->addSeparator();
    menuWorkspaceMocap->addAction(actionRemove_MOCAP);
    menuWorkspaceMocap->addSeparator();
    menuWorkspaceMocap->addAction(actionLoad_MOCAP);

    menuWorkspaceEmg = new QMenu;
    menuWorkspaceEmg->addSeparator();
    menuWorkspaceEmg->addAction(actionRemove_EMG);
    menuWorkspaceEmg->addSeparator();
    menuWorkspaceEmg->addAction(actionLoad_EMG);

    menuWorkspaceGrf = new QMenu;
    menuWorkspaceGrf->addSeparator();
    menuWorkspaceGrf->addAction(actionRemove_GRF);
    menuWorkspaceGrf->addSeparator();
    menuWorkspaceGrf->addAction(actionLoad_GRF);

    menuWorkspaceVideos = new QMenu;
    menuWorkspaceVideos->addSeparator();
    menuWorkspaceVideos->addAction(actionRemove_videos);
    menuWorkspaceVideos->addSeparator();
    menuWorkspaceVideos->addAction(actionLoad_Videos);

    connect(performerView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(sessionView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(trialView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(localView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    
    connect(workspace, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(workspaceItemClicked(QTreeWidgetItem*, int)));
    connect(workspaceTrialView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(workspaceItemClicked(QTreeWidgetItem*, int)));
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
    if(item) {
        IEntityTreeItem* current = reinterpret_cast<IEntityTreeItem*>(item);

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
    }
    currentViewRequest = true;
    menu->exec(mapToGlobal(p));
}

void CommunicationWidgetEx::workspaceContextMenu(QPoint p)
{
    QMenu* view = new QMenu;
    view->setTitle("View");
    view->addSeparator();
    view->addAction(actionWorkspace_View);
    view->addAction(actionWorkspaceTrial_View);

    QMenu* menu = new QMenu;
    menu->addSeparator();
    menu->addAction(actionUpdate);
    menu->addAction(actionAbort_download);

    menu->addSeparator();
    menu->addMenu(view);
    menu->addSeparator();

    //currentItem() przy drugim kliknieciu automatycznie wybiera pierwszy element drzewa
    //sprawdzamy czy cokolwiek jest wybrane sprawdzajac liste zaznaczonych itemow
    if(currentWorkspaceView && currentWorkspaceView->currentItem() && currentWorkspaceView->selectedItems().size() > 0) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(currentWorkspaceView->currentItem());
        if(item->getMenu())
            menu->addActions(item->getMenu()->actions());
    }
    currentViewRequest = false;
    QPoint point(mapToGlobal(p));
    point.setY(point.y() + currentView->height());
    menu->exec(mapToGlobal(point));
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

void CommunicationWidgetEx::workspaceViewPressed(bool tog)
{
    currentWorkspaceView = workspace;

    workspace->setVisible(true);
    workspaceTrialView->setVisible(false);
}

void CommunicationWidgetEx::workspaceTrialViewPressed(bool tog)
{
    currentWorkspaceView = workspaceTrialView;

    workspace->setVisible(false);
    workspaceTrialView->setVisible(true);
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
    QTreeWidget* widget;
    if(currentViewRequest) {
        widget = currentView;
    } else {
        widget = currentWorkspaceView;
    }
    actionDownload->setDisabled(true);
    actionUpdate->setDisabled(true);
    actionLoad_trial->setDisabled(false);
    actionAbort_download->setDisabled(false);
    actionAbort_download->setVisible(true);
    downloadTrial(reinterpret_cast<TrialTreeItem*>(widget->currentItem())->getEntityID());
}

void CommunicationWidgetEx::loadPressed()
{
    QTreeWidget* widget;
    if(currentViewRequest) {
        widget = currentView;
    } else {
        widget = currentWorkspaceView;
    }
    BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
    {
        boost::cmatch matches;
        boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
        //sprawdzamy, czy zgadza sie nazwa folderu
        if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e)) {
            if(reinterpret_cast<TrialTreeItem*>(widget->currentItem())->getName() == core::toQString(matches[2])) {
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

void CommunicationWidgetEx::loadFiles(const std::vector<core::IDataManager::Path>& files)
{
    communicationService->loadFiles(files);
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
    buildWorkspaceView(workspace);
    buildWorkspaceTrialView(workspaceTrialView);

    progressBar->reset();
    if(actionSession_View->isChecked()) {
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

    if(actionWorkspaceTrial_View->isChecked()) {   
        actionWorkspaceTrial_View->setChecked(false);
        actionWorkspaceTrial_View->setChecked(true);
    } else {
        actionWorkspace_View->setChecked(false);
        actionWorkspace_View->setChecked(true);
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
    QTreeWidget* widget;
    if(currentViewRequest) {
        widget = currentView;
    } else {
        widget = currentWorkspaceView;
    }
    if(widget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(widget->currentItem());
        markRecursive(true, item);
    }
    buildWorkspaceView(workspace);
    buildWorkspaceTrialView(workspaceTrialView);
}

void CommunicationWidgetEx::removeFromWorkspace()
{
    QTreeWidget* widget;
    if(currentViewRequest) {
        widget = currentView;
    } else {
        widget = currentWorkspaceView;
    }
    if(widget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(widget->currentItem());
        markRecursive(false, item);
    }
    buildWorkspaceView(workspace);
    buildWorkspaceTrialView(workspaceTrialView);
}

void CommunicationWidgetEx::addEMG()
{
    QTreeWidget* widget;
    if(currentViewRequest) {
        widget = currentView;
    } else {
        widget = currentWorkspaceView;
    }
    if(widget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(widget->currentItem());
        markRecursiveEMG(true, item);
    }
    buildWorkspaceView(workspace);
    buildWorkspaceTrialView(workspaceTrialView);
}

void CommunicationWidgetEx::addGRF()
{
    QTreeWidget* widget;
    if(currentViewRequest) {
        widget = currentView;
    } else {
        widget = currentWorkspaceView;
    }
    if(widget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(widget->currentItem());
        markRecursiveGRF(true, item);
    }
    buildWorkspaceView(workspace);
    buildWorkspaceTrialView(workspaceTrialView);
}

void CommunicationWidgetEx::addMocap()
{
    QTreeWidget* widget;
    if(currentViewRequest) {
        widget = currentView;
    } else {
        widget = currentWorkspaceView;
    }
    if(widget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(widget->currentItem());
        markRecursiveMocap(true, item);
    }
    buildWorkspaceView(workspace);
    buildWorkspaceTrialView(workspaceTrialView);
}

void CommunicationWidgetEx::addVideo()
{
    QTreeWidget* widget;
    if(currentViewRequest) {
        widget = currentView;
    } else {
        widget = currentWorkspaceView;
    }
    if(widget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(widget->currentItem());
        markRecursiveVideo(true, item);
    }
    buildWorkspaceView(workspace);
    buildWorkspaceTrialView(workspaceTrialView);
}

void CommunicationWidgetEx::removeEMG()
{
    QTreeWidget* widget;
    if(currentViewRequest) {
        widget = currentView;
    } else {
        widget = currentWorkspaceView;
    }
    if(widget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(widget->currentItem());
        markRecursiveEMG(false, item);
    }
    buildWorkspaceView(workspace);
    buildWorkspaceTrialView(workspaceTrialView);
}

void CommunicationWidgetEx::removeGRF()
{
    QTreeWidget* widget;
    if(currentViewRequest) {
        widget = currentView;
    } else {
        widget = currentWorkspaceView;
    }
    if(widget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(widget->currentItem());
        markRecursiveGRF(false, item);
    }
    buildWorkspaceView(workspace);
    buildWorkspaceTrialView(workspaceTrialView);
}

void CommunicationWidgetEx::removeMocap()
{
    QTreeWidget* widget;
    if(currentViewRequest) {
        widget = currentView;
    } else {
        widget = currentWorkspaceView;
    }
    if(widget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(widget->currentItem());
        markRecursiveMocap(false, item);
    }
    buildWorkspaceView(workspace);
    buildWorkspaceTrialView(workspaceTrialView);
}

void CommunicationWidgetEx::removeVideo()
{
    QTreeWidget* widget;
    if(currentViewRequest) {
        widget = currentView;
    } else {
        widget = currentWorkspaceView;
    }
    if(widget->currentItem()) {
        IEntityTreeItem* item = reinterpret_cast<IEntityTreeItem*>(widget->currentItem());
        markRecursiveVideo(false, item);
    }
    buildWorkspaceView(workspace);
    buildWorkspaceTrialView(workspaceTrialView);
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

void CommunicationWidgetEx::useAllFromWorkspace()
{
    std::vector<core::IDataManager::Path> files;
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                //lokalny czy serwerowy?
                BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
                {
                    boost::cmatch matches;
                    boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
                    //sprawdzamy, czy zgadza sie nazwa folderu
                    if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trial.trialName.compare(matches[2])) {
                        if(trial->markMocap) {
                            BOOST_FOREACH(ShallowCopy::File file, trial->trial.files)
                            {
                                boost::filesystem::path f("data/trials");
                                f /= boost::filesystem::path(trial->trial.trialName) /= boost::filesystem::path(file.fileName);
                                if(!f.extension().compare(".c3d") || !f.extension().compare(".amc") || !f.extension().compare(".asf") || !f.extension().compare(".avi")) {
                                    files.push_back(f);
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }

    }
    loadFiles(files);
}

void CommunicationWidgetEx::useVideos()
{
    std::vector<core::IDataManager::Path> files;
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                //lokalny czy serwerowy?
                BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
                {
                    boost::cmatch matches;
                    boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
                    //sprawdzamy, czy zgadza sie nazwa folderu
                    if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trial.trialName.compare(matches[2])) {
                        if(trial->markVideos) {
                            BOOST_FOREACH(ShallowCopy::File file, trial->trial.files)
                            {
                                boost::filesystem::path f("data/trials");
                                f /= boost::filesystem::path(trial->trial.trialName) /= boost::filesystem::path(file.fileName);
                                if(!f.extension().compare(".avi")) {
                                    files.push_back(f);
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    loadFiles(files);
}

void CommunicationWidgetEx::useMocap()
{
    std::vector<core::IDataManager::Path> files;
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                //lokalny czy serwerowy?
                BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
                {
                    boost::cmatch matches;
                    boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
                    //sprawdzamy, czy zgadza sie nazwa folderu
                    if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trial.trialName.compare(matches[2])) {
                        if(trial->markMocap) {
                            BOOST_FOREACH(ShallowCopy::File file, trial->trial.files)
                            {
                                boost::filesystem::path f("data/trials");
                                f /= boost::filesystem::path(trial->trial.trialName) /= boost::filesystem::path(file.fileName);
                                if(!f.extension().compare(".c3d") || !f.extension().compare(".amc") || !f.extension().compare(".asf")) {
                                    files.push_back(f);
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    loadFiles(files);
}

void CommunicationWidgetEx::useGrf()
{
    std::vector<core::IDataManager::Path> files;
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                //lokalny czy serwerowy?
                BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
                {
                    boost::cmatch matches;
                    boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
                    //sprawdzamy, czy zgadza sie nazwa folderu
                    if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trial.trialName.compare(matches[2])) {
                        if(trial->markGrf) {
                            BOOST_FOREACH(ShallowCopy::File file, trial->trial.files)
                            {
                                boost::filesystem::path f("data/trials");
                                f /= boost::filesystem::path(trial->trial.trialName) /= boost::filesystem::path(file.fileName);
                                if(!f.extension().compare(".c3d")) {
                                    files.push_back(f);
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    loadFiles(files);
}

void CommunicationWidgetEx::useEmg()
{
    std::vector<core::IDataManager::Path> files;
    BOOST_FOREACH(PerformerRelationPtr performer, performersWithRelations)
    {
        BOOST_FOREACH(SessionRelationPtr session, performer->sessions)
        {
            BOOST_FOREACH(TrialRelationPtr trial, session->trials)
            {
                //lokalny czy serwerowy?
                BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
                {
                    boost::cmatch matches;
                    boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
                    //sprawdzamy, czy zgadza sie nazwa folderu
                    if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trial.trialName.compare(matches[2])) {
                        if(trial->markEmg) {
                            BOOST_FOREACH(ShallowCopy::File file, trial->trial.files)
                            {
                                boost::filesystem::path f("data/trials");
                                f /= boost::filesystem::path(trial->trial.trialName) /= boost::filesystem::path(file.fileName);
                                if(!f.extension().compare(".c3d")) {
                                    files.push_back(f);
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    loadFiles(files);
}

void CommunicationWidgetEx::useTrialVideos()
{
    std::vector<core::IDataManager::Path> files;
    if(currentWorkspaceView->currentItem()) {
        WorkspaceTreeItem* item = reinterpret_cast<WorkspaceTreeItem*>(currentWorkspaceView->currentItem());
        if(item->isMarkedVideos()) {
            //lokalny czy serwerowy?
            BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
            {
                boost::cmatch matches;
                boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
                //sprawdzamy, czy zgadza sie nazwa folderu
                if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !item->getTrial()->trial.trialName.compare(matches[2])) {
                    if(item->getTrial()->markVideos) {
                        BOOST_FOREACH(ShallowCopy::File file, item->getTrial()->trial.files)
                        {
                            boost::filesystem::path f("data/trials");
                            f /= boost::filesystem::path(item->getTrial()->trial.trialName) /= boost::filesystem::path(file.fileName);
                            if(!f.extension().compare(".avi")) {
                                files.push_back(f);
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    loadFiles(files);
}

void CommunicationWidgetEx::useTrialMocap()
{
    std::vector<core::IDataManager::Path> files;
    if(currentWorkspaceView->currentItem()) {
        WorkspaceTreeItem* item = reinterpret_cast<WorkspaceTreeItem*>(currentWorkspaceView->currentItem());
        if(item->isMarkedVideos()) {
            //lokalny czy serwerowy?
            BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
            {
                boost::cmatch matches;
                boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
                //sprawdzamy, czy zgadza sie nazwa folderu
                if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !item->getTrial()->trial.trialName.compare(matches[2])) {
                    if(item->getTrial()->markMocap) {
                        BOOST_FOREACH(ShallowCopy::File file, item->getTrial()->trial.files)
                        {
                            boost::filesystem::path f("data/trials");
                            f /= boost::filesystem::path(item->getTrial()->trial.trialName) /= boost::filesystem::path(file.fileName);
                                if(!f.extension().compare(".c3d") || !f.extension().compare(".amc") || !f.extension().compare(".asf")) {
                                files.push_back(f);
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    loadFiles(files);
}

void CommunicationWidgetEx::useTrialGrf()
{
    std::vector<core::IDataManager::Path> files;
    if(currentWorkspaceView->currentItem()) {
        WorkspaceTreeItem* item = reinterpret_cast<WorkspaceTreeItem*>(currentWorkspaceView->currentItem());
        if(item->isMarkedVideos()) {
            //lokalny czy serwerowy?
            BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
            {
                boost::cmatch matches;
                boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
                //sprawdzamy, czy zgadza sie nazwa folderu
                if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !item->getTrial()->trial.trialName.compare(matches[2])) {
                    if(item->getTrial()->markGrf) {
                        BOOST_FOREACH(ShallowCopy::File file, item->getTrial()->trial.files)
                        {
                            boost::filesystem::path f("data/trials");
                            f /= boost::filesystem::path(item->getTrial()->trial.trialName) /= boost::filesystem::path(file.fileName);
                            if(!f.extension().compare(".c3d")) {
                                files.push_back(f);
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    loadFiles(files);
}

void CommunicationWidgetEx::useTrialEmg()
{
    std::vector<core::IDataManager::Path> files;
    if(currentWorkspaceView->currentItem()) {
        WorkspaceTreeItem* item = reinterpret_cast<WorkspaceTreeItem*>(currentWorkspaceView->currentItem());
        if(item->isMarkedVideos()) {
            //lokalny czy serwerowy?
            BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
            {
                boost::cmatch matches;
                boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
                //sprawdzamy, czy zgadza sie nazwa folderu
                if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !item->getTrial()->trial.trialName.compare(matches[2])) {
                    if(item->getTrial()->markEmg) {
                        BOOST_FOREACH(ShallowCopy::File file, item->getTrial()->trial.files)
                        {
                            boost::filesystem::path f("data/trials");
                            f /= boost::filesystem::path(item->getTrial()->trial.trialName) /= boost::filesystem::path(file.fileName);
                            if(!f.extension().compare(".c3d")) {
                                files.push_back(f);
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    loadFiles(files);
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

void CommunicationWidgetEx::buildWorkspaceView(QTreeWidget* tree)
{
    tree->clear();
    QStringList headers;
    headers << "" << "Date" << "Performer" << "Session";
    tree->setHeaderLabels(headers);
    WorkspaceTopTreeItem* workspace = new WorkspaceTopTreeItem;
    tree->addTopLevelItem(workspace);
    workspace->setText(0, "workspace");
    workspace->setMenu(menuWorkspace);
    
    WorkspaceTopTreeItem* mocap = new WorkspaceTopTreeItem;
    mocap->setText(0, "MOCAP");
    mocap->setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/mocap.png")));
    mocap->setMenu(menuWorkspaceMocap);
    WorkspaceTopTreeItem* emg = new WorkspaceTopTreeItem;
    emg->setText(0, "EMG");
    emg->setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/emg.png")));
    emg->setMenu(menuWorkspaceEmg);
    WorkspaceTopTreeItem* grf = new WorkspaceTopTreeItem;
    grf->setText(0, "GRF");
    grf->setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/grf.png")));
    grf->setMenu(menuWorkspaceGrf);
    WorkspaceTopTreeItem* video = new WorkspaceTopTreeItem;
    video->setText(0, "VIDEO");
    video->setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/video.png")));
    video->setMenu(menuWorkspaceVideos);
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

void CommunicationWidgetEx::buildWorkspaceTrialView(QTreeWidget* tree)
{
    tree->clear();
    QStringList headers;
    headers << "" << "Date" << "Performer" << "Session";
    tree->setHeaderLabels(headers);
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
                if((trial->markVideos && vid) || (trial->markMocap && c3d && amc && asf) || (trial->markEmg && c3d) || (trial->markGrf && c3d)) {
                    WorkspaceTreeItem* item = createWorkspaceTrialItem(trial);
                    item->setText(1, core::toQString(session->session.sessionDate).left(10));
                    item->setText(2, QString(name + " " + lastName));
                    item->setText(3, QString::fromUtf8(session->session.sessionName.c_str()));
                    tree->addTopLevelItem(item);
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
            item->setMenu(menuTw);
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