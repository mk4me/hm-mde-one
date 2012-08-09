/********************************************************************
	created:	2011/10/07
	created:	7:10:2011   9:59
	filename: 	FilterCommand.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMM__FILTERCOMMAND_H__
#define HEADER_GUARD_HMM__FILTERCOMMAND_H__

#include <utils/DataChannel.h>
#include <utils/DataChannelCollection.h>
#include <core/SmartPtr.h>

#include <core/SubjectDataFilters.h>
#include <plugins/subject/Types.h>
#include "TreeBuilder.h"


//! interfejs odpowiada za tworzenie przefiltrowanego drzewa danych lub jego czesci
class IFilterCommand
{
public:
    //! jesli komenda dostarcza konfiguratora to powinien on zwrocic jedna z tych wartosci
    enum ConfigurationResult
    {
        OK,
        Cancel
    };

public:
	virtual ~IFilterCommand() {}

public:
    //! tworzy galaz drzewa z przefiltrowanymi danymi
    //! \param rootItemName nazwa korzenia
    //! \param sessions sesje do przefiltrowania
    virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions) = 0;
    //! resetuje ustawienia konfiguratora
    virtual void reset() {}
    //! \return widget z konfiguratorem lub nullptr jesli nie jest on dostarczany
    virtual QWidget* getConfigurationWidget() { return nullptr; }
};
typedef boost::shared_ptr<IFilterCommand> IFilterCommandPtr;
typedef boost::shared_ptr<const IFilterCommand> IFilterCommandConstPtr;

//! komenda bez konfiguratora, ktora tworzy przefiltrowane drzewo na podstawie filtra danych
class SimpleFilterCommand : public IFilterCommand
{
public:
    //! Konstruktor
    //! \param dataFilter filtr danych, ktory bedzie uzyty do tworzenia drzewa
    SimpleFilterCommand(PluginSubject::DataFilterPtr dataFilter);

public:
    //! tworzy galaz drzewa z przefiltrowanymi danymi
    //! \param rootItemName nazwa korzenia
    //! \param sessions sesje do przefiltrowania
    virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions)
    {
        return TreeBuilder::createTree(rootItemName, sessions, dataFilter);
    }

private:
    //! filtr danych, ktory bedzie uzyty do tworzenia drzewa
    PluginSubject::DataFilterPtr dataFilter;
};
typedef boost::shared_ptr<SimpleFilterCommand> SimpleFilterCommandPtr;
typedef boost::shared_ptr<const SimpleFilterCommand> SimpleFilterCommandConstPtr;

// OBSOLETE!
template <class Type, class TypePtr = core::shared_ptr<Type> >
class MultiChartCommand : public IFilterCommand
{
    //! tworzy galaz drzewa z przefiltrowanymi danymi
    //! \param rootItemName nazwa korzenia
    //! \param sessions sesje do przefiltrowania
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions );
};

template <class Type, class TypePtr>
QTreeWidgetItem* MultiChartCommand<Type, TypePtr>::createTreeBranch( const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions )
{
    QTreeWidgetItem* rootItem = new QTreeWidgetItem();
    rootItem->setText(0, rootItemName);
    for (auto it = sessions.begin(); it != sessions.end(); ++it)
    {
        auto session = *it;
        std::vector<PluginSubject::MotionConstPtr> motions;
        session->getMotions(motions);
        for (auto it = motions.begin(); it != motions.end(); ++it) {
            auto motion = *it;
            QTreeWidgetItem* item = new QTreeWidgetItem();
            rootItem->addChild(item);
            item->setText(0, motion->getLocalName().c_str());
            if (motion->hasObjectOfType(typeid(Type))) {
                core::ObjectWrapperConstPtr wrapper = motion->getWrapperOfType(typeid(Type));
                TypePtr collection = wrapper->get();
                std::vector<core::ObjectWrapperConstPtr> xWrappers;
                std::vector<core::ObjectWrapperConstPtr> yWrappers;
                std::vector<core::ObjectWrapperConstPtr> zWrappers;
                for( int i = 0; i < collection->getNumChannels(); ++i) {
                    typename Type::ChannelPtr f = collection->getChannel(i);
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
                    wrapperX->setName("X_" + suffix);
                    wrapperX->setSource("X_" + suffix);
                    wrapperY->setName("Y_" + suffix);
                    wrapperY->setSource("Y_" + suffix);
                    wrapperZ->setName("Z_" + suffix);
                    wrapperZ->setSource("Z_" + suffix);

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
