#include "CommunicationPCH.h"
#include "CommunicationWidget.h"
//http://hmdb.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl
//http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl

CommunicationWidget::CommunicationWidget(CommunicationService* service) : QWidget(), m_service(service)
{
	m_update_button = new QPushButton(tr("Update data"));

	QDialogButtonBox* buttonsBox = new QDialogButtonBox(Qt::Horizontal);	
	buttonsBox->addButton(m_update_button, QDialogButtonBox::ActionRole);
	
	m_tree = new QTreeWidget(this);
	m_tree->setSelectionMode(QTreeView::SingleSelection);
	//m_tree->setHeaderHidden(true);
	m_tree->setHeaderLabels(QStringList() << tr("Entity") << tr("ID"));

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(buttonsBox);
	mainLayout->addWidget(m_tree);
	setLayout(mainLayout);

	connect(m_update_button, SIGNAL(clicked()), this, SLOT(updateButtonClicked()));
	connect(m_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem*, int)));
}

CommunicationWidget::~CommunicationWidget()
{
}

void CommunicationWidget::itemDoubleClicked(QTreeWidgetItem* item, int column)
{
	if(item->type() == QTreeWidgetItem::UserType)
	{
		try
		{
			this->m_service->downloadFile(item->text(1).toInt());
		}
		catch(communication::EDRException& e)
		{
			QMessageBox::critical(this, tr("Error"), tr(e.getExceptionMessage().c_str()));
		}
	}
}

void CommunicationWidget::updateButtonClicked()
{
	try
	{
		this->m_service->updateSessionContents();
		this->m_service->saveToXml();
	}
	catch(communication::EDRException& e)
	{
		QMessageBox::critical(this, tr("Error"), tr(e.getExceptionMessage().c_str()));
	}
}

void CommunicationWidget::update(const communication::CommunicationManager* subject)
{
	m_tree->clear();
	std::map<int, communication::Session> sessions = subject->getSessions();
	for(std::map<int, communication::Session>::const_iterator i = sessions.begin(); i != sessions.end(); ++i)
	{
		//map sessions
		QTreeWidgetItem* s = new QTreeWidgetItem(m_tree);
		s->setText(0, tr("Session"));
		s->setText(1, QString("%1").arg((*i).first));
		for(std::map<int, communication::Trial>::const_iterator j = (*i).second.session_trials.begin(); j != (*i).second.session_trials.end(); ++j)
		{
			//map trials
			QTreeWidgetItem* t = new QTreeWidgetItem(s);
			t->setText(0, tr("Trial"));
			t->setText(1, QString("%1").arg((*j).first));
			for(std::map<int, communication::File>::const_iterator k = (*j).second.trial_files.begin(); k != (*j).second.trial_files.end(); ++k)
			{
				//map files
				QTreeWidgetItem* f = new QTreeWidgetItem(t, QTreeWidgetItem::UserType);
				f->setText(0, tr("File"));
				f->setText(1, QString("%1").arg((*k).first));
			}
		}
	}
}
