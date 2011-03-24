#include "CommunicationPCH.h"
#include "CommunicationWidgetEx.h"
#include <core/StringTools.h>
#include <core/StringTools.h>

//http://hmdb.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl
//http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl

using namespace communication;

PerformerTreeItem::PerformerTreeItem()
{
    setToolTip(0, "");
    setToolTip(1, "Sessions");
    setToolTip(2, "Description");
    setToolTip(3, "Notes");
    setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/subject.png")));
}

void PerformerTreeItem::setPerformer(const ShallowCopy::Performer& performer)
{
    this->performer = performer;

    QString name = QString::fromUtf8(performer.firstName.c_str());
    QString lastName = QString::fromUtf8(performer.lastName.c_str());
    setText(0, QString(name + " " + lastName));
}

SessionTreeItem::SessionTreeItem()
{
    setToolTip(0, "");
    setToolTip(1, "Date");
    setToolTip(2, "Trials");
    setToolTip(3, "Description");
    setToolTip(4, "Notes");
    setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/session.png")));
}

void SessionTreeItem::setSession(const ShallowCopy::Session& session)
{
    this->session = session;

    setText(0, QString::fromUtf8(session.sessionName.c_str()));
    setText(1, QString::fromUtf8(session.sessionDate.c_str()).left(10));
    setText(3, QString::fromUtf8(session.sessionDescription.c_str()));
    setText(4, QString::fromUtf8(session.tags.c_str()));
}

TrialTreeItem::TrialTreeItem()
{
    setToolTip(0, "");
    setToolTip(1, "Files");
    setToolTip(2, "Description");
    setToolTip(3, "Notes");
    setIcon(0, QIcon(QString::fromUtf8("data/resources/icons/trial.png")));
}

void TrialTreeItem::setTrial(const ShallowCopy::Trial& trial, std::vector<core::IDataManager::LocalTrial>& localTrials)
{
    this->trial = trial;

    setText(0, QString::fromUtf8(trial.trialName.c_str()));
    setText(1, QString::number(localTrials.size()));
    setText(2, QString::fromUtf8(trial.trialDescription.c_str()));

    BOOST_FOREACH(core::IDataManager::LocalTrial lTrial, localTrials)
    {
        boost::cmatch matches;
        boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
        //sprawdzamy, czy zgadza sie nazwa folderu
        if(lTrial.size() > 0 && boost::regex_match(lTrial[0].string().c_str(), matches, e)) {
            if(!trial.trialName.compare(matches[2])) {
                setTextColor(0, QColor(0, 0, 255));
            }
        }
    }
}

const QString TrialTreeItem::getName() const
{
    return core::toQString(trial.trialName);
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

    connect(dataWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));
}

void CommunicationWidgetEx::treeItemClicked(QTreeWidgetItem* item, int column)
{
    int columns = item->columnCount();
    dataWidget->setColumnCount(columns);
    QStringList header;
    for(int i = 0; i < columns; i++) {
        header << item->toolTip(i);
    }
    dataWidget->setHeaderLabels(header);
    if(item->toolTip(1) == "Files") {
        if(item->textColor(0) == QColor(0, 0, 255)) {
            isLocalTrialSelected = true;
            isServerTrialSelected = false;
        } else {
            isLocalTrialSelected = false;
            isServerTrialSelected = true;
        }
    } else {
        isLocalTrialSelected = false;
        isServerTrialSelected = false;
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
    menu->addMenu(view);
    menu->addSeparator();
    menu->addAction(actionUpdate);

    menu->addSeparator();

    menu->addAction(actionDownload);
    menu->addAction(actionLoad_trial);
    menu->addSeparator();
    menu->addAction(actionAbort_download);

    if(isLocalTrialSelected) {
        actionLoad_trial->setVisible(true);
        actionAbort_download->setVisible(false);
        actionDownload->setVisible(false);
    } else {
        actionLoad_trial->setVisible(false);
    }
    if(isServerTrialSelected) {
        actionLoad_trial->setVisible(false);
        actionAbort_download->setVisible(false);
        actionDownload->setVisible(true);
        if(isOnline) {
            actionDownload->setDisabled(false);
        } else {
            actionDownload->setDisabled(true);
        }
    } else {
        actionDownload->setVisible(false);
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
    menu->addMenu(view);
    menu->addAction(actionUpdate);
    menu->addSeparator();
    menu->addAction(actionAbort_download);

    menu->exec(mapToGlobal(p));
}

void CommunicationWidgetEx::performerViewPressed(bool tog)
{
    //TODO: na razie listuje, ale chyba lepiej stworzyc jakies relacje wewnetrzne miedzy obiektami?
    dataWidget->clear();
    //listowanie performerow
    BOOST_FOREACH(ShallowCopy::Performer performer, shallowCopy.performers)
    {
        PerformerTreeItem* item = new PerformerTreeItem();
        
        item->setPerformer(performer);
        dataWidget->addTopLevelItem(item);
        //sesje polaczone z performerami
        BOOST_FOREACH(ShallowCopy::PerformerConf performerC, shallowCopy.performerConfs)
        {
            BOOST_FOREACH(ShallowCopy::Session session, shallowCopy.sessions)
            {
                if(performerC.performerID == performer.performerID && performerC.sessionID == session.sessionID) {
                    SessionTreeItem* itemS = new SessionTreeItem();
                    itemS->setSession(session);
                    item->addChild(itemS);
                    //kojarzenie sesji z trialami
                    BOOST_FOREACH(ShallowCopy::Trial trial, shallowCopy.trials)
                    {
                        if(trial.sessionID == session.sessionID) {
                            TrialTreeItem* itemT = new TrialTreeItem();
                            itemT->setTrial(trial, localTrials);
                            itemS->addChild(itemT);
                        }
                    }
                    itemS->setText(2, QString::number(itemS->childCount()));
                }
            }
        }
        item->setText(1, QString::number(item->childCount()));
        treeItemClicked(item, 0);
    }
}

void CommunicationWidgetEx::sessionViewPressed(bool tog)
{
    dataWidget->clear();
    //listowanie sesji
    BOOST_FOREACH(ShallowCopy::Session session, shallowCopy.sessions)
    {
        SessionTreeItem* item = new SessionTreeItem();
        item->setSession(session);
        dataWidget->addTopLevelItem(item);
        //kojarzenie sesji z trialami
        BOOST_FOREACH(ShallowCopy::Trial trial, shallowCopy.trials)
        {
            if(trial.sessionID == session.sessionID) {
                TrialTreeItem* itemT = new TrialTreeItem();
                itemT->setTrial(trial, localTrials);
                item->addChild(itemT);
            }
        }
        item->setText(2, QString::number(item->childCount()));
        treeItemClicked(item, 0);
    }
}

void CommunicationWidgetEx::trialViewPressed(bool tog)
{
    dataWidget->clear();
    //listowanie triali
    BOOST_FOREACH(ShallowCopy::Trial trial, shallowCopy.trials)
    {
        TrialTreeItem* item = new TrialTreeItem();
        item->setTrial(trial, localTrials);
        dataWidget->addTopLevelItem(item);

        treeItemClicked(item, 0);
    }
}

void CommunicationWidgetEx::updatePressed()
{
    communicationService->copyDbData();
}

void CommunicationWidgetEx::downloadPressed()
{
    downloadTrial(reinterpret_cast<TrialTreeItem*>(dataWidget->currentItem())->getEntityID());
}

void CommunicationWidgetEx::loadPressed()
{
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
    shallowCopy = subject->getShalloCopy();
    metaData = subject->getMetadata();
    localTrials = subject->getLocalTrials();
    infoLabel->setText("");

    refreshUI();
}

void CommunicationWidgetEx::refreshUI()
{
    dataWidget->clear();
    dataWidget->setSortingEnabled(true);
    dataWidget->setDisabled(false);
    progressBar->reset();

    actionPerformer_View->setChecked(false);
    actionPerformer_View->setChecked(true);
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
