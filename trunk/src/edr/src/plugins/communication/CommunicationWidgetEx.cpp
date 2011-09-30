#include "CommunicationPCH.h"
#include "CommunicationWidgetEx.h"
#include <plugins/communication/CommunicationService.h>
#include <core/StringTools.h>
#include <core/StringTools.h>
#include <QtGui/QPainter>
#include <boost/regex.hpp>

//http://hmdb.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl
//http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl

using namespace communication;

CommunicationWidgetEx::PerformerTreeItem::PerformerTreeItem(PerformerRelationPtr performer)
    : performer(performer)
{
    menu = nullptr;
    headers << "" << "Element";
    setToolTip(0, "Performer");
    setIcon(0, QIcon(core::getResourceString("icons/subject.png")));

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
    setIcon(0, QIcon(core::getResourceString("icons/session.png")));

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
    setIcon(0, QIcon(core::getResourceString("icons/trial.png")));
    setText(0, QString::fromUtf8(trial->trial.trialName.c_str()).right(11));

    QString files;
    bool video = false, c3d = false, amc = false, asf = false;
    BOOST_FOREACH(communication::ShallowCopy::File file, trial->trial.files)
    {
        if(core::Filesystem::fileExtension(file.fileName).compare(".avi") == 0) {
            video = true;
        } else if(core::Filesystem::fileExtension(file.fileName).compare(".c3d") == 0) {
            c3d = true;
        } else if(core::Filesystem::fileExtension(file.fileName).compare(".amc") == 0) {
            amc = true;
        } else if(core::Filesystem::fileExtension(file.fileName).compare(".asf") == 0) {
            asf = true;
        }
    }
    if(video) {
        files.append("VID");
    }
    if(c3d) {
        files.append(" EMG GRF");
    }
    if(amc && asf && c3d) {
        files.append(" MOC");
    }
    setText(1, QString::number(trial->trial.files.size()).append(" file(s) [").append(files).append("]"));
}

const QString CommunicationWidgetEx::TrialTreeItem::getName() const
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
    formatGroup->addAction(actionLocal_View);

    actionAbort_download->setVisible(false);

    currentView = performerView;

    menu = new QMenu;
    menu->addSeparator();
    
    view = new QMenu;
    view->setTitle("View");
    view->addSeparator();
    view->addAction(actionPerformer_View);
    view->addAction(actionSession_View);
    view->addAction(actionTrial_View);
    view->addAction(actionLocal_View);

    menuTl = new QMenu;
    menuTl->addSeparator();
    menuTl->addActions(menu->actions());
    menuTl->addSeparator();
    menuTl->addAction(actionLoad_trial);
    menuTl->addAction(actionLoad_Trial_videos);
    menuTl->addAction(actionLoad_Trial_GRF);
    menuTl->addAction(actionLoad_Trial_EMG);
    menuTl->addAction(actionLoad_Trial_MOCAP);

    menuTs = new QMenu;
    menuTs->addSeparator();
    menuTs->addActions(menu->actions());
    menuTs->addSeparator();
    menuTs->addAction(actionDownload);

    connect(performerView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(sessionView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(trialView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
    connect(localView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
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
    //budujemy relacje miedzy encjami bazodanowymi
    communication::ShallowCopy::ShallowCopy shallowCopy = subject->getShalloCopy();
    performersWithRelations.clear();

    BOOST_FOREACH(communication::ShallowCopy::Performer performer, shallowCopy.performers)
    {
        PerformerRelationPtr p = PerformerRelationPtr(new PerformerRelation);
        p->performer = performer;
        performersWithRelations.push_back(p);
        //sesje polaczone z performerami
        BOOST_FOREACH(communication::ShallowCopy::PerformerConf performerC, shallowCopy.performerConfs)
        {
            BOOST_FOREACH(communication::ShallowCopy::Session session, shallowCopy.sessions)
            {
                if(performerC.performerID == p->performer.performerID && performerC.sessionID == session.sessionID) {
                    SessionRelationPtr s = SessionRelationPtr(new SessionRelation);
                    s->session = session;
                    p->sessions.push_back(s);
                    //kojarzenie sesji z trialami
                    BOOST_FOREACH(communication::ShallowCopy::Trial trial, shallowCopy.trials)
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
    
    //buduj widoki tylko raz po zaladowaniu danych
    buildPerformerView(performerView);
    buildSessionView(sessionView);
    buildTrialView(trialView);
    buildLocalView(localView);

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

void CommunicationWidgetEx::useVideos()
{
    std::vector<core::Filesystem::Path> files;
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
                    boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})(.*)");
                    //sprawdzamy, czy zgadza sie nazwa folderu
                    if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trial.trialName.compare(matches[2])) {
                        if(trial->markVideos) {
                            BOOST_FOREACH(communication::ShallowCopy::File file, trial->trial.files)
                            {
                                core::Filesystem::Path f("trials");
                                f /= core::Filesystem::Path(trial->trial.trialName) /= core::Filesystem::Path(file.fileName);
                                if(core::Filesystem::fileExtension(f).compare(".avi") == 0) {
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
    std::vector<core::Filesystem::Path> files;
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
                    boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})(.*)");
                    //sprawdzamy, czy zgadza sie nazwa folderu
                    if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trial.trialName.compare(matches[2])) {
                        if(trial->markMocap) {
                            BOOST_FOREACH(communication::ShallowCopy::File file, trial->trial.files)
                            {
                                core::Filesystem::Path f("trials");
                                f /= core::Filesystem::Path(trial->trial.trialName) /= core::Filesystem::Path(file.fileName);
                                if(core::Filesystem::fileExtension(f).compare(".c3d") == 0 || core::Filesystem::fileExtension(f).compare(".amc") == 0 || core::Filesystem::fileExtension(f).compare(".asf") == 0) {
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
    std::vector<core::Filesystem::Path> files;
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
                    boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})(.*)");
                    //sprawdzamy, czy zgadza sie nazwa folderu
                    if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trial.trialName.compare(matches[2])) {
                        if(trial->markGrf) {
                            BOOST_FOREACH(communication::ShallowCopy::File file, trial->trial.files)
                            {
                                core::Filesystem::Path f("trials");
                                f /= core::Filesystem::Path(trial->trial.trialName) /= core::Filesystem::Path(file.fileName);
                                if(core::Filesystem::fileExtension(f).compare(".c3d") == 0) {
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
    std::vector<core::Filesystem::Path> files;
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
                    boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})(.*)");
                    //sprawdzamy, czy zgadza sie nazwa folderu
                    if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !trial->trial.trialName.compare(matches[2])) {
                        if(trial->markEmg) {
                            BOOST_FOREACH(communication::ShallowCopy::File file, trial->trial.files)
                            {
                                core::Filesystem::Path f("trials");
                                f /= core::Filesystem::Path(trial->trial.trialName) /= core::Filesystem::Path(file.fileName);
                                if(core::Filesystem::fileExtension(f).compare(".c3d") == 0) {
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
    //std::vector<core::IDataManager::Path> files;
    //if(currentView->currentItem()) {
    //    TrialTreeItem* item = reinterpret_cast<TrialTreeItem*>(currentView->currentItem());
    //    if(item->isMarkedVideos()) {
    //        //lokalny czy serwerowy?
    //        BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
    //        {
    //            boost::cmatch matches;
    //            boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})(.*)");
    //            //sprawdzamy, czy zgadza sie nazwa folderu
    //            if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !item->getTrial()->trial.trialName.compare(matches[2])) {
    //                if(item->getTrial()->markVideos) {
    //                    BOOST_FOREACH(communication::ShallowCopy::File file, item->getTrial()->trial.files)
    //                    {
    //                        core::Filesystem::Path f("data/trials");
    //                        f /= core::Filesystem::Path(item->getTrial()->trial.trialName) /= core::Filesystem::Path(file.fileName);
    //                        if(!f.extension().compare(".avi")) {
    //                            files.push_back(f);
    //                        }
    //                    }
    //                }
    //                break;
    //            }
    //        }
    //    }
    //}
    loadFiles(listTrialFiles(false, false, false, true));
}

void CommunicationWidgetEx::useTrialMocap()
{
    //std::vector<core::IDataManager::Path> files;
    //if(currentView->currentItem()) {
    //    TrialTreeItem* item = reinterpret_cast<TrialTreeItem*>(currentView->currentItem());
    //    if(item->isMarkedVideos()) {
    //        //lokalny czy serwerowy?
    //        BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
    //        {
    //            boost::cmatch matches;
    //            boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})(.*)");
    //            //sprawdzamy, czy zgadza sie nazwa folderu
    //            if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !item->getTrial()->trial.trialName.compare(matches[2])) {
    //                if(item->getTrial()->markMocap) {
    //                    BOOST_FOREACH(communication::ShallowCopy::File file, item->getTrial()->trial.files)
    //                    {
    //                        core::Filesystem::Path f("data/trials");
    //                        f /= core::Filesystem::Path(item->getTrial()->trial.trialName) /= core::Filesystem::Path(file.fileName);
    //                            if(!f.extension().compare(".c3d") || !f.extension().compare(".amc") || !f.extension().compare(".asf")) {
    //                            files.push_back(f);
    //                        }
    //                    }
    //                }
    //                break;
    //            }
    //        }
    //    }
    //}
    //loadFiles(files);
    loadFiles(listTrialFiles(true, false, false, false));
}

void CommunicationWidgetEx::useTrialGrf()
{
    //std::vector<core::IDataManager::Path> files;
    //if(currentView->currentItem()) {
    //    TrialTreeItem* item = reinterpret_cast<TrialTreeItem*>(currentView->currentItem());
    //    if(item->isMarkedVideos()) {
    //        //lokalny czy serwerowy?
    //        BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
    //        {
    //            boost::cmatch matches;
    //            boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})(.*)");
    //            //sprawdzamy, czy zgadza sie nazwa folderu
    //            if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !item->getTrial()->trial.trialName.compare(matches[2])) {
    //                if(item->getTrial()->markGrf) {
    //                    BOOST_FOREACH(communication::ShallowCopy::File file, item->getTrial()->trial.files)
    //                    {
    //                        core::Filesystem::Path f("data/trials");
    //                        f /= core::Filesystem::Path(item->getTrial()->trial.trialName) /= core::Filesystem::Path(file.fileName);
    //                        if(!f.extension().compare(".c3d")) {
    //                            files.push_back(f);
    //                        }
    //                    }
    //                }
    //                break;
    //            }
    //        }
    //    }
    //}
    //loadFiles(files);
    loadFiles(listTrialFiles(false, false, true, false));
}

void CommunicationWidgetEx::useTrialEmg()
{
    //std::vector<core::IDataManager::Path> files;
    //if(currentView->currentItem()) {
    //    TrialTreeItem* item = reinterpret_cast<TrialTreeItem*>(currentView->currentItem());
    //    if(item->isMarkedVideos()) {
    //        //lokalny czy serwerowy?
    //        BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
    //        {
    //            boost::cmatch matches;
    //            boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})(.*)");
    //            //sprawdzamy, czy zgadza sie nazwa folderu
    //            if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !item->getTrial()->trial.trialName.compare(matches[2])) {
    //                if(item->getTrial()->markEmg) {
    //                    BOOST_FOREACH(communication::ShallowCopy::File file, item->getTrial()->trial.files)
    //                    {
    //                        core::Filesystem::Path f("data/trials");
    //                        f /= core::Filesystem::Path(item->getTrial()->trial.trialName) /= core::Filesystem::Path(file.fileName);
    //                        if(!f.extension().compare(".c3d")) {
    //                            files.push_back(f);
    //                        }
    //                    }
    //                }
    //                break;
    //            }
    //        }
    //    }
    //}
    //loadFiles(files);
    loadFiles(listTrialFiles(false, true, false, false));
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

CommunicationWidgetEx::TrialTreeItem* CommunicationWidgetEx::createTrialItem(TrialRelationPtr trial)
{
    TrialTreeItem* item = new TrialTreeItem(trial);
    //lokalny czy serwerowy? jeœli nie ma w ogole lokalnych prob to zaznaczamy od razu jako serwerowy
    if(localTrials.size() != 0) {
        BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
        {
            boost::cmatch matches;
            boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})(.*)");
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

std::vector<core::Filesystem::Path> CommunicationWidgetEx::listTrialFiles(bool mocap, bool emg, bool grf, bool videos)
{
    std::vector<core::Filesystem::Path> files;
    if(currentView->currentItem()) {
        //TrialTreeItem* item = reinterpret_cast<TrialTreeItem*>(currentView->currentItem());
        TrialTreeItem* item = dynamic_cast<TrialTreeItem*>(currentView->currentItem());
        //lokalny czy serwerowy?
        BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
        {
            boost::cmatch matches;
            boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-B\\d{4,}-S\\d{2,}-T\\d{2,})(.*)");
            //sprawdzamy, czy zgadza sie nazwa folderu
            if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e) && !item->getTrial()->trial.trialName.compare(matches[2])) {
                if(videos) {
                    BOOST_FOREACH(communication::ShallowCopy::File file, item->getTrial()->trial.files)
                    {
                        core::Filesystem::Path f("trials");
                        f /= core::Filesystem::Path(item->getTrial()->trial.trialName) / core::Filesystem::Path(file.fileName);
                        if(core::Filesystem::fileExtension(f).compare(".avi") == 0) {
                            files.push_back(f);
                        }
                    }
                }
                if(mocap) {
                    BOOST_FOREACH(communication::ShallowCopy::File file, item->getTrial()->trial.files)
                    {
                        core::Filesystem::Path f("trials");
                        f /= core::Filesystem::Path(item->getTrial()->trial.trialName) / core::Filesystem::Path(file.fileName);
                        if(core::Filesystem::fileExtension(f).compare(".c3d") == 0 || core::Filesystem::fileExtension(f).compare(".asf") == 0 || core::Filesystem::fileExtension(f).compare(".amc") == 0) {
                            files.push_back(f);
                        }
                    }
                }
                if(grf) {
                    BOOST_FOREACH(communication::ShallowCopy::File file, item->getTrial()->trial.files)
                    {
                        core::Filesystem::Path f("trials");
                        f /= core::Filesystem::Path(item->getTrial()->trial.trialName) / core::Filesystem::Path(file.fileName);
                        if(core::Filesystem::fileExtension(f).compare(".c3d") == 0) {
                            files.push_back(f);
                        }
                    }
                }
                if(emg) {
                    BOOST_FOREACH(communication::ShallowCopy::File file, item->getTrial()->trial.files)
                    {
                        core::Filesystem::Path f("trials");
                        f /= core::Filesystem::Path(item->getTrial()->trial.trialName) /= core::Filesystem::Path(file.fileName);
                        if(core::Filesystem::fileExtension(f).compare(".c3d") == 0) {
                            files.push_back(f);
                        }
                    }
                }
                break;
            }
        }
    }
    return files;
}
