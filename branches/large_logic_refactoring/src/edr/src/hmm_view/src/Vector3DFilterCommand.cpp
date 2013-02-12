#include "hmmPCH.h"
#include "Vector3DFilterCommand.h"

__Helper::__Helper( boost::function<void (const QString&, int)> function ) :
function(function) 
{
}

void __Helper::checkBoxChanged( int state )
{
    QCheckBox* checkBox = qobject_cast<QCheckBox *>(sender());
    function(checkBox->text(), state);
}


void __Helper::onElementHovered( const QString& name, bool selected ) {
    ConfigurationWidget* widget = qobject_cast<ConfigurationWidget*>(sender());
    if (selected) {
        auto it = namesDictionary.find(name);
        if (it != namesDictionary.end()) {
            widget->setText(it->second.second);
        } else {
            // ?
            widget->setText("");
        }
    } else {
        widget->setText("");
    }
}

void __Helper::onItemSelected( const QString& name, bool selected )
{
    UTILS_ASSERT(namesDictionary.size() > 0);
    auto it = namesDictionary.find(name);
    if (it != namesDictionary.end()) {
        function(it->second.first, selected ? 1 : 0);
    } else {
        // ?
    }
}

BuilderFilterCommand::BuilderFilterCommand( BranchFunction function, const QIcon& rootIcon /*= QIcon()*/, const QIcon& elementIcon /*= QIcon()*/ ) : 
branchFunction(function),
    elementIcon(elementIcon),
    rootIcon(rootIcon)
{ }

QTreeWidgetItem* BuilderFilterCommand::createTreeBranch( const QString& rootItemName, const core::ObjectWrapperCollection& sessions ) 
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
    root->setIcon(0, rootIcon);
    BOOST_FOREACH(core::ObjectWrapperConstPtr sessionOW, sessions) {
		PluginSubject::SessionConstPtr session = sessionOW->get();
		core::ConstObjectsList motions;
        session->getMotions(motions);
        BOOST_FOREACH(core::ObjectWrapperConstPtr motionOW, motions) {
			PluginSubject::MotionConstPtr motion = motionOW->get();
            root->addChild(branchFunction(motion, QString(motion->getLocalName().c_str()), rootIcon, elementIcon));
        }
    }

    return root;
}

QTreeWidgetItem* JointsCommand::createTreeBranch( const QString& rootItemName, const core::ObjectWrapperCollection& sessions )
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
	BOOST_FOREACH(core::ObjectWrapperConstPtr sessionOW, sessions) {
		PluginSubject::SessionConstPtr session = sessionOW->get();
		core::ConstObjectsList motions;
		session->getMotions(motions);
		BOOST_FOREACH(core::ObjectWrapperConstPtr motionOW, motions) {
			PluginSubject::MotionConstPtr motion = motionOW->get();
			root->addChild(TreeBuilder::createJointsBranch(motion, motion->getLocalName().c_str(), TreeBuilder::getRootJointsIcon(), TreeBuilder::getJointsIcon()));
		}
	}

    return root;
}


EMGCommand::EMGCommand( BranchFunction function, const NamesDictionary& namesDictionary, 
    const QString& frontXml, const QString& backXml, const QIcon& rootIcon /*= QIcon()*/, const QIcon& elementIcon /*= QIcon()*/ ) :
BuilderConfiguredFilterCommand(function, namesDictionary, frontXml, backXml, rootIcon, elementIcon) 
{
}

QTreeWidgetItem* EMGCommand::createTreeBranch( const QString& rootItemName, const core::ObjectWrapperCollection& sessions ) 
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
    root->setIcon(0, rootIcon);

	BOOST_FOREACH(core::ObjectWrapperConstPtr sessionOW, sessions) {
		MeasurementConfigConstPtr config = getSessionMeta(sessionOW);
		PluginSubject::SessionConstPtr session = sessionOW->get();
		core::ConstObjectsList motions;
		session->getMotions(motions);
		BOOST_FOREACH(core::ObjectWrapperConstPtr motionOW, motions) {
			PluginSubject::MotionConstPtr motion = motionOW->get();
			QTreeWidgetItem* colItem = branchFunction(motion, QString(motion->getLocalName().c_str()), rootIcon, elementIcon);
			if (config) {

				for (int i = colItem->childCount() - 1; i >= 0; --i) {
					QTreeWidgetItem* item = colItem->child(i);
					QString name = item->text(0);
					auto entry = activeElements.find(config->getIdByName(name).toStdString());
					if (entry != activeElements.end() && !entry->second) {
						delete item;
					}
				}
			}
			root->addChild(colItem);
		}
	}

    return root;
}

MeasurementConfigConstPtr EMGCommand::getSessionMeta( const core::ObjectWrapperConstPtr & session ) 
{
    MeasurementConfigConstPtr config;
    //próbuje pobrać metadane
    try{
        auto measurements = Measurements::get();
		std::string l;
        if(measurements != nullptr && session->tryGetMeta("EMGConf", l) == true) {
            config = measurements->getConfig(("EMG_" + l).c_str());
        }
    }catch(...){

    }	
    return config;
}
