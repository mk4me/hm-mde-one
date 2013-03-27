#include "SummaryWindow.h"
#include "TreeItemHelper.h"
#include "HmmMainWindow.h"
#include "AnalisisWidget.h"
#include "TreeItemHelper.h"
#include <plugins/newCommunication/IPatient.h>

void SummaryWindow::initialize()
{
    QWidget* parent = hmm->getAnalisis()->SummaryWindowWidget;
    parent->setLayout(new QVBoxLayout());
    parent->layout()->setContentsMargins(0, 0, 0, 0);
    parent->layout()->setMargin(0);
    tree = new QTreeWidget();
    tree->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    tree->setHeaderHidden(true);
    parent->layout()->addWidget(tree);
}

void SummaryWindow::display( const std::vector<TreeItemHelperPtr>& helpers )
{
    UTILS_ASSERT(tree != nullptr);

    tree->clear();
    auto size = helpers.size();
    if (size > 0) {
        QTreeWidgetItem* root = new QTreeWidgetItem();
        root->setText(0, QObject::tr("Data Info"));
        QFont font = root->font(0);
        font.setBold(true);
        font.setPointSize(12);
        root->setFont(0, font);
        tree->addTopLevelItem(root);

        for (auto itHelper = helpers.begin(); itHelper != helpers.end(); ++itHelper) {
            
            NewMultiserieHelperPtr multiserieHelper = utils::dynamic_pointer_cast<NewMultiserieHelper>(*itHelper);
            if (multiserieHelper) {
                const auto& desc = multiserieHelper->getChartWithDescriptions();
                for (auto it = desc.begin(); it != desc.end(); ++it) {
                    auto motion = it->motion;
                    if (motion) {
						std::string name;
						it->wrapper->tryGetMeta("core/name", name);
                        QString text = QString::fromStdString(name) + createDescription(it->motion);
                        addItem(text, root);
                    }
                }
            } else {
                auto motion = (*itHelper)->getMotion();
                QString text = motion ? createDescription(motion) : QString();
                text = (*itHelper)->getText() + "\n" + text;
                //TreeWrappedItemHelper* wrapped = dynamic_cast<TreeWrappedItemHelper*>(*itHelper);
                //if (wrapped) {
                //    text = QString::fromStdString(wrapped->getWrapper()->getName()) + "\n" + text;
                //} 
                addItem(text, root);
            }
        }

        root->setExpanded(true);
    }
}

QString SummaryWindow::createDescription( PluginSubject::MotionConstPtr motion) const
{
    QString text;
    PluginSubject::SessionConstPtr session = motion->getUnpackedSession();
    PluginSubject::SubjectConstPtr subject = session->getUnpackedSubject();
    
    text += QObject::tr("Motion: ") + QString::fromStdString(motion->getLocalName()) + "\n";
    text += QObject::tr("Session: ") + QString::fromStdString(session->getLocalName()) + "\n";
    text += QObject::tr("Subject: ") + QString::fromStdString(subject->getName()) + "\n";
   
    std::vector<core::ObjectWrapperConstPtr> metadata;      
    try {

		std::string groupName, groupID;

		if(motion->getSession()->tryGetMeta("groupName", groupName) == true) {
			text += QObject::tr("Owner: %1").arg(groupName.c_str()); 
			if (motion->getSession()->tryGetMeta("groupID", groupID) == true) {
				text += QString("(%1)").arg(groupID.c_str());
			}
			text += "\n";
		}

		std::string date;
		if(motion->getSession()->tryGetMeta("data", date) == true) {
			text += QObject::tr("Date: %1\n").arg(date.c_str());
		}
    } catch (...) {
        PLUGIN_LOG_WARNING("Problem with summary window metadata");
    }
    
    if (session->hasObject(typeid(communication::IPatient), false)) {
		core::ConstObjectsList patients;
		session->getObjects(patients, typeid(communication::IPatient), false);
        communication::PatientConstPtr patient = patients.front()->get();
        text += QObject::tr("Patient: ") + QString::fromStdString(patient->getName()) + " " + QString::fromStdString(patient->getSurname()) + "\n";
    }

    if (session->hasObject(typeid(communication::AntropometricData), false)) {
		core::ConstObjectsList antropo;
		session->getObjects(antropo, typeid(communication::AntropometricData), false);
        utils::shared_ptr<const communication::AntropometricData> antro = antropo.front()->get();
        text += " ";
        text += QObject::tr("Weight: ") + QString("%1 ").arg(antro->bodyMass.first) + QString::fromStdString(antro->bodyMass.second) + "\n";
    }
    return text;
}

void SummaryWindow::addItem( QString text, QTreeWidgetItem* root )
{
    QTreeWidgetItem* itm = new QTreeWidgetItem();
    itm->setText(0, text);
    root->addChild(itm);
}


SummaryWindowController::SummaryWindowController( SummaryWindowPtr sw, HmmMainWindow* hmm  ) :
    hmm(hmm),
	summary(sw)
{

}

void SummaryWindowController::onTreeItemSelected( QTreeWidgetItem* item, int column )
{
    SummaryWindow::HelpersCollection collection;
    HmmTreeItem* hmmItem = dynamic_cast<HmmTreeItem*>(item);
    if (hmmItem) {
        collection.push_back(hmmItem->getHelper());
    }
    summary->display(collection);
}

void SummaryWindowController::onVisualizator( coreUI::CoreVisualizerWidget* visualizatorWidget )
{
    core::VisualizerPtr visualizer = visualizatorWidget->getVisualizer();
    
    SummaryWindow::HelpersCollection collection;
    for (auto it = hmm->items2Descriptions.begin(); it != hmm->items2Descriptions.end(); ++it) {
    
        if (it->second.visualizerWidget == visualizatorWidget) {
            collection.push_back(it->first);
        }
    }
    summary->display(collection);
}
