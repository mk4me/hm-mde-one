#include "CommunicationPCH.h"
#include "CommunicationWidget.h"
#include <core/StringTools.h>
#include <boost/regex.hpp>
//http://hmdb.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl
//http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl

using namespace communication;

CommunicationWidget::CommunicationWidget(CommunicationService* service)
    : QWidget(), communicationService(service)
{
    updateButton = new QPushButton(tr("Get trials"));
    downloadButton = new QPushButton(tr("Download"));
    abortButton = new QPushButton(tr("Abort"));
    abortButton->setDisabled(true);

    QDialogButtonBox* buttonsBox = new QDialogButtonBox(Qt::Horizontal);	
    buttonsBox->addButton(updateButton, QDialogButtonBox::ActionRole);
    buttonsBox->addButton(downloadButton, QDialogButtonBox::ActionRole);
    buttonsBox->addButton(abortButton, QDialogButtonBox::ActionRole);

    infoLabel = new QLabel("");
    aviableTrialsLabel = new QLabel("Aviable trials:");
    serverStateLabel = new QLabel();
    trials = new QListWidget(this);
    progressBar = new QProgressBar(this);

    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->addWidget(serverStateLabel);
    hbox->addWidget(infoLabel);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(aviableTrialsLabel);
    mainLayout->addWidget(trials);
    mainLayout->addWidget(buttonsBox);
    mainLayout->addWidget(progressBar);
    mainLayout->addLayout(hbox);
    setLayout(mainLayout);

    connect(updateButton, SIGNAL(clicked()), this, SLOT(updateButtonClicked()));
    connect(downloadButton, SIGNAL(clicked()), this, SLOT(downloadButtonClicked()));
    connect(abortButton, SIGNAL(clicked()), this, SLOT(abortButtonClicked()));
    connect(trials, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));
    connect(trials, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
}

CommunicationWidget::~CommunicationWidget()
{
}

void CommunicationWidget::update(const CommunicationManager* subject)
{
    //serverTrials = subject->getServerTrials();
    localTrials = subject->getLocalTrials();
    isOnline = subject->isServerResponse();

    //if(subject->isUpdated()) {
    //    infoText = "Last trial update:";
    //    infoText.append(subject->getLastUpdateTime().toString());
    //} else {
    //    infoText = "Cannot find date of last trial update";
    //}
    refreshUI();
}

void CommunicationWidget::refreshUI()
{
    updateButton->setDisabled(false);
    downloadButton->setDisabled(false);
    trials->clear();
    trials->setSortingEnabled(true);
    trials->setDisabled(false);
    abortButton->setDisabled(true);
    progressBar->reset();

    infoLabel->setText(core::toQString(infoText));

    //lokalne proby pomiarowe
    BOOST_FOREACH(core::IDataManager::LocalTrial& trial, localTrials)
    {
        LocalTrialItem* item = new LocalTrialItem();
        trials->addItem(item);
        item->localTrial = trial;

        boost::cmatch matches;
        boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
        //sprawdzamy, czy zgadza sie nazwa folderu
        if(trial.size() > 0 && boost::regex_match(trial[0].string().c_str(), matches, e)) {
            item->setText(core::toQString(matches[2]));
        } else {
            item->setText(core::toQString("Unknown trial"));
        }
    }
    //serwerowe proby pomiarowe, sprawdzamy czy serwer jest online i jesli tak to czy sie nie powtarzaja z lokalnymi
    if(isOnline) {
        serverStateLabel->setText("Online");
        serverStateLabel->setStyleSheet("QLabel {font: bold; color : green;}");
        bool isLocal;
        BOOST_FOREACH(communication::Trial& trial, serverTrials) {
            isLocal = false;
            BOOST_FOREACH(core::IDataManager::LocalTrial& local, localTrials) {
                boost::cmatch matches;
                boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
                //sprawdzamy, czy zgadza sie nazwa folderu
                if(local.size() > 0 && boost::regex_match(local[0].string().c_str(), matches, e) && trial.trialDescription.compare(matches[2]) == 0) {
                    isLocal = true;
                    break;
                }
            }
            if(!isLocal) {
                EntityTrialItem* item = new EntityTrialItem();
                trials->addItem(item);
                item->setText(core::toQString(trial.trialDescription));
                item->session = trial.sessionID;
                item->id = trial.id;
            }
        }
    } else {
        serverStateLabel->setText("Offline");
        serverStateLabel->setStyleSheet("QLabel {font: bold; color : red;}");
    }
}

void CommunicationWidget::showErrorMessage(const std::string& error)
{
    QMessageBox::warning(this, "Error", core::toQString(error));
}

void CommunicationWidget::setProgress(int value)
{
    if(value > 99) {
        progressBar->reset();
    } else {
        progressBar->setValue(value);
    }
}

void CommunicationWidget::itemDoubleClicked(QListWidgetItem* item)
{
    if(item && item->textColor() == QColor(0, 0, 255)) {
        LocalTrialItem* temp = reinterpret_cast<LocalTrialItem*>(trials->item(trials->currentRow()));
        loadTrial(temp->localTrial);
    } else {
        download();
    }
}

void CommunicationWidget::updateButtonClicked()
{
    updateTrials();
}

void CommunicationWidget::abortButtonClicked()
{
    abort();
}

void CommunicationWidget::downloadButtonClicked()
{
    itemDoubleClicked(trials->item(trials->currentRow()));
}

void CommunicationWidget::itemClicked(QListWidgetItem* item)
{
    if(trials->item(trials->currentRow())->textColor() == QColor(0, 0, 255)) {
        downloadButton->setText("Load");
    } else {
        downloadButton->setText("Download");
    }
}

void CommunicationWidget::download()
{
    if(trials->item(trials->currentRow())) {
        EntityTrialItem* temp = reinterpret_cast<EntityTrialItem*>(trials->item(trials->currentRow()));
        int trial = temp->id;
        downloadButton->setText("Load");
        abortButton->setDisabled(false);
        trials->clear();
        //lokalne proby pomiarowe
        BOOST_FOREACH(core::IDataManager::LocalTrial& trial, localTrials) {
            LocalTrialItem* item = new LocalTrialItem();
            trials->addItem(item);

            item->localTrial = trial;

            boost::cmatch matches;
            boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
            //sprawdzamy, czy zgadza sie nazwa folderu
            if(trial.size() > 0 && boost::regex_match(trial[0].string().c_str(), matches, e)) {
                item->setText(core::toQString(matches[2]));
            } else {
                item->setText(core::toQString("Unknown trial"));
            }
        }
        updateButton->setDisabled(true);
        this->communicationService->downloadTrial(trial);
    }
}

void CommunicationWidget::abort()
{
    downloadButton->setText("Download");
    abortButton->setDisabled(true);
    communicationService->cancelDownloading();
}

void CommunicationWidget::loadTrial(const core::IDataManager::LocalTrial& localTrial)
{
    communicationService->loadTrial(localTrial);
}

void CommunicationWidget::updateTrials()
{
    updateButton->setDisabled(true);
    downloadButton->setText("Load");
    trials->clear();
    //lokalne proby pomiarowe
    BOOST_FOREACH(core::IDataManager::LocalTrial& trial, localTrials) {
        LocalTrialItem* item = new LocalTrialItem();
        trials->addItem(item);

        item->localTrial = trial;

        boost::cmatch matches;
        boost::regex e("(.*)(\\d{4}-\\d{2}-\\d{2}-P\\d{2,}-S\\d{2,}-T\\d{2,})(.*)");
        //sprawdzamy, czy zgadza sie nazwa folderu
        if(trial.size() > 0 && boost::regex_match(trial[0].string().c_str(), matches, e)) {
            item->setText(core::toQString(matches[2]));
        } else {
            item->setText(core::toQString("Unknown trial"));
        }
    }
    communicationService->updateSessionContents();
}

void CommunicationWidget::setInfoLabel( const std::string& info )
{
    infoText=info;
    infoLabel->setText(core::toQString(infoText));
}