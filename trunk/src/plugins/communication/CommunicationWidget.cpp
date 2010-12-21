#include "CommunicationPCH.h"
#include "CommunicationWidget.h"
//http://hmdb.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl
//http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl

typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;

CommunicationWidget::CommunicationWidget(CommunicationService* service) : QWidget(), communicationService(service), trialsMutex()
{
	updateButton = new QPushButton(tr("Get trials"));
	downloadButton = new QPushButton(tr("Download"));
	downloadButton->setDisabled(true);

	QDialogButtonBox* buttonsBox = new QDialogButtonBox(Qt::Horizontal);	
	buttonsBox->addButton(updateButton, QDialogButtonBox::ActionRole);
	buttonsBox->addButton(downloadButton, QDialogButtonBox::ActionRole);

	infoLabel = new QLabel("");
	aviableTrialsLabel = new QLabel("Aviable trials:");

	trials = new QListWidget(this);

	progressBar = new QProgressBar(this);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(aviableTrialsLabel);
	mainLayout->addWidget(trials);
	mainLayout->addWidget(buttonsBox);
	mainLayout->addWidget(progressBar);
	mainLayout->addWidget(infoLabel);
	setLayout(mainLayout);
	updateView = false;

	connect(updateButton, SIGNAL(clicked()), this, SLOT(updateButtonClicked()));
	connect(downloadButton, SIGNAL(clicked()), this, SLOT(downloadButtonClicked()));
	connect(trials, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));
	connect(trials, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
}

CommunicationWidget::~CommunicationWidget()
{
}

void CommunicationWidget::update(const communication::CommunicationManager* subject)
{
	ScopedLock lock(trialsMutex);

	serverTrials = subject->getServerTrials();
	localTrials = subject->getLocalTrials();

	if(subject->isUpdated())
	{
		infoText = "Last trial update:";
		infoText.append(subject->getLastUpdateTime().toString());
	}
	else
	{
		infoText = "Cannot find date of last trial update";
	}
	updateView = true;
}

void CommunicationWidget::refresh()
{
	ScopedLock lock(trialsMutex);
	if(updateView)
	{
		refreshUI();
		updateView = false;
	}
}

void CommunicationWidget::refreshUI()
{
	trials->clear();
	trials->setSortingEnabled(true);

	infoLabel->setText(QString::fromStdString(infoText));

	//lokalne proby pomiarowe
	for(std::vector<LocalTrial>::const_iterator it = localTrials.begin(); it != localTrials.end(); ++it)
	{
		LocalTrialItem* item = new LocalTrialItem();
		trials->addItem(item);
		item->setPath(it->getTrialPath());
		item->setName(it->getName());
		item->setText(QString::fromStdString(it->getName()));
	}
	//serwerowe proby pomiarowe, sprawdzamy czy sie nie powtarzaja z lokalnymi
	bool isLocal;
	for(std::vector<communication::Trial>::const_iterator it = serverTrials.begin(); it != serverTrials.end(); ++it)
	{
		isLocal = false;
		for(std::vector<LocalTrial>::const_iterator it2 = localTrials.begin(); it2 != localTrials.end(); ++it2)
		{
			if(it->trialDescription.compare(it2->getName()) == 0)
			{
				isLocal = true;
				break;
			}
		}
		if(!isLocal)
		{
			EntityTrialItem* item = new EntityTrialItem();
			trials->addItem(item);
			item->setText(QString::fromStdString((*it).trialDescription));
			item->setSession((*it).sessionID);
			item->setID((*it).id);
		}
	}
}

void CommunicationWidget::setBusy(bool busy)
{
	this->busy = busy;
	if(busy)
	{
		trials->setDisabled(true);
		//downloadButton->setDisabled(true);
		updateButton->setDisabled(true);
	}
	else
	{
		trials->setDisabled(false);
		updateButton->setDisabled(false);
		progressBar->reset();
	}
}

bool CommunicationWidget::getBusy()
{
	return busy;
}

void CommunicationWidget::setProgress(int value)
{
	if(value > 99)
	{
		progressBar->reset();
	}
	else
	{
		progressBar->setValue(value);
	}
}

void CommunicationWidget::showErrorMessage(const std::string& error)
{
	QMessageBox::warning(this, "Error", QString::fromStdString(error));
}

void CommunicationWidget::itemDoubleClicked(QListWidgetItem* item)
{
	if(item->textColor() == QColor(0, 0, 255))
	{
		LocalTrialItem* temp = reinterpret_cast<LocalTrialItem*>(trials->item(trials->currentRow()));
		communicationService->loadTrial(temp->getName());
	}
}

void CommunicationWidget::updateButtonClicked()
{
	//infoLabel->setText(QString("Updating..."));
	this->communicationService->updateSessionContents();
}

void CommunicationWidget::downloadButtonClicked()
{
	if(!busy)
	{
		download();
	}
	else
	{
		abort();
	}
}

void CommunicationWidget::itemClicked(QListWidgetItem* item)
{
	if(item->textColor() == QColor(0, 0, 255))
	{
		downloadButton->setEnabled(false);
	}
	else
	{
		downloadButton->setEnabled(true);
	}
}

void CommunicationWidget::download()
{
	if(trials->item(trials->currentRow()))
	{
		EntityTrialItem* temp = reinterpret_cast<EntityTrialItem*>(trials->item(trials->currentRow()));
		int trial = temp->getID();
		downloadButton->setText("Cancel");
		this->communicationService->downloadTrial(trial);
	}
}

void CommunicationWidget::abort()
{
	communicationService->cancelDownloading();
	downloadButton->setText("Download");
}
