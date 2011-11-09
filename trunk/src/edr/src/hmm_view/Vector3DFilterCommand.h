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

#include <utils/DataChannel.h>
#include <utils/DataChannelCollection.h>
#include <core/SmartPtr.h>

#include <plugins/subject/Session.h>
#include <plugins/subject/DataFilter.h>

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

template <class Channel, class CollectionPtr, class ItemHelper>
class Vector3DFilterCommand : public IFilterCommand
{
public:
      Vector3DFilterCommand(core::IMemoryDataManager * memoryDataManager) :
          memoryDataManager(memoryDataManager),
          simpleTypeFilter(new TypeFilter(memoryDataManager, typeid(CollectionPtr::element_type))),
          helper(boost::bind( &Vector3DFilterCommand::checkBoxChanged, this, _1, _2 ))
      {

      }

      virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions)
      {
          QTreeWidgetItem* root = new QTreeWidgetItem();
          root->setText(0, rootItemName);
          BOOST_FOREACH(SessionConstPtr session, sessions) {
              SessionPtr filtered = simpleTypeFilter->filterData(session);
              BOOST_FOREACH(MotionPtr motion, filtered->getMotions()) {
                  QTreeWidgetItem* motionItem = new QTreeWidgetItem();
                  motionItem->setText(0, motion->getName().c_str());
                  root->addChild(motionItem);
                  BOOST_FOREACH(ObjectWrapperPtr wrapper, motion->getWrappers(typeid(CollectionPtr::element_type))) {
                      CollectionPtr collection = wrapper->get();
                      int count = collection->getNumChannels();
                      for (int i = 0 ; i < count; i++) {
                          __ChannelPtr channel = collection->getChannel(i);
                          auto entry = activeElements.find(channel->getName());
                          if (entry == activeElements.end() || (entry != activeElements.end() && entry->second)) {

                              core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<Channel>();

                              //wrapper->set(boost::dynamic_pointer_cast<Channel>(collection->getChannel(i)));
                              wrapper->set(channel);
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
          std::vector<SessionConstPtr> sessions = core::queryDataPtr();
          BOOST_FOREACH(SessionConstPtr session, sessions) {
              SessionPtr filtered = simpleTypeFilter->filterData(session);
              BOOST_FOREACH(MotionPtr motion, filtered->getMotions()) {
                  QTreeWidgetItem* motionItem = new QTreeWidgetItem();
                  BOOST_FOREACH(ObjectWrapperPtr wrapper, motion->getWrappers(typeid(CollectionPtr::element_type))) {
                      CollectionPtr collection = wrapper->get();
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
    typedef CollectionPtr __CollectionPtr;
    typedef core::shared_ptr<Channel> __ChannelPtr;

private:
    void createNameDictionary(__CollectionPtr collection)
    {
        int count = collection->getNumChannels();
        for (int i = 0; i < count; i++) {
            __ChannelPtr channel = collection->getChannel(i);
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
    core::IMemoryDataManager * memoryDataManager;
};

template <class Channel, class CollectionPtr, class ItemHelper>
class Vector3DFilterCommand2 : public Vector3DFilterCommand<Channel, CollectionPtr, ItemHelper>
{
public:
    Vector3DFilterCommand2(core::IMemoryDataManager * memoryDataManager, const NamesDictionary& namesDictionary, const QString& frontXml, const QString& backXml) : 
      Vector3DFilterCommand(memoryDataManager),
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
#endif



