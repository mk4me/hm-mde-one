/********************************************************************
	created:	2011/10/07
	created:	7:10:2011   9:59
	filename: 	FilterCommand.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__FILTERCOMMAND_H__
#define HEADER_GUARD_HMM__FILTERCOMMAND_H__

#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>

#include <utils/DataChannel.h>
#include <utils/DataChannelCollection.h>
#include <core/SmartPtr.h>

#include <plugins/subject/Session.h>
#include <plugins/subject/DataFilter.h>
#include "TreeBuilder.h"

class IFilterCommand
{
public:
    enum ConfigurationResult
    {
        OK,
        Cancel
    };
public:
	virtual ~IFilterCommand() {}
   
public:
    virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions) = 0;
    virtual QDialog* getConfigurationDialog(QWidget* parent) { return nullptr; }
    virtual void configurationStart() {}
    virtual void configurationStop(ConfigurationResult result) {}
};
typedef boost::shared_ptr<IFilterCommand> IFilterCommandPtr;
typedef boost::shared_ptr<const IFilterCommand> IFilterCommandConstPtr;

class SimpleFilterCommand : public IFilterCommand
{
public:
    SimpleFilterCommand(DataFilterPtr dataFilter);

public:
    virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions) 
    {
        return TreeBuilder::createTree(rootItemName, sessions, dataFilter);
    }
    
private:
    DataFilterPtr dataFilter;
};
typedef boost::shared_ptr<SimpleFilterCommand> SimpleFilterCommandPtr;
typedef boost::shared_ptr<const SimpleFilterCommand> SimpleFilterCommandConstPtr;

template <class Type, class TypePtr = core::shared_ptr<Type> >
class MultiChartCommand : public IFilterCommand
{
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const std::vector<SessionConstPtr>& sessions );

};

template <class Type, class TypePtr>
QTreeWidgetItem* MultiChartCommand<Type, TypePtr>::createTreeBranch( const QString& rootItemName, const std::vector<SessionConstPtr>& sessions )
{
    QTreeWidgetItem* rootItem = new QTreeWidgetItem();
    rootItem->setText(0, rootItemName);
    BOOST_FOREACH(SessionConstPtr session, sessions)
    {
        BOOST_FOREACH(MotionPtr motion, session->getMotions()) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            rootItem->addChild(item);
            item->setText(0, motion->getName().c_str());
            if (motion->hasObjectOfType(typeid(Type))) {
                ObjectWrapperPtr wrapper = motion->getWrapperOfType(typeid(Type));
                TypePtr collection = wrapper->get();
                std::vector<ObjectWrapperPtr> xWrappers;
                std::vector<ObjectWrapperPtr> yWrappers;
                std::vector<ObjectWrapperPtr> zWrappers;
                for( int i = 0; i < collection->getNumChannels(); i++) {
                    Type::ChannelPtr f = collection->getChannel(i);
                    ScalarChannelReaderInterfacePtr x(new VectorToScalarAdaptor(f, 0));
                    ScalarChannelReaderInterfacePtr y(new VectorToScalarAdaptor(f, 1));
                    ScalarChannelReaderInterfacePtr z(new VectorToScalarAdaptor(f, 2));

                    core::ObjectWrapperPtr wrapperX = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
                    core::ObjectWrapperPtr wrapperY = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
                    core::ObjectWrapperPtr wrapperZ = core::ObjectWrapper::create<ScalarChannelReaderInterface>();

                    wrapperX->set(x);
                    wrapperY->set(y);
                    wrapperZ->set(z);

                    static int number = 0;
                    // hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
                    std::string suffix = boost::lexical_cast<std::string>(number++);
                    wrapperX->setName("FX_" + suffix);
                    wrapperX->setSource("FX_" + suffix);
                    wrapperY->setName("FY_" + suffix);
                    wrapperY->setSource("FY_" + suffix);
                    wrapperZ->setName("FZ_" + suffix);
                    wrapperZ->setSource("FZ_" + suffix);

                    xWrappers.push_back(wrapperX);
                    yWrappers.push_back(wrapperY);
                    zWrappers.push_back(wrapperZ);
                }
                QTreeWidgetItem* fX = new HmmTreePolicyItem<MultiserieHelper>(xWrappers);
                fX->setText(0, "Motion Forces - X");
                item->addChild(fX);

                QTreeWidgetItem* fY = new HmmTreePolicyItem<MultiserieHelper>(yWrappers);
                fY->setText(0, "Motion Forces - Y");
                item->addChild(fY);

                QTreeWidgetItem* fZ = new HmmTreePolicyItem<MultiserieHelper>(zWrappers);
                fZ->setText(0, "Motion Forces - Z");
                item->addChild(fZ);
            }
        }
    }

    return rootItem;
}

#endif
