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
#include "configurationDialog.h"

class __Helper : public QObject
{
    Q_OBJECT;
public:
    __Helper(boost::function<void (const QString&, int)> function) :
      function(function) 
      {
      }
      public slots:
          void checkBoxChanged ( int state )
          {
              QCheckBox* checkBox = qobject_cast<QCheckBox *>(sender());
              function(checkBox->text(), state);
          }

          void onItemSelected(const QString& name, bool selected)
          {
              function(name, selected ? 1 : 0);
          }

private:
    boost::function<void (const QString&, int)> function;
};



template <class Channel, class CollectionPtr>
class Vector3DFilterCommand : public IFilterCommand
{
    
    
public:
    Vector3DFilterCommand(core::IMemoryDataManager * memoryDataManager) :
      memoryDataManager(memoryDataManager),
      simpleTypeFilter(new TypeFilter(memoryDataManager, typeid(CollectionPtr::element_type))),
      helper(boost::bind( &Vector3DFilterCommand::checkBoxChanged, this, _1, _2 ))
      {

      }
      virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions);
      virtual QWidget* getConfigurationWidget(QWidget* parent);

      void checkBoxChanged (const QString& box, int state )
      {
          std::string name = box2Name[box];
          nameDictionary[name] = (bool)state;
      }

private:
    typedef CollectionPtr __CollectionPtr;
    typedef core::shared_ptr<Channel> __ChannelPtr;
    void createNameDictionary(__CollectionPtr collection);



protected:
    std::map<std::string, bool> nameDictionary;
    std::map<QString, std::string> box2Name;
    DataFilterPtr simpleTypeFilter;
    __Helper helper;
    core::IMemoryDataManager * memoryDataManager;
};

template <class Channel, class CollectionPtr>
QWidget* Vector3DFilterCommand<Channel, CollectionPtr>::getConfigurationWidget(QWidget* parent)
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

    box2Name.clear();
    QWidget* widget = new QWidget(parent);
    QVBoxLayout* vlayout = new QVBoxLayout(widget);
    widget->setLayout(vlayout);
    for (auto it = nameDictionary.begin(); it != nameDictionary.end(); it++) {
        QWidget* entry = new QWidget(widget);
        QHBoxLayout* hlayout = new QHBoxLayout(entry);
        entry->setLayout(hlayout);
        QLabel* label = new QLabel(entry);
        label->setText(it->first.c_str());
        hlayout->addWidget(label);
        QCheckBox* checkBox = new QCheckBox(entry);
        // todo
        QString cbName = it->first.c_str();
        box2Name[cbName] = it->first;
        QObject::connect(checkBox, SIGNAL(stateChanged ( int)), &helper, SLOT(checkBoxChanged(int)));
        checkBox->setChecked(it->second);
        hlayout->addWidget(checkBox);
        vlayout->addWidget(entry);
    }

    return widget;
}

template <class Channel, class CollectionPtr>
QTreeWidgetItem* Vector3DFilterCommand<Channel, CollectionPtr>::createTreeBranch( const QString& rootItemName, const std::vector<SessionConstPtr>& sessions )
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
                    auto entry = nameDictionary.find(channel->getName());
                    if (entry == nameDictionary.end() || (entry != nameDictionary.end() && entry->second)) {

                        core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<Channel>();

                        //wrapper->set(boost::dynamic_pointer_cast<Channel>(collection->getChannel(i)));
                        wrapper->set(channel);
                        static int number = 0;
                        std::string name = "serie_" + boost::lexical_cast<std::string>(number);
                        wrapper->setName(name);
                        wrapper->setSource(name);
                        QTreeWidgetItem* treeItem = new HmmTreePolicyItem<Vector3ItemHelper>(wrapper);
                        treeItem->setText(0, channel->getName().c_str());
                        motionItem->addChild(treeItem);
                    }
                }
            }
        }
    }
    return root;
}

template <class Channel, class CollectionPtr>
void Vector3DFilterCommand<Channel, CollectionPtr>::createNameDictionary( __CollectionPtr collection )
{
    int count = collection->getNumChannels();
    for (int i = 0; i < count; i++) {
        __ChannelPtr channel = collection->getChannel(i);
        std::string name = channel->getName();
        auto it = nameDictionary.find(name);
        if (it == nameDictionary.end()) {
            nameDictionary[name] = true;
        }
    }
}

template <class Channel, class CollectionPtr>
class Vector3DFilterCommand2 : public Vector3DFilterCommand<Channel, CollectionPtr>
{
public:
    enum Configuration
    {
        Skeletal,
        Muscular
    };
public:
    Vector3DFilterCommand2(core::IMemoryDataManager * memoryDataManager,Configuration configuration) : 
      Vector3DFilterCommand(memoryDataManager),
      configuration(configuration)
    {
        box2Name["Marker_0"] = "LAnkleMoment";
        box2Name["Marker_1"] = "LElbowMoment";
        box2Name["Marker_2"] = "LGroundReactionMoment";
        box2Name["Marker_3"] = "LHipMoment";
        box2Name["Marker_4"] = "LKneeMoment";
        box2Name["Marker_5"] = "LNeckMoment";
        box2Name["Marker_6"] = "LShoulderMoment";
        box2Name["Marker_7"] = "LWaistMoment";
        box2Name["Marker_8"] = "LWristMoment";
        box2Name["Marker_9"] = "RAnkleMoment";
        box2Name["Marker_10"] = "RElbowMoment";
        box2Name["Marker_11"] = "RGroundReactionMoment";
        box2Name["Marker_12"] = "RHipMoment";
        box2Name["Marker_13"] = "RKneeMoment";
        box2Name["Marker_14"] = "RNeckMoment";
        box2Name["Marker_15"] = "RShoulderMoment";
        box2Name["Marker_16"] = "RWaistMoment";
        box2Name["Marker_17"] = "RWristMoment";
   }
                             
public:
    virtual QWidget* getConfigurationWidget( QWidget* parent) 
    {
        ConfigurationDialog* dialog = new ConfigurationDialog(parent);
        int w = dialog->width();
        int h = dialog->height();
        if (configuration == Muscular) {
            QString pathFront = core::getResourceString("images/muscular_front/muscular_front.xml");
            QString pathBack = core::getResourceString("images/muscular_back/muscular_back.xml");
            dialog->loadConfigurations(pathFront, pathBack);
        } else if (configuration == Skeletal) {
            QString pathFront = core::getResourceString("images/skeleton_front/skeleton_front.xml");
            QString pathBack = core::getResourceString("images/skeleton_back/skeleton_back.xml");
            dialog->loadConfigurations(pathFront, pathBack);
        }
        QObject::connect(dialog, SIGNAL(itemSelected(const QString&, bool)), &helper, SLOT(onItemSelected(const QString&, bool)));

        w = dialog->width();
        h = dialog->height();
        return dialog;
    }

private:
    Configuration configuration;
};


#endif



