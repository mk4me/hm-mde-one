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

#include <core/SubjectDataFilters.h>
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
    virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions) = 0;
    virtual void reset() {}
    virtual QWidget* getConfigurationWidget() { return nullptr; }
};
typedef boost::shared_ptr<IFilterCommand> IFilterCommandPtr;
typedef boost::shared_ptr<const IFilterCommand> IFilterCommandConstPtr;

class SimpleFilterCommand : public IFilterCommand
{
public:
    SimpleFilterCommand(PluginSubject::DataFilterPtr dataFilter);

public:
    virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions) 
    {
        return TreeBuilder::createTree(rootItemName, sessions, dataFilter);
    }
    
private:
    PluginSubject::DataFilterPtr dataFilter;
};
typedef boost::shared_ptr<SimpleFilterCommand> SimpleFilterCommandPtr;
typedef boost::shared_ptr<const SimpleFilterCommand> SimpleFilterCommandConstPtr;

template <class Type, class TypePtr = core::shared_ptr<Type> >
class MultiChartCommand : public IFilterCommand
{
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions );

};

template <class Type, class TypePtr>
QTreeWidgetItem* MultiChartCommand<Type, TypePtr>::createTreeBranch( const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions )
{
    QTreeWidgetItem* rootItem = new QTreeWidgetItem();
    rootItem->setText(0, rootItemName);
    BOOST_FOREACH(SessionConstPtr session, sessions)
    {
        std::vector<MotionConstPtr> motions;
        session->getMotions(motions);
        BOOST_FOREACH(MotionConstPtr motion, motions) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            rootItem->addChild(item);
            item->setText(0, motion->getLocalName().c_str());
            if (motion->hasObjectOfType(typeid(Type))) {
                ObjectWrapperConstPtr wrapper = motion->getWrapperOfType(typeid(Type));
                TypePtr collection = wrapper->get();
                std::vector<ObjectWrapperConstPtr> xWrappers;
                std::vector<ObjectWrapperConstPtr> yWrappers;
                std::vector<ObjectWrapperConstPtr> zWrappers;
                for( int i = 0; i < collection->getNumChannels(); ++i) {
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

                NewMultiserieHelperPtr xhelper(new NewMultiserieHelper(xWrappers));
                NewMultiserieHelperPtr yhelper(new NewMultiserieHelper(yWrappers));
                NewMultiserieHelperPtr zhelper(new NewMultiserieHelper(zWrappers));
                xhelper->setMotion(motion);
                yhelper->setMotion(motion);
                zhelper->setMotion(motion);
                HmmTreeItem* fX = new HmmTreeItem(xhelper);
                fX->setItemAndHelperText("Motion Items - X");
                item->addChild(fX);

                HmmTreeItem* fY = new HmmTreeItem(yhelper);
                fY->setItemAndHelperText("Motion Items - Y");
                item->addChild(fY);

                HmmTreeItem* fZ = new HmmTreeItem(zhelper);
                fZ->setItemAndHelperText("Motion Items - Z");
                item->addChild(fZ);
            }
        }
    }

    return rootItem;
}

#endif
