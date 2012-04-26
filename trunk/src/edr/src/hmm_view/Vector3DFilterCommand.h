/********************************************************************
	created:	2011/10/14
	created:	14:10:2011   8:03
	filename: 	Vector3DFilterCommand.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_VECTOR3DFILTERCOMMAND__VECTOR3DFILTERCOMMAND_H__
#define HEADER_GUARD_VECTOR3DFILTERCOMMAND__VECTOR3DFILTERCOMMAND_H__

#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/type_traits.hpp>

#include <utils/DataChannel.h>
#include <utils/DataChannelCollection.h>
#include <core/SmartPtr.h>

#include <plugins/subject/ISubjectService.h>

#include "FilterCommand.h"
#include "TreeItemHelper.h"
#include "configurationDialog.h"
#include "Measurements.h"

typedef std::map<QString, std::pair<QString,QString> > NamesDictionary;

//! klasa, dzieki ktorej mozliwe jest korzystanie z sygnalow i slotow w klasie szablonowej.
//! obsluguje sloty w klasach Vector3DFilterCommand i Vector3DFilterCommand2
class __Helper : public QObject
{
    Q_OBJECT;
public:
    __Helper(boost::function<void (const QString&, int)> function) :
      function(function) 
      {
      }
public:
    
public slots:
    void checkBoxChanged ( int state )
    {
        QCheckBox* checkBox = qobject_cast<QCheckBox *>(sender());
        function(checkBox->text(), state);
    }

    void onItemSelected(const QString& name, bool selected)
    {
        UTILS_ASSERT(namesDictionary.size() > 0);
        auto it = namesDictionary.find(name);
        if (it != namesDictionary.end()) {
            function(it->second.first, selected ? 1 : 0);
        } else {
            // ?
        }
    }

    void onElementHovered(const QString& name, bool selected) {
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

public:
    void setNamesDictionary(const NamesDictionary& val) { namesDictionary = val; }
    const NamesDictionary& getNamesDictionary() const { return namesDictionary; }

private:
    boost::function<void (const QString&, int)> function;
    NamesDictionary namesDictionary;
};


class BuilderFilterCommand : public IFilterCommand
{
public:
    typedef boost::function<QTreeWidgetItem* (const PluginSubject::MotionConstPtr&, const QString&, const QIcon&, const QIcon&)> BranchFunction;
public:
    BuilderFilterCommand(BranchFunction function, const QIcon& rootIcon = QIcon(), const QIcon& elementIcon = QIcon()) : 
        branchFunction(function),
        elementIcon(elementIcon),
        rootIcon(rootIcon)
     { }

public:
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions ) 
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

protected:
    BranchFunction branchFunction;
    QIcon elementIcon;
    QIcon rootIcon;
};

template <class Collection>
class BuilderConfiguredFilterCommand : public BuilderFilterCommand
{
public:
    typedef typename core::ObjectWrapperT<Collection>::Ptr CollectionPtr;
    typedef typename core::ObjectWrapperT<Collection>::ConstPtr CollectionConstPtr;

public:
    BuilderConfiguredFilterCommand(BranchFunction function, const NamesDictionary& namesDictionary, 
        const QString& frontXml, const QString& backXml, const QIcon& rootIcon = QIcon(), const QIcon& elementIcon = QIcon()) :
          BuilderFilterCommand(function, rootIcon, elementIcon),
          helper(boost::bind( &BuilderConfiguredFilterCommand::checkBoxChanged, this, _1, _2 )),
          frontXml(frontXml),
          backXml(backXml),
          configurationWidget(nullptr),
          parentWidget(nullptr)
      {
        helper.setNamesDictionary(namesDictionary);
      }

      void checkBoxChanged (const QString& box, int state )
      {
          std::string name = box.toStdString(); 
          activeElements[name] = (bool)state;
      }

private:
    void createNameDictionary(const CollectionConstPtr & collection)
    {
        int count = collection->getNumChannels();
        for (int i = 0; i < count; i++) {
            ChannelConstPtr channel = collection->getChannel(i);
            std::string name = channel->getName();
            auto it = activeElements.find(name);
            if (it == activeElements.end()) {
                activeElements[name] = true;
            }
        }
    }

public:
    virtual void configurationStop( ConfigurationResult result ) 
    {
        if (result == Cancel) {
            activeElements = tempNameDictionary;
        }
    }

public:
    virtual QWidget* getConfigurationWidget()
    {
        if (!configurationWidget) {
            parentWidget = new QWidget(nullptr);
            QLayout* h = new QHBoxLayout();
            h->setMargin(0);
            h->setContentsMargins(0, 0, 0, 0);
            configurationWidget = new ConfigurationWidget(nullptr);
            QWidget* left = new QWidget();
            QWidget* right = new QWidget();
            left->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            right->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            h->addWidget(left);
            h->addWidget(configurationWidget);
            h->addWidget(right);
            parentWidget->setLayout(h);
            configurationWidget->loadConfigurations(frontXml, backXml, helper.getNamesDictionary());
            QObject::connect(configurationWidget, SIGNAL(itemSelected(const QString&, bool)), &helper, SLOT(onItemSelected(const QString&, bool)));
            QObject::connect(configurationWidget, SIGNAL(elementHovered(const QString&, bool)), &helper, SLOT(onElementHovered(const QString&, bool)));
        }
        return parentWidget;
    }

    virtual void reset()
    {
        for (auto it = activeElements.begin(); it != activeElements.end(); it++) {
            it->second = true;
        }
        if (configurationWidget) {
            configurationStart();
            configurationWidget->repaint();
        }
    }

    virtual void configurationStart() 
    {
        tempNameDictionary = activeElements;
        std::map<QString, bool> visibles;
        const NamesDictionary& names = helper.getNamesDictionary();
        for (auto elementIT = activeElements.begin(); elementIT != activeElements.end(); elementIT++) {
            for (auto nameIT = names.cbegin(); nameIT != names.cend(); nameIT++) {
                if (nameIT->second.first.toStdString() == elementIT->first) {
                    visibles[nameIT->first] = elementIT->second;
                }
            }
        }
        configurationWidget->setVisibles(visibles);
    }


    


    //! 
    //! \param rootItemName 
    //! \param sessions fdsdsd
    virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions)
    {
        QTreeWidgetItem* root = BuilderFilterCommand::createTreeBranch(rootItemName, sessions);
        filterTree(root);
        return root;
    }

private:
    virtual void filterTree(QTreeWidgetItem* item)
    {
        if (item->childCount()) {
            for (int i = item->childCount() - 1; i >= 0; --i) {
                filterTree(item->child(i));
            }
        } else {
            std::string name = item->text(0).toStdString();
            auto entry = activeElements.find(name);
            if (entry != activeElements.end() && !entry->second) {
                delete item;
            }
        }
       
    }

protected:
    std::map<std::string, bool> activeElements;
    std::map<std::string, bool> tempNameDictionary;
    PluginSubject::DataFilterPtr simpleTypeFilter;
    __Helper helper;
    QString frontXml, backXml;
    ConfigurationWidget* configurationWidget;
    QWidget* parentWidget;
};

class EMGCommand : public BuilderConfiguredFilterCommand<EMGCollection>
{
public:
    EMGCommand(BranchFunction function, const NamesDictionary& namesDictionary, 
        const QString& frontXml, const QString& backXml, const QIcon& rootIcon = QIcon(), const QIcon& elementIcon = QIcon()) :
        BuilderConfiguredFilterCommand(function, namesDictionary, frontXml, backXml, rootIcon, elementIcon) {}

public:
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions ) 
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

    MeasurementConfigConstPtr getMeta( PluginSubject::SessionConstPtr session) 
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


};



template <class Channel, class Collection, class ItemHelper, bool useTreeItemHelperForRoot = false>
class Vector3DFilterCommand : public IFilterCommand
{
public:

    typedef typename core::ObjectWrapperT<Collection>::Ptr CollectionPtr;
    typedef typename core::ObjectWrapperT<Collection>::ConstPtr CollectionConstPtr;

    typedef typename core::ObjectWrapperT<Channel>::Ptr ChannelPtr;
    typedef typename core::ObjectWrapperT<Channel>::ConstPtr ChannelConstPtr;

public:

      Vector3DFilterCommand() :
          simpleTypeFilter(new TypeFilter(typeid(Collection))),
          helper(boost::bind( &Vector3DFilterCommand::checkBoxChanged, this, _1, _2 ))
      {

      }

      //! 
      //! \param rootItemName 
      //! \param sessions
      virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions)
      {
          QTreeWidgetItem* root = new QTreeWidgetItem();
          root->setText(0, rootItemName);
          BOOST_FOREACH(SessionConstPtr session, sessions) {
              SessionPtr filtered = simpleTypeFilter->filterData(session);
              Motions motions;
              filtered->getMotions(motions);
              BOOST_FOREACH(MotionConstPtr motion, motions) {
                  std::vector<core::ObjectWrapperConstPtr> objects;
                  motion->getWrappers(objects, typeid(Collection));
                  if (objects.size() == 1) {
                      QTreeWidgetItem* motionItem;
                      if (useTreeItemHelperForRoot) {
                          TreeWrappedItemHelper* wrapped = new TreeWrappedItemHelper(objects[0]);
                          wrapped->setMotion(motion);
                          motionItem = wrapped;
                      } else {
                          motionItem = new QTreeWidgetItem();
                      }
                      motionItem->setText(0, motion->getLocalName().c_str());
                      root->addChild(motionItem);

                      CollectionConstPtr collection = objects[0]->get();
                      int count = collection->getNumChannels();
                      for (int i = 0 ; i < count; i++) {
                          ChannelConstPtr channel = collection->getChannel(i);
                          auto entry = activeElements.find(channel->getName());
                          if (entry == activeElements.end() || (entry != activeElements.end() && entry->second)) {

                              core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<Channel>();

                              //wrapper->set(boost::dynamic_pointer_cast<Channel>(collection->getChannel(i)));
                              wrapper->set(core::const_pointer_cast<Channel>(channel));
                              static int number = 0;
                              std::string name = "serie_" + boost::lexical_cast<std::string>(number);
                              wrapper->setName(name);
                              wrapper->setSource(name);
                              ItemHelper* treeItem = new ItemHelper(wrapper);
                              treeItem->setMotion(motion);
                              treeItem->setText(0, channel->getName().c_str());
                              motionItem->addChild(treeItem);
                          }
                      }
                  }
              }
          }

          return root;
      }
      virtual QDialog* getConfigurationDialog(QWidget* parent)
      {
          std::vector<SessionConstPtr> sessions;
          core::queryDataPtr(core::getDataManagerReader(), sessions);
          BOOST_FOREACH(SessionConstPtr session, sessions) {
              SessionPtr filtered = simpleTypeFilter->filterData(session);
              Motions motions;
              filtered->getMotions(motions);
              BOOST_FOREACH(MotionConstPtr motion, motions) {
                  //QTreeWidgetItem* motionItem = new QTreeWidgetItem();
                  std::vector<core::ObjectWrapperConstPtr> objects;
                  motion->getWrappers(objects, typeid(Collection));
                  BOOST_FOREACH(ObjectWrapperConstPtr wrapper, objects) {
                      CollectionConstPtr collection = wrapper->get();
                      createNameDictionary(collection);
                  }
              }
          }

          QDialog* widget = new QDialog(parent);
          QVBoxLayout* vlayout = new QVBoxLayout(widget);
          widget->setLayout(vlayout);
          for (auto it = activeElements.begin(); it != activeElements.end(); it++) {
              QWidget* entry = new QWidget(widget);
              QHBoxLayout* hlayout = new QHBoxLayout(entry);
              entry->setLayout(hlayout);
              QLabel* label = new QLabel(entry);
              label->setText(it->first.c_str());
              hlayout->addWidget(label);
              QCheckBox* checkBox = new QCheckBox(entry);
              QObject::connect(checkBox, SIGNAL(stateChanged ( int)), &helper, SLOT(checkBoxChanged(int)));
              checkBox->setChecked(it->second);
              hlayout->addWidget(checkBox);
              vlayout->addWidget(entry);
          }

          return widget;
      }

      void checkBoxChanged (const QString& box, int state )
      {
          std::string name = box.toStdString(); 
          activeElements[name] = (bool)state;
      }

private:
    void createNameDictionary(const CollectionConstPtr & collection)
    {
        int count = collection->getNumChannels();
        for (int i = 0; i < count; i++) {
            ChannelConstPtr channel = collection->getChannel(i);
            std::string name = channel->getName();
            auto it = activeElements.find(name);
            if (it == activeElements.end()) {
                activeElements[name] = true;
            }
        }
    }

public:
    virtual void configurationStart() 
    {
        tempNameDictionary = activeElements;
    }

    virtual void configurationStop( ConfigurationResult result ) 
    {
        if (result == Cancel) {
            activeElements = tempNameDictionary;
        }
    }


protected:
    std::map<std::string, bool> activeElements;
    std::map<std::string, bool> tempNameDictionary;
    PluginSubject::DataFilterPtr simpleTypeFilter;
    __Helper helper;
};

template <class Channel, class Collection, class ItemHelper, bool useTreeItemHelperForRoot = false>
class Vector3DFilterCommand2 : public Vector3DFilterCommand<Channel, Collection, ItemHelper, useTreeItemHelperForRoot>
{
public:
    Vector3DFilterCommand2(const NamesDictionary& namesDictionary, const QString& frontXml, const QString& backXml) : 
      Vector3DFilterCommand(),
      frontXml(frontXml),
      backXml(backXml),
      dialog(nullptr)
    {
        helper.setNamesDictionary(namesDictionary);
    }

    virtual ~Vector3DFilterCommand2() 
    {
    }
                             
public:
    virtual QDialog* getConfigurationDialog( QWidget* parent) 
    {
        if (!dialog) {
            dialog = new ConfigurationDialog(parent);
            int w = dialog->width();
            int h = dialog->height();
            dialog->loadConfigurations(frontXml, backXml, helper.getNamesDictionary());
            QObject::connect(dialog, SIGNAL(itemSelected(const QString&, bool)), &helper, SLOT(onItemSelected(const QString&, bool)));
            QObject::connect(dialog, SIGNAL(elementHovered(const QString&, bool)), &helper, SLOT(onElementHovered(const QString&, bool)));

            w = dialog->width();
            h = dialog->height();
        }
        return dialog;
    }

    virtual void configurationStart() 
    {
        Vector3DFilterCommand::configurationStart();
        std::map<QString, bool> visibles;
        const NamesDictionary& names = helper.getNamesDictionary();
        for (auto elementIT = activeElements.begin(); elementIT != activeElements.end(); elementIT++) {
            for (auto nameIT = names.cbegin(); nameIT != names.cend(); nameIT++) {
                if (nameIT->second.first.toStdString() == elementIT->first) {
                    visibles[nameIT->first] = elementIT->second;
                }
            }
        }
        dialog->setVisibles(visibles);
    }

private:
    QString frontXml, backXml;
    ConfigurationWidget* dialog;
};


class JointsCommand : public IFilterCommand
{
public:
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions ) 
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
};
#endif



