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

QTreeWidgetItem* BuilderFilterCommand::createTreeBranch( const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions ) 
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
    root->setIcon(0, rootIcon);
    BOOST_FOREACH(PluginSubject::SessionConstPtr session, sessions) {
        PluginSubject::Motions motions;
        session->getMotions(motions);
        BOOST_FOREACH(PluginSubject::MotionConstPtr motion, motions) {
            root->addChild(branchFunction(motion, QString(motion->getLocalName().c_str()), rootIcon, elementIcon));
        }
    }

    return root;
}

QTreeWidgetItem* JointsCommand::createTreeBranch( const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions )
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
    BOOST_FOREACH(PluginSubject::SessionConstPtr session, sessions) {
        PluginSubject::Motions motions;
        session->getMotions(motions);
        BOOST_FOREACH(PluginSubject::MotionConstPtr motion, motions) {
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

QTreeWidgetItem* EMGCommand::createTreeBranch( const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions ) 
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, rootItemName);
    root->setIcon(0, rootIcon);
    BOOST_FOREACH(PluginSubject::SessionConstPtr session, sessions) {

        MeasurementConfigConstPtr config = getMeta(session);
        PluginSubject::Motions motions;
        session->getMotions(motions);
        BOOST_FOREACH(PluginSubject::MotionConstPtr motion, motions) {
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

MeasurementConfigConstPtr EMGCommand::getMeta( PluginSubject::SessionConstPtr session ) 
{
    MeasurementConfigConstPtr config;
    //próbuje pobraæ metadane
    try{
        auto measurements = Measurements::get();
        std::vector<core::ObjectWrapperConstPtr> metadata;        
        core::IDataManagerReader::getMetadataForObject(DataManager::getInstance(), session, metadata);
        auto metaITEnd = metadata.end();
        for(auto metaIT = metadata.begin(); metaIT != metaITEnd; ++metaIT){
            core::MetadataConstPtr meta = (*metaIT)->get(false);
            std::string l;

            if(measurements != nullptr && meta != nullptr && meta->value("EMGConf", l) == true) {
                config = measurements->getConfig(("EMG_" + l).c_str());
            }
        }
    }catch(...){

    }	
    return config;
}