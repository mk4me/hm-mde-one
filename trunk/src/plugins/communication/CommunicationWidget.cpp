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
	BOOST_FOREACH(LocalTrial& trial, localTrials)
	{
		LocalTrialItem* item = new LocalTrialItem();
		trials->addItem(item);
		item->setPath(trial.getTrialPath());
		item->setName(trial.getName());
		item->setText(QString::fromStdString(trial.getName()));
	}
	//serwerowe proby pomiarowe, sprawdzamy czy sie nie powtarzaja z lokalnymi
	bool isLocal;
	BOOST_FOREACH(communication::Trial& trial, serverTrials)
	{
		isLocal = false;
		BOOST_FOREACH(LocalTrial& local, localTrials)
		{
			if(trial.trialDescription.compare(local.getName()) == 0)
			{
				isLocal = true;
				break;
			}
		}
		if(!isLocal)
		{
			EntityTrialItem* item = new EntityTrialItem();
			trials->addItem(item);
			item->setText(QString::fromStdString(trial.trialDescription));
			item->setSession(trial.sessionID);
			item->setID(trial.id);
		}
	}
}

void CommunicationWidget::showErrorMessage(const std::string& error)
{
	QMessageBox::warning(this, "Error", QString::fromStdString(error));
}

void CommunicationWidget::setBusy(bool busy)
{
	this->busy = busy;
	if(busy)
	{
		trials->setDisabled(true);
		//downloadButton->setDisabled(true);
		updateButton->setDisabled(true);
		downloadButton->setText("Cancel");
	}
	else
	{
		trials->setDisabled(false);
		updateButton->setDisabled(false);
		progressBar->reset();
		downloadButton->setText("Download");
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

void CommunicationWidget::itemDoubleClicked(QListWidgetItem* item)
{
	if(item->textColor() == QColor(0, 0, 255))
	{
		LocalTrialItem* temp = reinterpret_cast<LocalTrialItem*>(trials->item(trials->currentRow()));
		loadTrial(temp->getName());
	}
	else
	{
		download();
	}
}

void CommunicationWidget::updateButtonClicked()
{
	if(!busy)
	{
		updateTrials();
	}
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
		this->communicationService->downloadTrial(trial);
	}
}

void CommunicationWidget::abort()
{
	communicationService->cancelDownloading();
}

void CommunicationWidget::loadTrial(const std::string& name)
{
	communicationService->loadTrial(name);
}

void CommunicationWidget::updateTrials()
{
	communicationService->updateSessionContents();
}
