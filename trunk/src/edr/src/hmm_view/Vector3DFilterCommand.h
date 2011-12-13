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
        ConfigurationDialog* dialog = qobject_cast<ConfigurationDialog*>(sender());
        if (selected) {
            auto it = namesDictionary.find(name);
            if (it != namesDictionary.end()) {
                dialog->setText(it->second.second);
            } else {
                // ?
                dialog->setText("");
            }
        } else {
            dialog->setText("");
        }
    }

public:
    void setNamesDictionary(const NamesDictionary& val) { namesDictionary = val; }
    const NamesDictionary& getNamesDictionary() const { return namesDictionary; }

private:
    boost::function<void (const QString&, int)> function;
    NamesDictionary namesDictionary;
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

      virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions)
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
                          motionItem = new TreeWrappedItemHelper(objects[0]);
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
                              QTreeWidgetItem* treeItem = new ItemHelper(wrapper);
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
                  QTreeWidgetItem* motionItem = new QTreeWidgetItem();
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
    DataFilterPtr simpleTypeFilter;
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
    ConfigurationDialog* dialog;
};


class JointsCommand : public IFilterCommand
{
public:
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const std::vector<SessionConstPtr>& sessions ) 
    {
        QTreeWidgetItem* root = new QTreeWidgetItem();
        root->setText(0, rootItemName);
        BOOST_FOREACH(SessionConstPtr session, sessions) {
            Motions motions;
            session->getMotions(motions);
            BOOST_FOREACH(MotionConstPtr motion, motions) {
                root->addChild(TreeBuilder::createJointsBranch(motion, motion->getLocalName().c_str(), QIcon()));
            }
        }

        return root;
    }
};
#endif



