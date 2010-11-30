#include "CommunicationPCH.h"
#include "CommunicationWidget.h"
//http://hmdb.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl
//http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl

CommunicationWidget::CommunicationWidget(CommunicationService* service) : QWidget(), communicationService(service)
{
	updateButton = new QPushButton(tr("Update data"));

	QDialogButtonBox* buttonsBox = new QDialogButtonBox(Qt::Horizontal);	
	buttonsBox->addButton(updateButton, QDialogButtonBox::ActionRole);
	
	treeEntities = new QTreeWidget(this);
	treeEntities->setSelectionMode(QTreeView::SingleSelection);
	treeEntities->setHeaderLabels(QStringList() << tr("Entity") << tr("ID") << tr("Description"));

	progressBar = new QProgressBar(this);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(buttonsBox);
	mainLayout->addWidget(treeEntities);
	mainLayout->addWidget(progressBar);
	setLayout(mainLayout);

	connect(updateButton, SIGNAL(clicked()), this, SLOT(updateButtonClicked()));
	connect(treeEntities, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem*, int)));
}

CommunicationWidget::~CommunicationWidget()
{
}

void CommunicationWidget::itemDoubleClicked(QTreeWidgetItem* item, int column)
{
	if(!busy)
	{
		if(item && item->text(0) == tr("Trial"))
		{
			int session = item->parent()->text(1).toInt();
			int trial = item->text(1).toInt();
			this->communicationService->downloadTrial(session, trial);
		}
	}
}

void CommunicationWidget::updateButtonClicked()
{
	try
	{
		this->communicationService->updateSessionContents();
		this->communicationService->save();
	}
	catch(std::runtime_error& e)
	{
		QMessageBox::critical(this, tr("Error"), tr(e.what()));
	}
}

void CommunicationWidget::update(const communication::CommunicationManager* subject)
{
	treeEntities->clear();
	std::map<int, communication::Session> sessions = subject->getSessions();
	for(std::map<int, communication::Session>::const_iterator i = sessions.begin(); i != sessions.end(); ++i)
	{
		//map sessions
		QTreeWidgetItem* s = new QTreeWidgetItem(treeEntities);
		s->setText(0, tr("Session"));
		s->setText(1, tr("%1").arg((*i).first));
		s->setText(2, QString::fromUtf8((*i).second.sessionDescription.c_str()));
		s->setToolTip(0, QString::fromUtf8((*i).second.sessionDescription.c_str()));
		s->setToolTip(1, QString::fromUtf8((*i).second.sessionDescription.c_str()));
		s->setToolTip(2, QString::fromUtf8((*i).second.sessionDescription.c_str()));
		for(std::map<int, communication::Trial>::const_iterator j = (*i).second.sessionTrials.begin(); j != (*i).second.sessionTrials.end(); ++j)
		{
			//map trials
			QTreeWidgetItem* t = new QTreeWidgetItem(s);
			t->setText(0, tr("Trial"));
			t->setText(1, tr("%1").arg((*j).first));
			t->setText(2, QString::fromUtf8((*j).second.trialDescription.c_str()));
			t->setToolTip(0, QString::fromUtf8((*j).second.trialDescription.c_str()));
			t->setToolTip(1, QString::fromUtf8((*j).second.trialDescription.c_str()));
			t->setToolTip(2, QString::fromUtf8((*j).second.trialDescription.c_str()));
			for(std::map<int, communication::File>::const_iterator k = (*j).second.trialFiles.begin(); k != (*j).second.trialFiles.end(); ++k)
			{
				//map files
				QTreeWidgetItem* f = new QTreeWidgetItem(t);
				f->setText(0, tr("File"));
				f->setText(1, tr("%1").arg((*k).first));
				f->setText(2, QString::fromUtf8((*k).second.fileName.c_str()));
				f->setToolTip(0, QString::fromUtf8((*k).second.fileName.c_str()));
				f->setToolTip(1, QString::fromUtf8((*k).second.fileName.c_str()));
				f->setToolTip(2, QString::fromUtf8((*k).second.fileName.c_str()));
			}
		}
	}
}

void CommunicationWidget::setBusy(bool busy)
{
	this->busy = busy;
	if(busy)
	{
		treeEntities->setDisabled(true);
	}
	else
	{
		treeEntities->setDisabled(false);
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
